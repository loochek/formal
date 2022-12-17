#include <libformal/utils.hpp>
#include <libformal/grammar.hpp>
#include <fmt/core.h>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cassert>

namespace formal {
    using GrammarChainClosures = std::unordered_map<CFNonTerminal, std::unordered_set<const std::string*>>;

    namespace {
        // TransformToCNF intermediate steps

        void ShortifyRules(CFGrammar& grammar);
        void DemixifyRules(CFGrammar& grammar);
        /// User must add S => . at the end if true is returned
        bool RemoveEpsNonTerminals(CFGrammar& grammar);
        void RemoveChainRules(CFGrammar& grammar);
        void RemoveUnreachableAndNonProducers(CFGrammar& grammar);

        // Help routines

        std::array<bool, ALPHABET_SIZE> FindGenerative(const CFGrammar& grammar);
        std::array<bool, ALPHABET_SIZE> FindEpsGenerative(const CFGrammar& grammar);
        void BuildChainClosure(CFNonTerminal nt, const CFGrammar& grammar, GrammarChainClosures& closures);
        void TraverseGrammar(CFNonTerminal nt, const CFGrammar& grammar, std::array<bool, ALPHABET_SIZE>& visited);
    }

    CFGrammarRule::CFGrammarRule(CFNonTerminal lhs, std::string rhs) : lhs(lhs), rhs(std::move(rhs)) {
        if (!std::isupper(this->lhs)) {
            throw GrammarProcessError(fmt::format(
                    "Bad rule \"{} => {}\" - single uppercase symbol is only allowed on the left side", lhs, rhs));
        }

        for (CFRuleSymbol symbol : this->rhs) {
            if (!std::isalpha(symbol)) {
                throw GrammarProcessError(fmt::format(
                        "Bad rule \"{} => {}\" - only alphabetic symbols is allowed on the right side", lhs, rhs));
            }
        }
    };

    CFGrammarRule ParseRuleFromString(std::string_view rule) {
        int lhs_size = rule.find(PROD_LEX);
        if (lhs_size == std::string::npos) {
            throw GrammarProcessError(fmt::format("Bad rule {} - no production lexeme", rule));
        }

        std::string_view lhs_str = strip(rule.substr(0, lhs_size));
        if (!(lhs_str.size() == 1 && std::isupper(lhs_str[0]))) {
            throw GrammarProcessError(fmt::format(
                    "Bad rule {} - left-hand side is not an capital letter", rule));
        }

        std::string_view rhs_str = strip(rule.substr(lhs_size + PROD_LEX.size(), std::string::npos));
        if (rhs_str.empty()) {
            throw GrammarProcessError(fmt::format(
                    "Bad rule {} - empty right-hand side is not allowed", rule));
        }

        if (rhs_str == EPS_RHS_LEX) {
            return CFGrammarRule(lhs_str[0], "");
        }

        return CFGrammarRule(lhs_str[0], std::string(rhs_str));
    }

    CFGrammar ParseGrammarFromString(const std::string &str) {
        CFGrammar grammar;

        std::string_view rem_string = str;
        while (true) {
            int rule_size = rem_string.find(RULE_SEP_LEX);
            if (rule_size == std::string::npos) {
                break;
            }

            grammar.AddRule(ParseRuleFromString(rem_string.substr(0, rule_size)));
            rem_string.remove_prefix(rule_size + RULE_SEP_LEX.size());
        }

        if (!rem_string.empty()) {
            // Final rule (if present)
            grammar.AddRule(ParseRuleFromString(rem_string));
        }

        return grammar;
    }

    void SinglifyEntryRule(CFGrammar& grammar) {
        CFGrammar new_grammar;

        CFNonTerminal start_replacement = grammar.GetUnusedNonTerm();
        for (auto& [lhs, rule] : grammar.GetRules()) {
            if (lhs == START_SYMBOL) {
                new_grammar.AddRule(CFGrammarRule(start_replacement, rule.rhs));
            } else {
                new_grammar.AddRule(rule);
            }
        }

        new_grammar.AddRule(CFGrammarRule(START_SYMBOL, std::string(1, start_replacement)));
        grammar = std::move(new_grammar);
    }

    void TransformToCNF(CFGrammar& grammar) {
        RemoveUnreachableAndNonProducers(grammar);
        DemixifyRules(grammar);
        ShortifyRules(grammar);
        bool eps_gen = RemoveEpsNonTerminals(grammar);
        RemoveChainRules(grammar);

        if (eps_gen) {
            grammar.AddRule(CFGrammarRule(START_SYMBOL, ""));
        }

        // Final cleanup
        RemoveUnreachableAndNonProducers(grammar);
    }

