#include <libformal/grammar.hpp>
#include <libformal/earley.hpp>
#include <gtest/gtest.h>

// There are copy of Earley tests, but with grammar transformed into CNF
// The main purpose is to test the CNF transformer, but it's also an additional check for Earley

TEST(GeneralTest, CNFEarleyTestCPS) {
    // Correct parentheses sequences
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => XaXb"));
    grammar.AddRule(formal::ParseRuleFromString("X => ."));
    grammar.AddRule(formal::ParseRuleFromString("S => X"));

    formal::TransformToCNF(grammar);
    EXPECT_TRUE(grammar.IsCNF());
    formal::SinglifyEntryRule(grammar);
    formal::EarleyParser parser(grammar);

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

TEST(GeneralTest, CNFEarleyTestPalindroms) {
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
    EXPECT_TRUE(grammar.IsCNF());
    formal::SinglifyEntryRule(grammar);
    formal::EarleyParser parser(grammar);

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

TEST(GeneralTest, CNFEarleyNoEpsGrammar) {
    // Grammar: specified words only
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => Y"));
    grammar.AddRule(formal::ParseRuleFromString("Y => Z"));
    grammar.AddRule(formal::ParseRuleFromString("Z => aboba"));
    grammar.AddRule(formal::ParseRuleFromString("Z => amogus"));

    formal::TransformToCNF(grammar);
    EXPECT_TRUE(grammar.IsCNF());
    formal::SinglifyEntryRule(grammar);
    formal::EarleyParser parser(grammar);

    EXPECT_TRUE(parser.parse("aboba"));
    EXPECT_TRUE(parser.parse("amogus"));

    EXPECT_FALSE(parser.parse(""));
    EXPECT_FALSE(parser.parse("kek"));
}