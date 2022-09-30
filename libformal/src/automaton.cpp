#include <libformal/automaton.hpp>
#include <libformal/automaton_state.hpp>

namespace formal {
    Automaton::Automaton() : initial_state_(nullptr), node_counter_(0) {}

    Automaton::Automaton(Automaton &&other) noexcept :
      initial_state_(other.initial_state_), node_counter_(other.node_counter_),
      states_(std::move(other.states_)), final_states_(std::move(other.final_states_)) {
        other.initial_state_ = nullptr;
        other.node_counter_ = 0;
    }

    Automaton& Automaton::operator=(Automaton &&other) noexcept {
        for (AutomatonState* state : states_) {
            delete state;
        }

        initial_state_ = other.initial_state_;
        node_counter_ = other.node_counter_;
        states_ = std::move(other.states_);
        final_states_ = std::move(other.final_states_);

        other.initial_state_ = nullptr;
        other.node_counter_ = 0;

        return *this;
    }

    Automaton::~Automaton() {
        for (AutomatonState* state : states_) {
            delete state;
        }
    }

    AutomatonState *Automaton::InsertState()  {
        AutomatonState* new_state = new AutomatonState(this);
        states_.insert(new_state);
        return new_state;
    }
}