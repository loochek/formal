#include <libformal/algorithms.hpp>
#include <libformal/automaton_state.hpp>

int main() {
    // hw3 task1 automaton
    formal::Automaton aut;
    formal::AutomatonState* init_state = aut.InsertState();
    init_state->MarkAsInitial();

    formal::AutomatonState* state1 = aut.InsertState();
    formal::AutomatonState* state2 = aut.InsertState();
    formal::AutomatonState* state3 = aut.InsertState();
    formal::AutomatonState* state4 = aut.InsertState();
    formal::AutomatonState* state5 = aut.InsertState();
    formal::AutomatonState* state6 = aut.InsertState();
    formal::AutomatonState* state7 = aut.InsertState();

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

    formal::DumpAutomaton(aut, "pre_eps_rm");
    formal::RemoveEpsTransitions(aut);
    formal::DumpAutomaton(aut, "post_eps_rm");
    formal::TransformToDFA(aut);
    formal::DumpAutomaton(aut, "dfa");

    return 0;
}