    namespace {
        void ShortifyRules(CFGrammar& grammar) {
            CFGrammar new_grammar;
            for (auto& [lhs, rule] : grammar.GetRules()) {
                if (rule.rhs.size() <= 2) {
                    new_grammar.AddRule(rule);
                    continue;
                }

                CFNonTerminal last_part = lhs;
                for (int i = 0; i < rule.rhs.size() - 2; i++) {
                    CFNonTerminal next_part = new_grammar.GetUnusedNonTerm();
                    new_grammar.AddRule(CFGrammarRule(last_part, fmt::format("{}{}", rule.rhs[i], next_part)));
                    last_part = next_part;
                }

                new_grammar.AddRule(CFGrammarRule(last_part, rule.rhs.substr(rule.rhs.size() - 2, 2)));
            }

            grammar = std::move(new_grammar);
        }

        void DemixifyRules(CFGrammar& grammar) {
            std::array<CFNonTerminal, ALPHABET_SIZE> alias_terms{};

            CFGrammar new_grammar;
            for (auto& [lhs, rule] : grammar.GetRules()) {
                std::string rule_draft = rule.rhs;
                for (int i = 0; i < rule_draft.size(); i++) {
                    CFRuleSymbol s = rule_draft[i];
                    if (IsTerminal(s)) {
                        if (alias_terms[s - 'a'] == 0) {
                            alias_terms[s - 'a'] = new_grammar.GetUnusedNonTerm(toupper(s));
                        }

                        rule_draft[i] = alias_terms[s - 'a'];
                    }
                }

                new_grammar.AddRule(CFGrammarRule(lhs, rule_draft));
            }

            for (CFRuleSymbol term = 'a'; term <= 'z'; term++) {
                if (alias_terms[term - 'a'] != 0) {
                    new_grammar.AddRule(CFGrammarRule(alias_terms[term - 'a'], std::string(1, term)));
                }
            }

            grammar = std::move(new_grammar);
        }

        bool RemoveEpsNonTerminals(CFGrammar& grammar) {
            std::array<bool, ALPHABET_SIZE> eps_generative_nts_mask = FindEpsGenerative(grammar);

            CFGrammar new_grammar;
            for (auto& [lhs, rule] : grammar.GetRules()) {
                assert(rule.rhs.size() <= 2);

                switch (rule.rhs.size()) {
                    case 0:
                        continue;

                    case 1:
                        new_grammar.AddRule(rule);
                        continue;

                    case 2:
                        new_grammar.AddRule(rule);

                        if (eps_generative_nts_mask[rule.rhs[0] - 'A']) {
                            assert(!eps_generative_nts_mask[rule.rhs[1] - 'A']);
                            new_grammar.AddRule(CFGrammarRule(lhs, std::string(1, rule.rhs[1])));
                        } else if (eps_generative_nts_mask[rule.rhs[1] - 'A']) {
                            assert(!eps_generative_nts_mask[rule.rhs[0] - 'A']);
                            new_grammar.AddRule(CFGrammarRule(lhs, std::string(1, rule.rhs[0])));
                        }
                }
            }

            grammar = std::move(new_grammar);
            return eps_generative_nts_mask[START_SYMBOL - 'A'];
        }

        void RemoveChainRules(CFGrammar& grammar) {
            GrammarChainClosures closures;

            CFGrammar new_grammar;
            for (auto& [lhs, rule] : grammar.GetRules()) {
                BuildChainClosure(lhs, grammar, closures);

                if (rule.rhs.size() == 2 || (rule.rhs.size() == 1 && IsTerminal(rule.rhs[0]))) {
                    new_grammar.AddRule(rule);
                }
            }

            for (CFNonTerminal nt = 'A'; nt <= 'Z'; nt++) {
                for (const std::string* clos_rhs : closures[nt]) {
                    new_grammar.AddRule(CFGrammarRule(nt, *clos_rhs));
                }
            }

            grammar = std::move(new_grammar);
        }

        void RemoveUnreachableAndNonProducers(CFGrammar& grammar) {
            std::array<bool, ALPHABET_SIZE> reachable_nts_mask{};
            TraverseGrammar(START_SYMBOL, grammar, reachable_nts_mask);

            std::array<bool, ALPHABET_SIZE> generative_nts_mask = FindGenerative(grammar);

            CFGrammar new_grammar;
            for (auto& [lhs, rule] : grammar.GetRules()) {
                if (!reachable_nts_mask[lhs - 'A'] || !generative_nts_mask[lhs - 'A']) {
                    continue;
                }

                bool bad_rule = false;
                for (CFNonTerminal s : rule.rhs) {
                    if (IsTerminal(s) && (!reachable_nts_mask[lhs - 'A'] || !generative_nts_mask[lhs - 'A'])) {
                        bad_rule = true;
                        continue;
                    }
                }

                if (bad_rule) {
                    continue;
                }

                new_grammar.AddRule(rule);
            }

            grammar = std::move(new_grammar);
        }

