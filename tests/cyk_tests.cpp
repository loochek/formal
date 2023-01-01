#include <libformal/grammar.hpp>
#include <libformal/cyk.hpp>
#include <gtest/gtest.h>

// Same tests are used for Earley parser

TEST(GeneralTest, CYKTestCPS) {
    // Correct parentheses sequences
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => XaXb"));
    grammar.AddRule(formal::ParseRuleFromString("X => ."));
    grammar.AddRule(formal::ParseRuleFromString("S => X"));

    formal::TransformToCNF(grammar);
    formal::CYKParser parser(grammar);
    fmt::print("{}", std::string(grammar));

    EXPECT_TRUE(parser.parse(""));
    EXPECT_TRUE(parser.parse("aabb"));
    EXPECT_TRUE(parser.parse("abab"));
    EXPECT_TRUE(parser.parse("aababaabbb"));
    EXPECT_TRUE(parser.parse("aabbabababaaaaaababbaaaabbbabbbbbaabbbab"));
    EXPECT_TRUE(parser.parse("aaaaaaabbbbabaaabbbbabaabaababaaaaabababbbabbabbaaabaaaaaababbbbbabaababbbbbbbbb"));

    EXPECT_FALSE(parser.parse("bbaaabab"));
    EXPECT_FALSE(parser.parse("aabbabababaabbaaaaabbabaaaabbbabbbbbaabbbab"));
    EXPECT_FALSE(parser.parse("aaaaaaabbbbabaaababbabaabaababaaaaababaababbbabbabbaaabbaaaaababbbbbabaababbbbbbbbb"));
    EXPECT_FALSE(parser.parse("amogus"));
}

TEST(GeneralTest, CYKTestCPSInitialCNF) {
    // Correct parentheses sequences grammar in CNF
    formal::CFGrammar grammar;
    grammar.AddRule(formal::CFGrammarRule('S', "AD"));
    grammar.AddRule(formal::CFGrammarRule('S', "XC"));
    grammar.AddRule(formal::CFGrammarRule('S', ""));
    grammar.AddRule(formal::CFGrammarRule('X', "AD"));
    grammar.AddRule(formal::CFGrammarRule('X', "XC"));
    grammar.AddRule(formal::CFGrammarRule('C', "AD"));
    grammar.AddRule(formal::CFGrammarRule('D', "b"));
    grammar.AddRule(formal::CFGrammarRule('D', "XB"));
    grammar.AddRule(formal::CFGrammarRule('A', "a"));
    grammar.AddRule(formal::CFGrammarRule('B', "b"));
    
    formal::CYKParser parser(grammar);

    EXPECT_TRUE(parser.parse(""));
    EXPECT_TRUE(parser.parse("aabb"));
    EXPECT_TRUE(parser.parse("abab"));
    EXPECT_TRUE(parser.parse("aababaabbb"));
    EXPECT_TRUE(parser.parse("aabbabababaaaaaababbaaaabbbabbbbbaabbbab"));
    EXPECT_TRUE(parser.parse("aaaaaaabbbbabaaabbbbabaabaababaaaaabababbbabbabbaaabaaaaaababbbbbabaababbbbbbbbb"));

    EXPECT_FALSE(parser.parse("bbaaabab"));
    EXPECT_FALSE(parser.parse("aabbabababaabbaaaaabbabaaaabbbabbbbbaabbbab"));
    EXPECT_FALSE(parser.parse("aaaaaaabbbbabaaababbabaabaababaaaaababaababbbabbabbaaabbaaaaababbbbbabaababbbbbbbbb"));
    EXPECT_FALSE(parser.parse("amogus"));
}

TEST(GeneralTest, CYKTestPalindroms) {
    // Palindroms constructed with {a, b, c, o}
    std::string gr_str = "S => X\n"
                         "X => aXa\n"
                         "X => bXb\n"
                         "X => cXc\n"
                         "X => cXc\n"
                         "X => oXo\n"
                         "X => a\n"
                         "X => b\n"
                         "X => c\n"
                         "X => o\n"
                         "X => .\n";

    formal::CFGrammar grammar = formal::ParseGrammarFromString(gr_str);

    formal::TransformToCNF(grammar);
    formal::CYKParser parser(grammar);

    EXPECT_TRUE(parser.parse("aba"));
    EXPECT_TRUE(parser.parse("abacaba"));
    EXPECT_TRUE(parser.parse("aboba"));
    EXPECT_TRUE(parser.parse("abacabaoooabacababbbabacabaoooabacabacccabacabaoooabacababbbabacabaoooabacaba"));
    EXPECT_TRUE(parser.parse("aaabaaa"));

    EXPECT_FALSE(parser.parse("ba"));
    EXPECT_FALSE(parser.parse("baa"));
    EXPECT_FALSE(parser.parse("amogus"));
    EXPECT_FALSE(parser.parse("abacabaobbabacabaoooabacabaobbabacaba"));
}

TEST(GeneralTest, CYKNoEpsGrammar) {
    // Grammar: specified words only
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => Y"));
    grammar.AddRule(formal::ParseRuleFromString("Y => Z"));
    grammar.AddRule(formal::ParseRuleFromString("Z => aboba"));
    grammar.AddRule(formal::ParseRuleFromString("Z => amogus"));

    formal::TransformToCNF(grammar);
    formal::CYKParser parser(grammar);

    EXPECT_TRUE(parser.parse("aboba"));
    EXPECT_TRUE(parser.parse("amogus"));

    EXPECT_FALSE(parser.parse(""));
    EXPECT_FALSE(parser.parse("kek"));
}

TEST(GeneralTest, CYKNoCNFTest) {
    // Grammar: specified words only
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => Y"));
    grammar.AddRule(formal::ParseRuleFromString("Y => Z"));
    grammar.AddRule(formal::ParseRuleFromString("Z => aboba"));
    grammar.AddRule(formal::ParseRuleFromString("Z => amogus"));

    EXPECT_ANY_THROW(formal::CYKParser parser(grammar));
}