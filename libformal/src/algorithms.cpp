#include <cassert>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <fmt/core.h>
#include <libformal/automaton_state.hpp>
#include <libformal/algorithms.hpp>

namespace formal {
    namespace {
        using StateSet = std::unordered_set<AutomatonState*>;
        using StatewiseSet = std::unordered_map<AutomatonState*, StateSet>;

        uint64_t PtrToIndex(AutomatonState* state) {
            return reinterpret_cast<uint64_t>(state);
        }

        void BuildEpsClosure(Automaton& automaton, AutomatonState* state, StatewiseSet& closures) {
            if (closures.find(state) != closures.end()) {
                return;
            }

            auto& closure = closures[state];
            closure.insert(state);
            for (auto& [word, dst_state] : state->GetTransitions()) {
                if (word.empty()) {
                    BuildEpsClosure(automaton, dst_state, closures);
                    closure.insert(closures[dst_state].begin(), closures[dst_state].end());
                }
            }
        }

        /// RemoveEpsTransitions step for the single node
        void EpsShortcutify(Automaton &automaton, AutomatonState* state, StatewiseSet& eps_closures) {
            assert(automaton.Belongs(state));

            // Calculate eps closure
            BuildEpsClosure(automaton, state, eps_closures);

            // Add shortcuts & mark current state as final if necessary
            auto& eps_closure = eps_closures[state];
            for (AutomatonState* cls_state : eps_closure) {
                if (cls_state->IsFinal()) {
                    state->MarkAsFinal();
                }

                for (auto& [word, dst_state] : cls_state->GetTransitions()) {
                    if (!word.empty()) {
                        state->AddTransition(word, dst_state);
                    }
                }
            }

            // Remove eps transitions from current state (they aren't need anymore)
            // Yes, we need a copy
            auto old_transitions = state->GetTransitions();
            for (auto& [word, dst_state] : old_transitions) {
                state->RemoveTransition("", dst_state);
            }
        }

        /// Just DFS routine
        void AutomatonWalk(Automaton& automaton, AutomatonState* state, StateSet& visited) {
            if (visited.find(state) != visited.end()) {
                return;
            }

            visited.insert(state);

            for (auto& [word, dst_state] : state->GetTransitions()) {
                AutomatonWalk(automaton, dst_state, visited);
            }
        }

    } // namespace

    void DumpAutomaton(const Automaton &automaton, const std::string& out_file_name) {
        std::ofstream dot_file(fmt::format("{}.dot", out_file_name));
        dot_file << "digraph {\n";
        dot_file << "nowhere [label=\"\", shape=\"none\"]\n";

        auto& final_states = automaton.GetFinalStates();
        for (AutomatonState* state : automaton.GetStates()) {
            std::string shape = final_states.find(state) != final_states.end() ? "doublecircle" : "circle";
            dot_file << fmt::format("{} [shape=\"{}\", label=\"{}\"]\n", PtrToIndex(state), shape, state->GetNodeId());
        }

        if (automaton.GetInitialState() != nullptr) {
            dot_file << fmt::format("nowhere -> {}\n", PtrToIndex(automaton.GetInitialState()));
        }

        for (AutomatonState* src_state : automaton.GetStates()) {
            for (auto& [word, dst_state] : src_state->GetTransitions()) {
                dot_file << fmt::format("{} -> {} [label=\"{}\"]\n",
                                        PtrToIndex(src_state), PtrToIndex(dst_state),
                                        word.empty() ? "eps" : word);
            }
        }

        dot_file << "}\n";
        dot_file.close();

        system(fmt::format("dot {}.dot -Tpng -o {}.png", out_file_name, out_file_name).c_str());
    }

    void RemoveEpsTransitions(Automaton &automaton) {
        StatewiseSet eps_closures;
        for (AutomatonState* state : automaton.GetStates()) {
            EpsShortcutify(automaton, state, eps_closures);
        }

        Optimize(automaton);
    }

    void Optimize(Automaton& automaton) {
        assert(automaton.GetInitialState() != nullptr);

        StateSet visited;
        AutomatonWalk(automaton, automaton.GetInitialState(), visited);

        // Yes, we need a copy
        auto old_states = automaton.GetStates();
        for (AutomatonState* state : old_states) {
            if (visited.find(state) == visited.end()) {
                state->Remove();
            } else if (state->TransitionPresent("", state)) {
                state->RemoveTransition("", state);
            }
        }
    }

    void TransformToDFA(Automaton& automaton) {
        assert(automaton.GetInitialState() != nullptr);

        Automaton dfa;

        AutomatonState* dfa_init_state = dfa.InsertState();
        dfa_init_state->MarkAsInitial();

        // Mapping between states in the old NFA and their representations in DFA
        // Can't use unordered_ versions here =(
        std::map<std::set<AutomatonState*>, AutomatonState*> old2new;
        std::unordered_map<AutomatonState*, std::set<AutomatonState*>> new2old;

        AutomatonState* old_init_state = automaton.GetInitialState();
        old2new[{ old_init_state }] = dfa_init_state;
        new2old[dfa_init_state] = { old_init_state };

        if (old_init_state->IsFinal()) {
            dfa_init_state->MarkAsFinal();
        }

        std::queue<AutomatonState*> queue;
        queue.push(dfa_init_state);
        while (!queue.empty()) {
            AutomatonState* dfa_repr = queue.front();
            queue.pop();

            auto& nfa_states = new2old[dfa_repr];
            std::unordered_map<std::string, std::set<AutomatonState*>> nfa_states_dst;
            for (AutomatonState* nfa_state : nfa_states) {
                for (auto& [word, nfa_state_dst] : nfa_state->GetTransitions()) {
                    nfa_states_dst[word].insert(nfa_state_dst);
                }
            }

            for (auto& [word, nfa_states_dst_word] : nfa_states_dst) {
                AutomatonState* dst_dfa_repr = nullptr;
                if (old2new.find(nfa_states_dst_word) == old2new.end()) {
                    dst_dfa_repr = dfa.InsertState();
                    old2new[nfa_states_dst_word] = dst_dfa_repr;
                    new2old[dst_dfa_repr] = nfa_states_dst_word;

                    for (AutomatonState* nfa_dst_state : nfa_states_dst_word) {
                        if (nfa_dst_state->IsFinal()) {
                            dst_dfa_repr->MarkAsFinal();
                            break;
                        }
                    }

                    queue.push(dst_dfa_repr);
                } else {
                    dst_dfa_repr = old2new[nfa_states_dst_word];
                }

                dfa_repr->AddTransition(word, dst_dfa_repr);
            }
        }

        automaton = std::move(dfa);
    }
}