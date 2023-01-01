#include <libformal/grammar.hpp>
#include <libformal/earley.hpp>
#include <gtest/gtest.h>

// It's weird, but I don't think that I should export intermediate steps as public API
#include "../libformal/src/grammar.cpp"

bool PresentInGrammar(const formal::CFGrammar& grammar, const formal::CFGrammarRule& rule) {
    auto rules = grammar[rule.lhs];
    for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
        if (rule_iter->second == rule) {
            return true;
        }
    }

    return false;
}

TEST(GeneralTest, CNFTest) {
    // Correct brace sequences
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => XaXb"));
    grammar.AddRule(formal::ParseRuleFromString("X => ."));

    ASSERT_FALSE(grammar.IsCNF());
    formal::TransformToCNF(grammar);
    ASSERT_TRUE(grammar.IsCNF());

    ASSERT_EQ(grammar.GetRules().size(), 10);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('S', "AD")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('S', "XC")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('S', "")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "AD")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "XC")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('C', "AD")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('D', "b")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('D', "XB")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('B', "b")));
}

TEST(GeneralTest, CNFStagesUnitTestingDemixify) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => NaNb"));
    ASSERT_FALSE(grammar.IsCNF());

    formal::DemixifyRules(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 3);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "NANB")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('B', "b")));
}

TEST(GeneralTest, CNFStagesUnitTestingShortify) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => NaNb"));
    ASSERT_FALSE(grammar.IsCNF());

    formal::ShortifyRules(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 3);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "NA")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "aB")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('B', "Nb")));
}

TEST(GeneralTest, CNFStagesRemoveEpsNonTerms1) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => AB"));
    grammar.AddRule(formal::ParseRuleFromString("A => ."));
    ASSERT_FALSE(grammar.IsCNF());

    formal::RemoveEpsNonTerminals(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 2);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "B")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "AB")));
}

TEST(GeneralTest, CNFStagesRemoveEpsNonTerms2) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("X => AB"));
    grammar.AddRule(formal::ParseRuleFromString("B => ."));
    ASSERT_FALSE(grammar.IsCNF());

    formal::RemoveEpsNonTerminals(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 2);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "A")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "AB")));
}

TEST(GeneralTest, CNFStagesRemoveEpsNonTerms3) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("L => DX"));
    grammar.AddRule(formal::ParseRuleFromString("X => AB"));
    grammar.AddRule(formal::ParseRuleFromString("A => ."));
    grammar.AddRule(formal::ParseRuleFromString("B => ."));
    ASSERT_FALSE(grammar.IsCNF());

    formal::RemoveEpsNonTerminals(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 3);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('L', "D")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('L', "DX")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "AB")));
}

TEST(GeneralTest, CNFStagesRemoveChainRules) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("A => B"));
    grammar.AddRule(formal::ParseRuleFromString("B => C"));
    grammar.AddRule(formal::ParseRuleFromString("C => a"));
    grammar.AddRule(formal::ParseRuleFromString("C => b"));
    ASSERT_FALSE(grammar.IsCNF());

    formal::RemoveChainRules(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 6);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "b")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('B', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('B', "b")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('C', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('C', "b")));

    // Additional check
    grammar.AddRule(formal::CFGrammarRule('S', "A"));
    formal::RemoveUnreachableAndNonProducers(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 3);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('S', "A")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "a")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('A', "b")));
}

TEST(GeneralTest, CNFStagesRemoveUnreachableAndNonProducers) {
    formal::CFGrammar grammar;
    grammar.AddRule(formal::ParseRuleFromString("S => A"));

    grammar.AddRule(formal::ParseRuleFromString("A => B"));
    grammar.AddRule(formal::ParseRuleFromString("B => A"));

    grammar.AddRule(formal::ParseRuleFromString("S => X"));
    grammar.AddRule(formal::ParseRuleFromString("X => a"));

    grammar.AddRule(formal::ParseRuleFromString("Y => Z"));

    formal::RemoveUnreachableAndNonProducers(grammar);

    ASSERT_EQ(grammar.GetRules().size(), 2);
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('S', "X")));
    ASSERT_TRUE(PresentInGrammar(grammar, formal::CFGrammarRule('X', "a")));
}