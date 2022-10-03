#include <libformal/algorithms.hpp>
#include <libformal/automaton_state.hpp>

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
    formal::DumpAutomaton(aut, "t4_re");
}

void hw4_task5() {
    // hw4 task6 automaton
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
    formal::DumpAutomaton(aut, "hw4_t6_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "hw4_t6_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_cdfa");
    formal::ComplementCDFA(aut);
    formal::DumpAutomaton(aut, "hw4_t6_ccdfa");
    std::string regexp = formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "hw4_t6_re");
}

void test1() {
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

    formal::DumpAutomaton(aut, "kek_nfa");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "kek_nfa_noeps");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "kek_dfa");
    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "kek_cdfa");
    formal::ComplementCDFA(aut);
    formal::DumpAutomaton(aut, "kek_ccdfa");
    formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "kek_re");
}

int main() {
//    // hw3 task1 automaton
//    formal::Automaton aut;
//    formal::AutomatonState* init_state = aut.InsertState();
//    init_state->MarkAsInitial();
//
//    formal::AutomatonState* state1 = aut.InsertState();
//    formal::AutomatonState* state2 = aut.InsertState();
//    formal::AutomatonState* state3 = aut.InsertState();
//    formal::AutomatonState* state4 = aut.InsertState();
//    formal::AutomatonState* state5 = aut.InsertState();
//    formal::AutomatonState* state6 = aut.InsertState();
//    formal::AutomatonState* state7 = aut.InsertState();
//
//    init_state->AddTransition("a", state1);
//    init_state->AddTransition("b", state2);
//
//    state1->AddTransition("b", state3);
//    state2->AddTransition("a", state3);
//    state3->AddTransition("", init_state);
//
//    init_state->AddTransition("", state4);
//    state4->MarkAsFinal();
//
//    init_state->AddTransition("a", state5);
//    state5->MarkAsFinal();
//
//    init_state->AddTransition("b", state6);
//    state6->AddTransition("a", state7);
//    state7->MarkAsFinal();
//
//    formal::DumpAutomaton(aut, "pre_eps_rm");
//    formal::RemoveEpsTransitions(aut);
//    formal::DumpAutomaton(aut, "post_eps_rm");
//    formal::TransformToDFA(aut);
//    formal::DumpAutomaton(aut, "dfa");
//    formal::CompleteDFA(aut);
//    formal::DumpAutomaton(aut, "cdfa");
//    formal::ComplementCDFA(aut);
//    formal::DumpAutomaton(aut, "ccdfa");

//    hw4_task5();
//    hw4_task6();
    hw4_task4();

    return 0;
}