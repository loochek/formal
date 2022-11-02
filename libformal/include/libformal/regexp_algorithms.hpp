#pragma once

#include <climits>
#include <libformal/regexp.hpp>
#include <vector>

namespace formal {
    const int INT_NONE = INT_MAX;

    /**
     * Determines length of the shortest word containing prefix x^k
     * that accepted by regular expression
     *
     * \param regexp Regular expression in Reverse Polish Notation
     * \param letter Letter x
     * \param count Value k
     * \return Specified length or INT_NONE if there is no such word
     */
    int GetPrefixedMin(const std::string& regexp, char letter, int count);

    struct PrefixedMinResult {
        /// Min length of word of type x^n.w accepted by regexp for each n = [0;count]
        std::vector<int> min_prefixed_len;
        /// Is word x^n accepted by regexp for each n = [0;count]
        std::vector<bool> has_prefix;

        PrefixedMinResult(int count) : min_prefixed_len(count + 1, INT_NONE), has_prefix(count + 1, false) {}

        PrefixedMinResult(const PrefixedMinResult& other) = default;
        PrefixedMinResult(PrefixedMinResult&& other) = default;

        PrefixedMinResult& operator=(const PrefixedMinResult& other) = default;
        PrefixedMinResult& operator=(PrefixedMinResult&& other) = default;
    };

    class PrefixedMinWalker : public IRegExpWalker<PrefixedMinResult> {
    public:
        PrefixedMinWalker(char pref_letter, int count) : pref_letter_(pref_letter), count_(count) {}

        PrefixedMinResult InstantiateEmptyResult() override {
            return PrefixedMinResult(count_);
        }

        PrefixedMinResult ProcessSingleLetter(char letter) override;
        PrefixedMinResult ProcessEpsilon() override;
        PrefixedMinResult ProcessUnion(PrefixedMinResult a, PrefixedMinResult b) override;
        PrefixedMinResult ProcessConcat(PrefixedMinResult a, PrefixedMinResult b) override;
        PrefixedMinResult ProcessStar(PrefixedMinResult a) override;

    private:
        char pref_letter_;
        int count_;
    };
};