#include <libformal/algorithms.hpp>
#include <libformal/automaton_state.hpp>

void hw4_task1() {
    formal::Automaton aut;

    auto s00 = aut.InsertState();
    auto s01 = aut.InsertState();
    auto s02 = aut.InsertState();
    auto s10 = aut.InsertState();
    auto s11 = aut.InsertState();
    auto s12 = aut.InsertState();
    auto s20 = aut.InsertState();
    auto s21 = aut.InsertState();
    auto s22 = aut.InsertState();

    s00->MarkAsInitial();
    s00->MarkAsFinal();
    s11->MarkAsFinal();
    s22->MarkAsFinal();

    s00->AddTransition("a", s10);
    s00->AddTransition("b", s01);
    s01->AddTransition("a", s11);
    s01->AddTransition("b", s02);
    s02->AddTransition("a", s12);
    s02->AddTransition("b", s00);
    s10->AddTransition("a", s20);
    s10->AddTransition("b", s11);
    s11->AddTransition("a", s21);
    s11->AddTransition("b", s12);
    s12->AddTransition("a", s22);
    s12->AddTransition("b", s10);
    s20->AddTransition("a", s00);
    s20->AddTransition("b", s21);
    s21->AddTransition("a", s01);
    s21->AddTransition("b", s22);
    s22->AddTransition("a", s02);
    s22->AddTransition("b", s20);

    assert(aut.IsDFA());

    formal::DumpAutomaton(aut, "hw4_t1_cdfa");
    formal::MinimizeCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t1_mcdfa");
}

void hw4_task2() {
    // hw4 task{2,5} automaton
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
    formal::DumpAutomaton(aut, "hw4_t2_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "hw4_t2_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t2_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t2_cdfa");
    formal::MinimizeCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t2_mcdfa");
}

void hw4_task3() {
    // hw4 task{3,6} automaton
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
    formal::DumpAutomaton(aut, "hw4_t3_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "hw4_t3_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t3_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t3_cdfa");
    formal::MinimizeCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t3_mcdfa");
}

void hw4_task4() {
    // hw4 task4 automaton
    formal::Automaton aut;

    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();

    s1->MarkAsInitial();
    s1->MarkAsFinal();
    s2->MarkAsFinal();

    s1->AddTransition("a", s2);
    s1->AddTransition("b", s1);
    s2->AddTransition("a", s3);
    s2->AddTransition("b", s1);
    s3->AddTransition("a", s2);
    s3->AddTransition("b", s4);
    s4->AddTransition("a", s3);
    s4->AddTransition("b", s4);

    formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "hw4_t4_re");
}

void hw4_task5() {
    // hw4 task{2,5} automaton
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
    formal::DumpAutomaton(aut, "hw4_t5_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "hw4_t5_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t5_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t5_cdfa");
    formal::ComplementCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t5_ccdfa");
    formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "hw4_t5_re");
}

void hw4_task6() {
    // hw4 task{3,6} automaton
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
    formal::DumpAutomaton(aut, "hw4_t6_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "hw4_t6_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_cdfa");
    formal::ComplementCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_ccdfa");
    formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "hw4_t6_re");
}

int main() {
    hw4_task1();
    hw4_task2();
    hw4_task3();
    hw4_task4();
    hw4_task5();
    hw4_task6();

    return 0;
}