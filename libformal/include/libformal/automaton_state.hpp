#pragma once

#include <cassert>
#include <map>
#include <libformal/automaton.hpp>

namespace formal {
    /**
     * Class which represents an NFA node
     */
    class AutomatonState {
        friend class Automaton;
        using TransitionSet = std::multimap<std::string, AutomatonState*>;

    public:
        AutomatonState() = delete;
        explicit AutomatonState(Automaton* owner) :
          owner_(owner), final_(false), node_id_(owner->GetNextNodeId()), label_(std::to_string(node_id_)) {}

        bool TransitionPresent(const std::string& word, AutomatonState* dst_state) const {
            return GetTransitionIter(word, dst_state) != transitions_.end();
        }

        void AddTransition(const std::string& word, AutomatonState* dst_state) {
            assert(owner_->Belongs(dst_state));
            if (TransitionPresent(word, dst_state)) {
                return;
            }

            transitions_.insert({word, dst_state});
            dst_state->back_transitions_.insert({word, this});

            owner_->OnNewTransition(this, dst_state, word);
        }

        void RemoveTransition(const std::string& word, AutomatonState* dst_state) {
            assert(owner_->Belongs(dst_state));

            auto transition_iter = GetTransitionIter(word, dst_state);
            assert(transition_iter != transitions_.end());
            if (transition_iter == transitions_.end()) {
                return;
            }

            transitions_.erase(transition_iter);

            auto back_transition_iter = dst_state->GetBackTransitionIter(word, this);
            assert(back_transition_iter != dst_state->back_transitions_.end());
            dst_state->back_transitions_.erase(back_transition_iter);
        }

        const auto& GetTransitions() const {
            return transitions_;
        }

        const auto& GetBackTransitions() const {
            return back_transitions_;
        }

        int GetNodeId() const {
            return node_id_;
        }

        const std::string& GetLabel() const {
            return label_;
        }

        void SetLabel(std::string label) {
            label_ = std::move(label);
        }

        bool IsInitial() const {
            return owner_->GetInitialState() == this;
        }

        bool IsFinal() const {
            return final_;
        }

        void MarkAsInitial() {
            owner_->initial_state_ = this;
        }

        void MarkAsFinal() {
            final_ = true;
            owner_->final_states_.insert(this);
        }

        void UnmarkAsFinal() {
            if (!final_) {
                return;
            }

            final_ = false;
            owner_->final_states_.erase(this);
        }

        void Remove() {
            // Yes, we need a copy
            auto transitions = GetTransitions();
            for (auto& [word, dst_state] : transitions) {
                RemoveTransition(word, dst_state);
            }

            // Yes, we need a copy
            auto back_transitions = GetBackTransitions();
            for (auto& [word, back_state] : back_transitions) {
                back_state->RemoveTransition(word, this);
            }

            UnmarkAsFinal();
            if (owner_->GetInitialState() == this) {
                owner_->ClearInitialState();
            }

            owner_->states_.erase(this);
            delete this;
        }

    private:
        TransitionSet::const_iterator GetTransitionIter(const std::string& word, AutomatonState* state) const {
            return GetTransitionIntl(transitions_, word, state);
        }

        TransitionSet::const_iterator GetBackTransitionIter(const std::string& word, AutomatonState* state) const {
            return GetTransitionIntl(back_transitions_, word, state);
        }

        static TransitionSet::const_iterator GetTransitionIntl(const TransitionSet& transitions,
                                  const std::string& word, AutomatonState* state) {
            auto range = transitions.equal_range(word);
            for (auto iter = range.first; iter != range.second; iter++) {
                if (iter->first == word && iter->second == state) {
                    return iter;
                }
            }

            return transitions.end();
        }

        // Used by Automaton move constructor
        void ReassignOwner(Automaton* owner) {
            owner_ = owner;
        }

    private:
        TransitionSet transitions_;
        TransitionSet back_transitions_;

        int node_id_;
        std::string label_;

        bool final_;

        Automaton* owner_;
    };
}