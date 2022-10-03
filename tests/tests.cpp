#include <libformal/algorithms.hpp>
#include <libformal/automaton_state.hpp>
#include <gtest/gtest.h>
#include <fmt/core.h>

TEST(GeneralTest, Hw3Task1Test) {
    // hw3 task1 automaton
    formal::Automaton aut;
    formal::AutomatonState *init_state = aut.InsertState();
    init_state->MarkAsInitial();

    formal::AutomatonState *state1 = aut.InsertState();
    formal::AutomatonState *state2 = aut.InsertState();
    formal::AutomatonState *state3 = aut.InsertState();
    formal::AutomatonState *state4 = aut.InsertState();
    formal::AutomatonState *state5 = aut.InsertState();
    formal::AutomatonState *state6 = aut.InsertState();
    formal::AutomatonState *state7 = aut.InsertState();

    init_state->AddTransition("a", state1);
    init_state->AddTransition("b", state2);

    state1->AddTransition("b", state3);
    state2->AddTransition("a", state3);
    state3->AddTransition("", init_state);

    init_state->AddTransition("", state4);
    state4->MarkAsFinal();

    init_state->AddTransition("a", state5);
    state5->MarkAsFinal();

    init_state->AddTransition("b", state6);
    state6->AddTransition("a", state7);
    state7->MarkAsFinal();

    formal::RemoveEpsTransitions(aut);
    formal::TransformToDFA(aut);

    EXPECT_TRUE(formal::DFAReadWord(aut, "abbaa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "baab"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "baa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "bb"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "ababb"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abababb"));

    formal::CompleteDFA(aut);
    formal::ComplementCDFA(aut);

    EXPECT_FALSE(formal::DFAReadWord(aut, "abbaa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "baab"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "baa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "bb"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "ababb"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abababb"));
}

TEST(GeneralTest, Hw4Task5Test) {
    // hw4 task5 automaton
    formal::Automaton aut;

    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();
    auto s5 = aut.InsertState();
    auto s6 = aut.InsertState();

    s1->AddTransition("a", s2);
    s2->AddTransition("a", s2);
    s2->AddTransition("", s3);
    s3->AddTransition("b", s4);
    s4->AddTransition("a", s3);
    s3->AddTransition("a", s5);
    s5->AddTransition("a", s6);
    s6->AddTransition("b", s5);
    s5->AddTransition("", s2);

    s1->MarkAsInitial();
    s2->MarkAsFinal();

    formal::DumpAutomaton(aut, "dump_test");

    formal::RemoveEpsTransitions(aut);
    formal::TransformToDFA(aut);

    EXPECT_TRUE(formal::DFAReadWord(aut, "abaaabababababaa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "aaaaaaaabaaba"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abababababa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "aabab"));
    EXPECT_FALSE(formal::DFAReadWord(aut, ""));

    formal::CompleteDFA(aut);
    formal::ComplementCDFA(aut);

    EXPECT_FALSE(formal::DFAReadWord(aut, "abaaabababababaa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "aaaaaaaabaaba"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abababababa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "aabab"));
    EXPECT_TRUE(formal::DFAReadWord(aut, ""));

    std::string regexp = formal::NFAToRegExp(aut);
    EXPECT_TRUE(aut.GetStates().size() == 2 && aut.GetInitialState()->GetTransitions().size() == 1);
    formal::AutomatonState* sec = aut.GetInitialState()->GetTransitions().begin()->second;
    EXPECT_EQ(aut.GetInitialState()->GetTransitions().begin()->first, "1+b(a+b)*+a((a(a(a+b(ab)*aa)*(b(ab)*b)+b)+b)(a(aa(a+b(ab)*aa)*(b(ab)*b)+ab+b))*(1+b(a+b)*+a))");
    EXPECT_TRUE(sec->IsFinal() && sec->GetTransitions().size() == 0);

    // Answer to hw4t5
    EXPECT_EQ(regexp, "(1+b(a+b)*+a((a(a(a+b(ab)*aa)*(b(ab)*b)+b)+b)(a(aa(a+b(ab)*aa)*(b(ab)*b)+ab+b))*(1+b(a+b)*+a)))");
}

TEST(GeneralTest, Hw4Task6Test) {
    // hw4 task6 automaton
    formal::Automaton aut;

    auto i = aut.InsertState();
    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();
    auto s5 = aut.InsertState();

    i->AddTransition("a", s1);
    s1->AddTransition("a", s2);
    s1->AddTransition("b", s3);
    s2->AddTransition("b", s1);
    s3->AddTransition("a", s1);
    s1->AddTransition("b", s4);
    s4->AddTransition("a", s4);
    s4->AddTransition("b", s5);
    s5->AddTransition("a", s4);
    s4->AddTransition("", i);

    i->MarkAsInitial();
    s4->MarkAsFinal();

    formal::RemoveEpsTransitions(aut);
    formal::TransformToDFA(aut);

    EXPECT_TRUE(formal::DFAReadWord(aut, "aabbabaabbbabaaaaaabaaabbabaabbbabaaaaaabaaabbabaabbbabaaaaaaba"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abababaaa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abababababababab"));

    EXPECT_FALSE(formal::DFAReadWord(aut, "aababababaabbabababababbababba"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "aabbabbbbb"));
    EXPECT_FALSE(formal::DFAReadWord(aut, ""));

    formal::CompleteDFA(aut);
    formal::ComplementCDFA(aut);

    EXPECT_FALSE(formal::DFAReadWord(aut, "aabbabaabbbabaaaaaabaaabbabaabbbabaaaaaabaaabbabaabbbabaaaaaaba"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abababaaa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abababababababab"));

    EXPECT_TRUE(formal::DFAReadWord(aut, "aababababaabbabababababbababba"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "aabbabbbbb"));
    EXPECT_TRUE(formal::DFAReadWord(aut, ""));

    std::string regexp = formal::NFAToRegExp(aut);

    EXPECT_TRUE(aut.GetStates().size() == 2 && aut.GetInitialState()->GetTransitions().size() == 1);
    formal::AutomatonState* sec = aut.GetInitialState()->GetTransitions().begin()->second;
    EXPECT_EQ(aut.GetInitialState()->GetTransitions().begin()->first, "1+b(a+b)*+a(ab)*(1+a(1+a(a+b)*)+b((a((a(a+ba)*bb+b)a)*((a(a+ba)*bb+b)b)+b)(aa((a(a+ba)*bb+b)a)*((a(a+ba)*bb+b)b)+ab)*(1+b(a+b)*)))");
    EXPECT_TRUE(sec->IsFinal() && sec->GetTransitions().size() == 0);

    // Answer to hw4t6
    EXPECT_EQ(regexp, "(1+b(a+b)*+a(ab)*(1+a(1+a(a+b)*)+b((a((a(a+ba)*bb+b)a)*((a(a+ba)*bb+b)b)+b)(aa((a(a+ba)*bb+b)a)*((a(a+ba)*bb+b)b)+ab)*(1+b(a+b)*))))");
}

TEST(GeneralTest, RegExpGenTest) {
    formal::Automaton aut;

    formal::AutomatonState* initial = aut.InsertState();
    formal::AutomatonState* final = aut.InsertState();

    initial->MarkAsInitial();
    final->MarkAsFinal();

    initial->AddTransition("a", final);
    final->AddTransition("b", initial);
    initial->AddTransition("a", initial);
    final->AddTransition("b", final);

    std::string regexp = formal::NFAToRegExp(aut);
    ASSERT_EQ(regexp, "(a)*a(b)*(b(a)*a(b)*)*");
}

TEST(GeneralTest, Test1) {
    formal::Automaton aut;

    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();
    auto s5 = aut.InsertState();
    auto s6 = aut.InsertState();
    auto s7 = aut.InsertState();

    s1->AddTransition("a", s2);
    s2->AddTransition("b", s3);
    s3->AddTransition("a", s4);
    s1->AddTransition("a", s5);
    s5->AddTransition("a", s6);
    s6->AddTransition("b", s7);

    s1->MarkAsInitial();
    s4->MarkAsFinal();
    s7->MarkAsFinal();

    formal::RemoveEpsTransitions(aut);
    formal::TransformToDFA(aut);

    EXPECT_TRUE(formal::DFAReadWord(aut, "aba"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "aab"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "ab"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abb"));
    EXPECT_FALSE(formal::DFAReadWord(aut, ""));
    EXPECT_FALSE(formal::DFAReadWord(aut, "abababa"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "bab"));

    formal::CompleteDFA(aut);
    formal::ComplementCDFA(aut);

    EXPECT_FALSE(formal::DFAReadWord(aut, "aba"));
    EXPECT_FALSE(formal::DFAReadWord(aut, "aab"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "ab"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abb"));
    EXPECT_TRUE(formal::DFAReadWord(aut, ""));
    EXPECT_TRUE(formal::DFAReadWord(aut, "abababa"));
    EXPECT_TRUE(formal::DFAReadWord(aut, "bab"));
}