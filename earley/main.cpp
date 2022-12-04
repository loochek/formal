#include <fmt/core.h>
#include <iostream>
#include <libformal/grammar.hpp>
#include <libformal/earley.hpp>

int main() {
    fmt::print("Hello! I can parse words by the given context-free grammar!\n"
               "Enter your grammar rules separated by newlines: (or hit Enter to finish)\n");

    formal::CFGrammar grammar;
    std::string rule = "kek";
    while (!rule.empty()) {
        std::getline(std::cin, rule);
        if (rule.empty()) {
            break;
        }

        grammar.AddRule(formal::ParseRuleFromString(rule));
    }

    formal::EarleyParser parser(grammar);

    fmt::print("Great! Now tell me the words and I will try to produce it from the grammar!\n");
    std::string word = "kek";
    while (true) {
        fmt::print("Enter word (or hit Enter to finish): ");
        std::getline(std::cin, word);
        if (word.empty()) {
            break;
        }

        bool result = parser.parse(word);
        if (result) {
            fmt::print("Yes! Word \"{}\" is producible from the grammar!\n", word);
        } else {
            fmt::print("Nah! Word \"{}\" is NOT producible from the grammar!\n", word);
        }
    }

    fmt::print("Goodbye! :3\n");
    return 0;
}