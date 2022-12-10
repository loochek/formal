#include <libformal/grammar.hpp>
#include <gtest/gtest.h>

TEST(GeneralTest, RuleParseTest) {
    EXPECT_EQ(formal::ParseRuleFromString("S => X"), formal::CFGrammarRule('S', "X"));
    EXPECT_EQ(formal::ParseRuleFromString("A=>BoBa"), formal::CFGrammarRule('A', "BoBa"));
    EXPECT_EQ(formal::ParseRuleFromString("A =>mOgUs"), formal::CFGrammarRule('A', "mOgUs"));
    EXPECT_EQ(formal::ParseRuleFromString("S=> X"), formal::CFGrammarRule('S', "X"));
    EXPECT_EQ(formal::ParseRuleFromString("S => X"), formal::CFGrammarRule('S', "X"));
}

TEST(GeneralTest, MultilineParseTest) {
    formal::CFGrammar grammar = formal::ParseGrammarFromString("S => X\n X=> XaXb\n H=>.\n");
    EXPECT_TRUE(grammar.GetRules().count('S') == 1);
    EXPECT_TRUE(grammar.GetRules().count('X') == 1);
    EXPECT_TRUE(grammar.GetRules().count('H') == 1);

    EXPECT_EQ(grammar['S'].first->second, formal::CFGrammarRule('S', "X"));
    EXPECT_EQ(grammar['X'].first->second, formal::CFGrammarRule('X', "XaXb"));
    EXPECT_EQ(grammar['H'].first->second, formal::CFGrammarRule('H', ""));
}

TEST(GeneralTest, BadSyntaxTest) {
    EXPECT_ANY_THROW(formal::ParseRuleFromString("Ss => AB"));
    EXPECT_ANY_THROW(formal::ParseRuleFromString("S X"));
    EXPECT_ANY_THROW(formal::ParseRuleFromString("S =>"));
    EXPECT_ANY_THROW(formal::ParseRuleFromString("=>"));
    EXPECT_ANY_THROW(formal::ParseRuleFromString("=> X"));
    EXPECT_ANY_THROW(formal::ParseRuleFromString("S => KeK.LoL"));
}