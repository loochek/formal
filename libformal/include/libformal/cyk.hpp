#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fmt/core.h>
#include <libformal/grammar.hpp>

namespace formal {
    class CYKParseError : public std::runtime_error
    {
    public:
        explicit CYKParseError(const std::string& what = "") : std::runtime_error(what) {}
    };

    class CYKParser {
    public:
        explicit CYKParser(const CFGrammar& grammar);

        bool parse(const std::string& word);

    private:
        const CFGrammar& grammar_;
    };
}