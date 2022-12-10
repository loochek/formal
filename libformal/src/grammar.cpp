#include <libformal/utils.hpp>
#include <libformal/grammar.hpp>
#include <fmt/core.h>

namespace formal {
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
}