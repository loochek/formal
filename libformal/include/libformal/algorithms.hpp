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
     * Minimizes CDFA
     * @param automaton Automaton to process
     */
    void MinimizeCDFA(Automaton& automaton);

    /**
     * Makes |F|=1 (breaking all properties :) )
     * @param automaton Automaton to process
     */
    void SinglifyFinalState(Automaton& automaton, bool force = false);

    /**
     * Generates a regular expression for the given automaton.
     * Note that automaton is completely trashed by this method
     * @param automaton Automaton to process.
     */
     std::string NFAToRegExp(Automaton& automaton);

    /**
     * Tries to read given word in given DFA
     * @param automaton Automaton
     * @return True if word is accepted by automaton, false otherwise
     */
    bool DFAReadWord(const Automaton& automaton, const std::string& word);

    /**
     * Checks given DFA to be CDFA
     * @param automaton Automaton
     * @return True if CDFA, false otherwise
     */
    bool IsCDFA(const Automaton& automaton);
}