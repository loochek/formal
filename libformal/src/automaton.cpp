#include <libformal/automaton.hpp>
#include <libformal/automaton_state.hpp>

namespace formal {
    Automaton::Automaton(std::unordered_set<char> alphabet) :
        alphabet_(std::move(alphabet)), respect_alphabet_(true), no_eps_(true), single_letter_(true),
        initial_state_(nullptr), node_counter_(0) {}

    Automaton::Automaton(Automaton &&other) noexcept :
      initial_state_(other.initial_state_), node_counter_(other.node_counter_),
      states_(std::move(other.states_)), final_states_(std::move(other.final_states_)) {
        other.initial_state_ = nullptr;
        other.node_counter_ = 0;

        for (AutomatonState* state : states_) {
            state->ReassignOwner(this);
        }
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

        for (AutomatonState* state : states_) {
            state->ReassignOwner(this);
        }

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

    void Automaton::OnNewTransition(AutomatonState* src, AutomatonState* dst, const std::string& word) {
        // Update our pessimistic data

        if (word.empty()) {
            no_eps_ = false;
            dfa_ = false;
        }

        if (respect_alphabet_) {
            for (char letter : word) {
                if (alphabet_.find(letter) == alphabet_.end()) {
                    respect_alphabet_ = false;
                    break;
                }
            }
        }

        if (word.length() != 1) {
            single_letter_ = false;
            dfa_ = false;
        }

        if (single_letter_ && dfa_) {
            for (auto& [letter, dst_state] : src->GetTransitions()) {
                if (letter == word && dst_state != dst) {
                    dfa_ = false;
                    break;
                }
            }
        }
    }

    void Automaton::ActualizeProperties() const {
        respect_alphabet_ = true;
        no_eps_ = true;
        single_letter_ = true;
        dfa_ = true;

        for (AutomatonState* state : GetStates()) {
            std::unordered_set<char> present_trans;
            for (auto& [word, dst_state] : state->GetTransitions()) {
                if (word.empty()) {
                    no_eps_ = false;
                    dfa_ = false;
                }

                if (word.size() != 1) {
                    single_letter_ = false;
                    dfa_ = false;
                }

                for (char letter : word) {
                    if (alphabet_.find(letter) == alphabet_.end()) {
                        respect_alphabet_ = false;
                        break;
                    }
                }

                if (single_letter_ && dfa_) {
                    if (present_trans.find(word[0]) != present_trans.end()) {
                        dfa_ = false;
                    } else {
                        present_trans.insert(word[0]);
                    }
                }
            }
        }
    }
}