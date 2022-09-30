#pragma once

#include <cassert>
#include <map>
#include <libformal/automaton.hpp>

namespace formal {
    /**
     * Class which represents an NFA node
     */
    class AutomatonState {
        using TransitionSet = std::multimap<std::string, AutomatonState*>;
    public:
        AutomatonState() = delete;
        explicit AutomatonState(Automaton* owner) : owner_(owner), final_(false), node_id_(owner->GetNextNodeId()) {}

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
        }

        void RemoveTransition(const std::string& word, AutomatonState* dst_state) {
            assert(owner_->Belongs(dst_state));

            auto transition_iter = GetTransitionIter(word, dst_state);
            if (transition_iter == transitions_.end()) {
                return;
            }

            transitions_.erase(transition_iter);

            auto back_transition_iter = dst_state->GetBackTransitionIter(word, this);
            dst_state->back_transitions_.erase(back_transition_iter);
        }

        const auto GetTransitions() const {
            return transitions_;
        }

        const auto GetBackTransitions() const {
            return back_transitions_;
        }

        int GetNodeId() const {
            return node_id_;
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
            for (auto& [word, back_state] : GetBackTransitions()) {
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

    private:
        TransitionSet transitions_;
        TransitionSet back_transitions_;

        int node_id_;
        bool final_;

        Automaton* const owner_;
    };
}