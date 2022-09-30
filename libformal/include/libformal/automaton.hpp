#pragma once

#include <cassert>
#include <map>
#include <string>
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
        Automaton();
        ~Automaton();

        // I'm lazy ¯\_(ツ)_/¯
        Automaton(const Automaton& other) = delete;
        Automaton& operator=(const Automaton& other) = delete;

        Automaton(Automaton&& other) noexcept;
        Automaton& operator=(Automaton&& other) noexcept;

        AutomatonState* InsertState();

        const auto GetStates() const {
            return states_;
        }

        AutomatonState* GetInitialState() const {
            return initial_state_;
        }

        void ClearInitialState() {
            initial_state_ = nullptr;
        }

        const auto GetFinalStates() const {
            return final_states_;
        }

        bool Belongs(AutomatonState* state) const {
            return states_.find(state) != states_.end();
        }

    private:
        int GetNextNodeId() {
            return node_counter_++;
        }

    private:
        std::set<AutomatonState*> states_;
        std::set<AutomatonState*> final_states_;
        AutomatonState* initial_state_;

        int node_counter_;
    };
}