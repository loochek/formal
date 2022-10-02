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
     * Transforms NFA to IsDFA
     * @param automaton Automaton to process. Must have defined initial state.
     * All transitions must be single-letter
     */
    void TransformToDFA(Automaton& automaton);

    /**
     * Transforms IsDFA to CDFA
     * @param automaton Automaton to process. Must be IsDFA.
     */
    void CompleteDFA(Automaton& automaton);

    /**
     * Complements given CDFA (L(A) -> \sigma^* - L(A))
     * @param automaton Automaton to process. Must be CDFA.
     */
    void ComplementCDFA(Automaton& automaton);

    /**
     * Makes |F|=1 (breaking all properties :) )
     * @param automaton Automaton to process
     */
    void SinglifyFinalState(Automaton& automaton);

    /**
     * Transforms given automaton to the regular automaton with |Q| <= 2
     * @param automaton Automaton to process.
     */
     void NFAToRegExp(Automaton& automaton);

    /**
     * Tries to read given word in given DFA
     * @param automaton Automaton
     * @return True if word is accepted by automaton, false otherwise
     */
    bool DFAReadWord(Automaton& automaton, const std::string& word);
}