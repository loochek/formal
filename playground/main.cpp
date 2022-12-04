#include <fmt/core.h>
#include <iostream>
#include <libformal/grammar.hpp>
#include <libformal/earley.hpp>

int main() {
//    formal::CFGrammar grammar;
//    grammar.AddRule(formal::ParseRuleFromString("X => XaXb"));
//    grammar.AddRule(formal::ParseRuleFromString("X => ."));
//    grammar.AddRule(formal::ParseRuleFromString("S => X"));

//    std::string grammar2 = "S => X\n"
//                           "X => aXa\n"
//                           "X => bXb\n"
//                           "X => cXc\n"
//                           "X => a\n"
//                           "X => b\n"
//                           "X => c\n"
//                           "X => .\n";
//
//    formal::CFGrammar grammar = formal::ParseGrammarFromString(grammar2);
//
//    formal::EarleyParser parser(grammar);
//    while (true) {
//        std::string word;
//        std::cin >> word;
//        std::cout << (parser.parse(word) ? "True\n" : "False\n");
//    }

    std::string gr_str = "X => amogus";
    formal::CFGrammar grammar = formal::ParseGrammarFromString(gr_str);

    return 0;
}