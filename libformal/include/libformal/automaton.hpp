#pragma once

#include <cassert>
#include <map>
#include <string>
#include <unordered_set>
#include <set>
#include <memory>

namespace formal {
    class AutomatonState;

    /**
     * Nondeterministic finite automaton
     */
    class Automaton {
        friend class AutomatonState;

    public:
        explicit Automaton(std::unordered_set<char> alphabet = { 'a', 'b' });
        ~Automaton();

        // I'm lazy ¯\_(ツ)_/¯
        Automaton(const Automaton& other) = delete;
        Automaton& operator=(const Automaton& other) = delete;

        Automaton(Automaton&& other) noexcept;
        Automaton& operator=(Automaton&& other) noexcept;

        AutomatonState* InsertState();

        const auto& GetAlphabet() const {
            return alphabet_;
        }

        const auto& GetStates() const {
            return states_;
        }

        AutomatonState* GetInitialState() const {
            return initial_state_;
        }

        void ClearInitialState() {
            initial_state_ = nullptr;
        }

        const auto& GetFinalStates() const {
            return final_states_;
        }

        bool Belongs(AutomatonState* state) const {
            return states_.find(state) != states_.end();
        }

        bool RespectsAlphabet() const {
            if (!respect_alphabet_) {
                ActualizeProperties();
            }

            return respect_alphabet_;
        }

        bool HasNoEpsTransitions() const {
            if (!no_eps_) {
                ActualizeProperties();
            }

            return no_eps_;
        }

        bool IsSingleLetter() const {
            if (!single_letter_) {
                ActualizeProperties();
            }

            return single_letter_;
        }

        bool IsDFA() const {
            if (!dfa_) {
                ActualizeProperties();
            }

            return dfa_;
        }

    private:
        int GetNextNodeId() {
            return node_counter_++;
        }

        void OnNewTransition(AutomatonState* src, AutomatonState* dst, const std::string& word);
        void ActualizeProperties() const;

    private:
        // TODO: better architectural approach for this properties?
        std::unordered_set<char> alphabet_;
        mutable bool respect_alphabet_;
        mutable bool no_eps_;
        mutable bool single_letter_;
        mutable bool dfa_;

        std::unordered_set<AutomatonState*> states_;
        std::unordered_set<AutomatonState*> final_states_;
        AutomatonState* initial_state_;

        int node_counter_;
    };
}