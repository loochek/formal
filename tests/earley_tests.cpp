#include <libformal/grammar.hpp>
#include <libformal/earley.hpp>
#include <gtest/gtest.h>

TEST(GeneralTest, EarleyTestCPS) {
    // Correct parentheses sequences
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => XaXb"));
    grammar.AddRule(formal::ParseRuleFromString("X => ."));
    grammar.AddRule(formal::ParseRuleFromString("S => X"));

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

TEST(GeneralTest, EarleyTestPalindroms) {
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

TEST(GeneralTest, EarleyNonSingleSRule) {
    std::string gr_str = "X => amogus";
    formal::CFGrammar grammar = formal::ParseGrammarFromString(gr_str);
    EXPECT_ANY_THROW(formal::EarleyParser parser(grammar));

    gr_str = "S => amogus\nS => kek\n";
    grammar = formal::ParseGrammarFromString(gr_str);
    EXPECT_ANY_THROW(formal::EarleyParser parser2(grammar));
}

TEST(GeneralTest, EarleyBadSRule) {
    std::string gr_str = "S => AB";
    formal::CFGrammar grammar = formal::ParseGrammarFromString(gr_str);
    EXPECT_ANY_THROW(formal::EarleyParser parser(grammar));

    gr_str = "S => .";
    grammar = formal::ParseGrammarFromString(gr_str);
    EXPECT_ANY_THROW(formal::EarleyParser parser2(grammar));
}

TEST(GeneralTest, NoEpsGrammar) {
    // Grammar: specified words only
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => Y"));
    grammar.AddRule(formal::ParseRuleFromString("Y => Z"));
    grammar.AddRule(formal::ParseRuleFromString("Z => aboba"));
    grammar.AddRule(formal::ParseRuleFromString("Z => amogus"));

    formal::EarleyParser parser(grammar);

    EXPECT_TRUE(parser.parse("aboba"));
    EXPECT_TRUE(parser.parse("amogus"));

    EXPECT_FALSE(parser.parse(""));
    EXPECT_FALSE(parser.parse("kek"));
}