        std::array<bool, ALPHABET_SIZE> FindGenerative(const CFGrammar& grammar) {
            std::array<bool, ALPHABET_SIZE> generative_nts_mask{};

            std::unordered_map<CFGrammarRule, std::string> curr_rhses;
            std::queue<CFNonTerminal> scheduled_for_deletion;

            for (const auto& [lhs, rule] : grammar.GetRules()) {
                curr_rhses[rule] = rule.rhs;

                bool is_word = true;
                for (CFRuleSymbol s : rule.rhs) {
                    if (!IsTerminal(s)) {
                        is_word = false;
                        break;
                    }
                }

                if (is_word) {
                    generative_nts_mask[lhs - 'A'] = true;
                    scheduled_for_deletion.push(lhs);
                }
            }

            while (!scheduled_for_deletion.empty()) {
                CFNonTerminal to_delete = scheduled_for_deletion.front();
                scheduled_for_deletion.pop();

                for (const auto& [lhs, rule] : grammar.GetRules()) {
                    auto& curr_rhs = curr_rhses[rule];
                    curr_rhs.erase(std::remove(curr_rhs.begin(), curr_rhs.end(), to_delete), curr_rhs.end());
                    if (curr_rhs.empty() && !generative_nts_mask[lhs - 'A']) {
                        scheduled_for_deletion.push(lhs);
                        generative_nts_mask[lhs - 'A'] = true;
                    }
                }
            }

            return generative_nts_mask;
        }

        std::array<bool, ALPHABET_SIZE> FindEpsGenerative(const CFGrammar& grammar) {
            std::array<bool, ALPHABET_SIZE> eps_generative_nts_mask{};

            std::unordered_map<CFGrammarRule, std::string> curr_rhses;
            std::queue<CFNonTerminal> scheduled_for_deletion;

            for (const auto& [lhs, rule] : grammar.GetRules()) {
                curr_rhses[rule] = rule.rhs;
                if (rule.rhs.empty()) {
                    eps_generative_nts_mask[lhs - 'A'] = true;
                    scheduled_for_deletion.push(lhs);
                }
            }

            while (!scheduled_for_deletion.empty()) {
                CFNonTerminal to_delete = scheduled_for_deletion.front();
                scheduled_for_deletion.pop();

                for (const auto& [lhs, rule] : grammar.GetRules()) {
                    auto& curr_rhs = curr_rhses[rule];
                    curr_rhs.erase(std::remove(curr_rhs.begin(), curr_rhs.end(), to_delete), curr_rhs.end());
                    if (curr_rhs.empty() && !eps_generative_nts_mask[lhs - 'A']) {
                        scheduled_for_deletion.push(lhs);
                        eps_generative_nts_mask[lhs - 'A'] = true;
                    }
                }
            }

            return eps_generative_nts_mask;
        }

        void BuildChainClosure(CFNonTerminal nt, const CFGrammar& grammar, GrammarChainClosures& closures) {
            if (closures.find(nt) != closures.end()) {
                return;
            }

            // This method assumes that 1 <= len(RHS) <= 2

            auto& closure = closures[nt];

            auto nt_rules = grammar[nt];
            for (auto rule_iter = nt_rules.first; rule_iter != nt_rules.second; rule_iter++) {
                const CFGrammarRule& rule = rule_iter->second;
                assert(!rule.rhs.empty() && rule.rhs.size() <= 2);

                if (rule.rhs.size() == 1 && !IsTerminal(rule.rhs[0])) {
                    BuildChainClosure(rule.rhs[0], grammar, closures);
                    closure.insert(closures[rule.rhs[0]].begin(), closures[rule.rhs[0]].end());
                } else {
                    assert(rule.rhs.size() == 2 || (rule.rhs.size() == 1 && IsTerminal(rule.rhs[0])));
                    closure.insert(&rule.rhs);
                }
            }
        }

        void TraverseGrammar(CFNonTerminal nt, const CFGrammar& grammar, std::array<bool, ALPHABET_SIZE>& visited) {
            assert(!IsTerminal(nt));

            if (visited[nt - 'A']) {
                return;
            }

            visited[nt - 'A'] = true;

            auto rules = grammar[nt];
            for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                const CFGrammarRule& rule = rule_iter->second;
                for (CFRuleSymbol s : rule.rhs) {
                    if (!IsTerminal(s)) {
                        TraverseGrammar(s, grammar, visited);
                    }
                }
            }
        }
    }
}