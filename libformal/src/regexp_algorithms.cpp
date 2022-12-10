#include <libformal/regexp_algorithms.hpp>

namespace formal {
    int GetPrefixedMin(const std::string& regexp, char letter, int count) {
        formal::PrefixedMinWalker walker(letter, count);
        formal::PrefixedMinResult result = formal::ProcessRPNRegExp(regexp, walker);
        return result.min_prefixed_len[count];
    }

    PrefixedMinResult PrefixedMinWalker::ProcessSingleLetter(char letter) {
        PrefixedMinResult result = InstantiateEmptyResult();
        result.min_prefixed_len[0] = 1;

        if (letter == pref_letter_ && count_ > 0) {
            result.has_prefix[1] = true;
            result.min_prefixed_len[1] = 1;
        }

        return result;
    }

    PrefixedMinResult PrefixedMinWalker::ProcessEpsilon() {
        PrefixedMinResult result = InstantiateEmptyResult();
        result.min_prefixed_len[0] = 0;
        result.has_prefix[0] = true;
        return result;
    }

    PrefixedMinResult PrefixedMinWalker::ProcessUnion(PrefixedMinResult a, PrefixedMinResult b) {
        for (int i = 0; i <= count_; i++) {
            a.has_prefix[i] = a.has_prefix[i] || b.has_prefix[i];
            a.min_prefixed_len[i] = std::min(a.min_prefixed_len[i], b.min_prefixed_len[i]);
        }

        return a;
    }

    PrefixedMinResult PrefixedMinWalker::ProcessConcat(PrefixedMinResult a, PrefixedMinResult b) {
        PrefixedMinResult result = InstantiateEmptyResult();
        for (int x_count = 0; x_count <= count_; x_count++) {
            // has x^k if left has x^n, right has x^m and n+m=k
            for (int x_on_left = 0; x_on_left <= x_count; x_on_left++) {
                if (a.has_prefix[x_on_left] && b.has_prefix[x_count - x_on_left]) {
                    result.has_prefix[x_count] = true;
                    break;
                }
            }

            // try to split x^k.w between a.b
            for (int x_on_left = 0; x_on_left <= x_count; x_on_left++) {
                if (a.has_prefix[x_on_left] && b.min_prefixed_len[x_count - x_on_left] != INT_NONE) {
                    int len = x_on_left + b.min_prefixed_len[x_count - x_on_left];
                    result.min_prefixed_len[x_count] = std::min(result.min_prefixed_len[x_count], len);
                }
            }

            // uncovered case - x^k.w in a, but not x^k
            if (a.min_prefixed_len[x_count] != INT_NONE && b.min_prefixed_len[0] != INT_NONE) {
                int len = a.min_prefixed_len[x_count] + b.min_prefixed_len[0];
                result.min_prefixed_len[x_count] = std::min(result.min_prefixed_len[x_count], len);
            }
        }

        return result;
    }

    PrefixedMinResult PrefixedMinWalker::ProcessStar(PrefixedMinResult a) {
        // just do 1 + x^1 + ... + x^count

        PrefixedMinResult result = ProcessEpsilon();
        PrefixedMinResult power = a;
        for (int i = 0; i < count_; i++) {
            result = ProcessUnion(result, power);
            power = ProcessConcat(power, a);
        }

        return result;
    }
}