#include <fmt/core.h>
#include <libformal/algorithms.hpp>
#include <libformal/automaton_state.hpp>

void test1_task3()
{
    formal::Automaton aut;
    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();
    auto s5 = aut.InsertState();
    auto s6 = aut.InsertState();
    auto s7 = aut.InsertState();
    auto s8 = aut.InsertState();

    s1->MarkAsInitial();
    s4->MarkAsFinal();
    s8->MarkAsFinal();

    s1->AddTransition("b", s2);
    s2->AddTransition("b", s3);
    s3->AddTransition("", s4);
    s4->AddTransition("b", s4);
    s3->AddTransition("", s5);
    s5->AddTransition("a", s6);
    s6->AddTransition("a", s7);
    s7->AddTransition("a", s8);

    formal::DumpAutomaton(aut, "test1_task3_nfa");

    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "test1_task3_noeps");

    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "test1_task3_dfa");

    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "test1_task3_cdfa");

    formal::MinimizeCDFA(aut);
    formal::DumpAutomaton(aut, "test1_task3_mcdfa");

}

void test1_task4()
{
    formal::Automaton aut;
    auto s1 = aut.InsertState();
    auto s2 = aut.InsertState();
    auto s3 = aut.InsertState();
    auto s4 = aut.InsertState();
    auto s5 = aut.InsertState();
    auto s6 = aut.InsertState();
    auto s7 = aut.InsertState();
    auto s8 = aut.InsertState();

    s1->MarkAsInitial();
    s4->MarkAsFinal();
    s8->MarkAsFinal();

    s1->AddTransition("b", s2);
    s2->AddTransition("b", s3);
    s3->AddTransition("", s4);
    s4->AddTransition("b", s4);
    s3->AddTransition("", s5);
    s5->AddTransition("a", s6);
    s6->AddTransition("a", s7);
    s7->AddTransition("a", s8);

    formal::DumpAutomaton(aut, "test1_task4_nfa");

    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "test1_task4_noeps");

    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "test1_task4_dfa");

    formal::CompleteDFA(aut);
    formal::DumpAutomaton(aut, "test1_task4_cdfa");

    formal::ComplementCDFA(aut);
    formal::DumpAutomaton(aut, "test1_task4_ccdfa");

    formal::MinimizeCDFA(aut);
    formal::DumpAutomaton(aut, "test1_task4_mccdfa");

    formal::NFAToRegExp(aut);
    formal::DumpAutomaton(aut, "test1_task4_regexp");

}

int main() {
    test1_task3();
    test1_task4();
    return 0;
}