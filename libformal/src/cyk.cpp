#include <libformal/cyk.hpp>
#include <cassert>

namespace formal {
    CYKParser::CYKParser(const CFGrammar &grammar): grammar_(grammar) {
        if (!grammar.IsCNF()) {
            throw CYKParseError("Bad grammar - it must be in Chomsky normal form to be used by CYK");
        }
    }

    bool CYKParser::parse(const std::string& word) {
        // is_producible[A][l][r] - true if word[l:r] is producible by non-terminal A
        std::array<std::vector<std::vector<bool>>, ALPHABET_SIZE> is_producible{};

        if (word.empty()) {
            auto rules = grammar_[START_SYMBOL];
            for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                const CFGrammarRule& rule = rule_iter->second;
                if (rule.rhs.empty()) {
                    return true;
                }
            }

            return false;
        }

        // Prepare table
        for (CFNonTerminal nt = 'A'; nt <= 'Z'; nt++) {
            if (!grammar_.IsNonTermUsed(nt)) {
                continue;
            }

            is_producible[nt - 'A'].resize(word.size());
            for (int i = 0; i < word.size(); i++) {
                is_producible[nt - 'A'][i].resize(word.size());
            }
        }

        // l == r
        for (CFNonTerminal nt = 'A'; nt <= 'Z'; nt++) {
            if (!grammar_.IsNonTermUsed(nt)) {
                continue;
            }

            // Terminals that producible by nt
            std::array<bool, ALPHABET_SIZE> prod_terms_mask{};
            auto rules = grammar_[nt];
            for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                const CFGrammarRule& rule = rule_iter->second;
                if (rule.rhs.size() == 1) {
                    assert(IsTerminal(rule.rhs[0]));
                    prod_terms_mask[rule.rhs[0] - 'a'] = true;
                }
            }

            for (int l = 0; l < word.size(); l++) {
                is_producible[nt - 'A'][l][l] = prod_terms_mask[word[l] - 'a'];
//                if (prod_terms_mask[word[l] - 'a']) {
//                    fmt::print("{} [{}:{}] is producible by {}\n", word[l], l, l, nt);
//                }
            }
        }

        for (int len = 2; len <= word.size(); len++) {
            for (int l = 0; l <= word.size() - len; l++) {
                int r = l + len - 1;

                for (CFNonTerminal nt = 'A'; nt <= 'Z'; nt++) {
                    if (!grammar_.IsNonTermUsed(nt)) {
                        continue;
                    }

                    bool success = false;

                    auto rules = grammar_[nt];
                    for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                        const CFGrammarRule& rule = rule_iter->second;
                        if (rule.rhs.size() < 2) {
                            continue;
                        }

                        assert(rule.rhs.size() == 2 && !IsTerminal(rule.rhs[0]) && !IsTerminal(rule.rhs[1]));

                        for (int k = l; k < r; k++) {
                            if (is_producible[rule.rhs[0] - 'A'][l][k] && is_producible[rule.rhs[1] - 'A'][k + 1][r]) {
                                is_producible[nt - 'A'][l][r] = true;
//                                fmt::print("{} [{}:{}] is producible by {}\n", word.substr(l, len), l, r, nt);
                                success = true;
                                break;
                            }
                        }

                        if (success) {
                            break;
                        }
                    }
                }
            }
        }

        return is_producible[START_SYMBOL - 'A'][0][word.size() - 1];
    }
}
