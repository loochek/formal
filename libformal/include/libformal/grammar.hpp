#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <libformal/utils.hpp>
#include <array>
#include <fmt/core.h>

namespace formal {
    using CFNonTerminal = char;
    using CFRuleSymbol = char;

    inline const std::string PROD_LEX = "=>";
    inline const std::string EPS_RHS_LEX = ".";
    inline const std::string RULE_SEP_LEX = "\n";

    inline CFNonTerminal START_SYMBOL = 'S';

    inline const int ALPHABET_SIZE = 26;

    inline bool IsTerminal(CFRuleSymbol symbol);

    class GrammarProcessError : public std::runtime_error
    {
    public:
        explicit GrammarProcessError(const std::string& what = "") : std::runtime_error(what) {}
    };

    struct CFGrammarRule {
        explicit CFGrammarRule(CFNonTerminal lhs, std::string rhs);

        CFGrammarRule(const CFGrammarRule& other) = default;
        CFGrammarRule(CFGrammarRule&& other) = default;

        CFGrammarRule& operator=(const CFGrammarRule& other) = default;
        CFGrammarRule& operator=(CFGrammarRule&& other) = default;

        bool operator==(const CFGrammarRule& other) const {
            return lhs == other.lhs && rhs == other.rhs;
        }

        bool operator<(const CFGrammarRule& other) const {
            if (lhs == other.lhs) {
                return rhs < other.rhs;
            }

            return lhs < other.lhs;
        }

        explicit operator std::string() const {
            return fmt::format("{} => {}", lhs, rhs.empty() ? "." : rhs);
        }

        CFNonTerminal lhs;
        std::string rhs;
    };

    class CFGrammar {
    public:
        CFGrammar() : used_{} {}

        CFGrammar(const CFGrammar& other) = default;
        CFGrammar& operator=(const CFGrammar& other) = default;

        CFGrammar(CFGrammar& other) : rules_(std::move(other.rules_)) {
            for (int term = 0; term < ALPHABET_SIZE; term++) {
                used_[term] = other.used_[term];
                other.used_[term] = false;
            }
        }

        CFGrammar& operator=(CFGrammar&& other) noexcept {
            rules_ = std::move(other.rules_);

            for (CFRuleSymbol i = 0; i < ALPHABET_SIZE; i++) {
                used_[i] = other.used_[i];
                other.used_[i] = false;
            }

            return *this;
        }

        auto operator[](CFNonTerminal lhs) const {
            return rules_.equal_range(lhs);
        }

        void AddRule(CFGrammarRule rule) {
            // Check if already exists
            auto rules = this->operator[](rule.lhs);
            for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                const CFGrammarRule& old_rule = rule_iter->second;
                if (old_rule == rule) {
                    return;
                }
            }

            used_[rule.lhs - 'A'] = true;
            for (CFRuleSymbol s : rule.rhs) {
                if (!IsTerminal(s)) {
                    used_[s - 'A'] = true;
                }
            }

            rules_.insert(std::pair(rule.lhs, std::move(rule)));
        }

        auto& GetRules() const {
            return rules_;
        }

        bool IsCNF() const {
            for (auto& [lhs, rule] : rules_) {
                switch (rule.rhs.size()) {
                    case 0:
                        if (lhs != START_SYMBOL) {
                            return false;
                        }

                        break;

                    case 1:
                        if (!IsTerminal(rule.rhs[0])) {
                            return false;
                        }

                        break;

                    case 2:
                        if (IsTerminal(rule.rhs[0]) || IsTerminal(rule.rhs[1])) {
                            return false;
                        }

                        break;

                    default:
                        return false;
                }
            }

            return true;
        }

        bool IsNonTermUsed(CFNonTerminal nt) const {
            return used_[nt - 'A'];
        }

        CFNonTerminal GetUnusedNonTerm(CFNonTerminal wanted = '\0') {
            if (wanted != '\0' && !used_[wanted - 'A']) {
                used_[wanted - 'A'] = true;
                return wanted;
            }

            for (CFNonTerminal nt = 'A'; nt <= 'Z'; nt++) {
                if (!used_[nt - 'A']) {
                    used_[nt - 'A'] = true;
                    return nt;
                }
            }

            throw GrammarProcessError("No non-terminal symbols left to satisfy GetUnusedNonTerm call");
        }

        explicit operator std::string() const {
            std::string result;
            for (auto& [nt, rule] : rules_) {
                result += std::string(rule);
                result += '\n';
            }

            return result;
        }

    private:
        std::unordered_multimap<CFNonTerminal, CFGrammarRule> rules_;
        // A..Z
        std::array<bool, ALPHABET_SIZE> used_;
    };

    CFGrammarRule ParseRuleFromString(std::string_view rule);
    CFGrammar ParseGrammarFromString(const std::string &str);

    /// Transforms given grammar into the form where exactly one rule starts from S
    void SinglifyEntryRule(CFGrammar& grammar);

    /// Transforms given grammar into Chomsky normal form
    void TransformToCNF(CFGrammar& grammar);

    inline bool IsTerminal(CFRuleSymbol symbol) {
        return std::islower(symbol);
    }
}

MAKE_HASHABLE(formal::CFGrammarRule, t.lhs, t.rhs);