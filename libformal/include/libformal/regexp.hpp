#pragma once

#include <fmt/core.h>
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace formal {
    class RegExpProcessError : public std::runtime_error
    {
    public:
        explicit RegExpProcessError(const std::string& what = "") : std::runtime_error(what) {}
    };

    template<typename Result>
    class IRegExpWalker {
    public:
        virtual Result InstantiateEmptyResult() = 0;
        virtual Result ProcessSingleLetter(char letter) = 0;
        virtual Result ProcessEpsilon() = 0;
        virtual Result ProcessUnion(Result a, Result b) = 0;
        virtual Result ProcessConcat(Result a, Result b) = 0;
        virtual Result ProcessStar(Result a) = 0;
    };

    class DummyResult {};

    class DummyRegExpWalker : public IRegExpWalker<DummyResult> {
    public:
        DummyResult InstantiateEmptyResult() override { return DummyResult(); }
        DummyResult ProcessSingleLetter(char letter) override { return DummyResult(); };
        DummyResult ProcessEpsilon() override { return DummyResult(); };
        DummyResult ProcessUnion(DummyResult a, DummyResult b) override { return DummyResult(); };
        DummyResult ProcessConcat(DummyResult a, DummyResult b) override { return DummyResult(); };
        DummyResult ProcessStar(DummyResult a) override { return DummyResult(); };
    };

    template<typename Result>
    Result ProcessRPNRegExp(const std::string& regexp, IRegExpWalker<Result>& algo) {
        std::vector<Result> result_stack;
        for (int pos = 0; pos < regexp.size(); pos++) {
            char c = regexp[pos];

            Result lhs = algo.InstantiateEmptyResult();
            Result rhs = algo.InstantiateEmptyResult();
            bool enough_args = true;

            switch (c) {
                case '+':
                case '.':
                    if (result_stack.size() < 2) {
                        enough_args = false;
                        break;
                    }

                    rhs = std::move(result_stack.back());
                    result_stack.pop_back();
                    lhs = std::move(result_stack.back());
                    result_stack.pop_back();
                    break;

                case '*':
                    if (result_stack.empty()) {
                        enough_args = false;
                        break;
                    }

                    lhs = std::move(result_stack.back());
                    result_stack.pop_back();
                    break;

                default:
                    break;
            }

            if (!enough_args) {
                throw RegExpProcessError(fmt::format("not enough operands for operation {} at pos {}", c, pos + 1));
            }

            switch (c) {
                case '1':
                    result_stack.push_back(algo.ProcessEpsilon());
                    break;

                case '+':
                    result_stack.push_back(algo.ProcessUnion(std::move(lhs), std::move(rhs)));
                    break;

                case '.':
                    result_stack.push_back(algo.ProcessConcat(std::move(lhs), std::move(rhs)));
                    break;

                case '*':
                    result_stack.push_back(algo.ProcessStar(std::move(lhs)));
                    break;

                default:
                    result_stack.push_back(algo.ProcessSingleLetter(c));
                    break;
            }
        }

        if (result_stack.size() > 1) {
            throw RegExpProcessError("unused operands were left");
        }

        return result_stack.back();
    }
}