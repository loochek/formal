#pragma once

#include <string>
#include <libformal/automaton.hpp>

namespace formal {
    /**
     * Creates a cute graphical representation for given automation using Graphvis
     * @param automaton Automaton to dump
     * @param out_file_name Out file name
     */
    void DumpAutomaton(const Automaton& automaton, const std::string& out_file_name);

    /**
     * Replaces eps-transitions with non-eps shortcuts in given NFA
     * @param automaton Automaton to process
     */
    void RemoveEpsTransitions(Automaton& automaton);

    /**
     * Removes unreachable states and epsilon-loops
     * @param automaton Automaton to process. Must have defined initial state
     */
    void Optimize(Automaton& automaton);

    /**
     * Transforms NFA to DFA
     * @param automaton Automaton to process. Must have defined initial state.
     * All transitions must be single-letter
     */
    void TransformToDFA(Automaton& automaton);
}