#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <libformal/utils.hpp>

namespace formal {
    using CFNonTerminal = char;
    using CFRuleSymbol = char;

    inline const std::string PROD_LEX = "=>";
    inline const std::string EPS_RHS_LEX = ".";
    inline const std::string RULE_SEP_LEX = "\n";

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

        CFNonTerminal lhs;
        std::string rhs;
    };

    class CFGrammar {
    public:
        void AddRule(CFGrammarRule rule) {
            rules_.insert(std::pair(rule.lhs, std::move(rule)));
        }

        auto operator[](CFNonTerminal lhs) const {
            return rules_.equal_range(lhs);
        }

        auto& GetRules() const {
            return rules_;
        }

    private:
        std::unordered_multimap<CFNonTerminal, CFGrammarRule> rules_;
    };

    CFGrammarRule ParseRuleFromString(std::string_view rule);
    CFGrammar ParseGrammarFromString(const std::string &str);

    inline bool IsTerminal(CFRuleSymbol symbol) {
        return std::islower(symbol);
    }
}

MAKE_HASHABLE(formal::CFGrammarRule, t.lhs, t.rhs);