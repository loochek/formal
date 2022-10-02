#include <cassert>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
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
                if (word.empty()) {
                    state->RemoveTransition("", dst_state);
                }
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

        /// Used by DFA builder
        std::string GenSetLabel(const std::set<AutomatonState*>& states) {
            std::string label = "[";
            for (AutomatonState* state: states) {
                label.append(state->GetLabel());
                label.append(", ");
            }

            label.resize(label.size() - 2);
            label.append("]");
            return label;
        }

        /// Used by NFAToRegExp
        void RenameEpsTransitions(Automaton& automaton) {
            for (AutomatonState* state : automaton.GetStates()) {
                // Yes, we need a copy
                auto transitions = state->GetTransitions();
                for (auto& [word, dst_state] : transitions) {
                    if (word.empty()) {
                        state->RemoveTransition(word, dst_state);
                        state->AddTransition("1", dst_state);
                    }
                }
            }
        }

        /// Used by NFAToRegExp
        void CollapseMultipleEdges(Automaton& automaton) {
            for (AutomatonState* state : automaton.GetStates()) {
                std::unordered_map<AutomatonState*, std::vector<std::string>> transitions_by_state;
                for (auto& [word, dst_state] : state->GetTransitions()) {
                    transitions_by_state[dst_state].push_back(word);
                }

                for (auto& [dst_state, words] : transitions_by_state) {
                    if (words.size() == 1) {
                        continue;
                    }

                    for (std::string& word : words) {
                        state->RemoveTransition(word, dst_state);
                    }

                    std::string new_word = words[0];
                    for (int i = 1; i < words.size(); i++) {
                        new_word.append("+");
                        new_word.append(words[i]);
                    }

                    state->AddTransition(new_word, dst_state);
                }
            }
        }

        /// Used by NFAToRegExp
        std::string AutoBrace(const std::string& regexp) {
            if (regexp.find('+') != -1 || regexp.find('*') != -1) {
                return fmt::format("({})", regexp);
            } else {
                return regexp;
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
            dot_file << fmt::format("{} [shape=\"{}\", label=\"{}\"]\n", PtrToIndex(state), shape, state->GetLabel());
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
        assert(automaton.GetInitialState() != nullptr && automaton.IsSingleLetter());

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
                for (auto& [letter, nfa_state_dst] : nfa_state->GetTransitions()) {
                    nfa_states_dst[letter].insert(nfa_state_dst);
                }
            }

            for (auto& [word, nfa_states_dst_letter] : nfa_states_dst) {
                AutomatonState* dst_dfa_repr = nullptr;
                if (old2new.find(nfa_states_dst_letter) == old2new.end()) {
                    dst_dfa_repr = dfa.InsertState();
                    dst_dfa_repr->SetLabel(GenSetLabel(nfa_states_dst_letter));

                    old2new[nfa_states_dst_letter] = dst_dfa_repr;
                    new2old[dst_dfa_repr] = nfa_states_dst_letter;

                    for (AutomatonState* nfa_dst_state : nfa_states_dst_letter) {
                        if (nfa_dst_state->IsFinal()) {
                            dst_dfa_repr->MarkAsFinal();
                            break;
                        }
                    }

                    queue.push(dst_dfa_repr);
                } else {
                    dst_dfa_repr = old2new[nfa_states_dst_letter];
                }

                dfa_repr->AddTransition(word, dst_dfa_repr);
            }
        }

        automaton = std::move(dfa);
    }

    void CompleteDFA(Automaton& automaton) {
        assert(automaton.IsDFA());

        AutomatonState* drain = automaton.InsertState();
        drain->SetLabel("drain");

        for (AutomatonState* state : automaton.GetStates()) {
            if (state == drain) {
                continue;
            }

            std::unordered_set<char> present_trans;
            for (auto& [letter, dst_state] : state->GetTransitions()) {
                present_trans.insert(letter[0]);
            }

            for (char letter : automaton.GetAlphabet()) {
                if (present_trans.find(letter) == present_trans.end()) {
                    state->AddTransition(std::string(1,letter), drain);
                }
            }
        }

        if (drain->GetBackTransitions().empty()) {
            // Remove drain if already CDFA
            drain->Remove();
        } else {
            // Add drain loops otherwise
            for (char letter : automaton.GetAlphabet()) {
                drain->AddTransition(std::string(1,letter), drain);
            }
        }
    }

    void ComplementCDFA(Automaton& automaton) {
        assert(automaton.IsDFA()); // TODO: CDFA check

        for (AutomatonState *state : automaton.GetStates()) {
            if (state->IsFinal()) {
                state->UnmarkAsFinal();
            } else {
                state->MarkAsFinal();
            }
        }
    }

    void SinglifyFinalState(Automaton &automaton) {
        if (automaton.GetFinalStates().size() <= 1) {
            return;
        }

        // Yes, we need a copy
        auto old_finals = automaton.GetFinalStates();

        AutomatonState* new_final = automaton.InsertState();
        new_final->MarkAsFinal();

        for (AutomatonState* old_final : old_finals) {
            old_final->UnmarkAsFinal();
            old_final->AddTransition("", new_final);
        }

        assert(automaton.GetFinalStates().size() == 1);
    }

    void NFAToRegExp(Automaton &automaton) {
        int finals_count = automaton.GetFinalStates().size();
        if (finals_count == 0 || automaton.GetInitialState() == nullptr) {
            return;
        } else if (finals_count > 0) {
            SinglifyFinalState(automaton);
        }

        assert(automaton.GetFinalStates().size() == 1);

        RenameEpsTransitions(automaton);
        CollapseMultipleEdges(automaton);

        while (true) {
            AutomatonState* victim = nullptr;
            for (AutomatonState* victim_candidate : automaton.GetStates()) {
                if (!(victim_candidate->IsFinal() || victim_candidate->IsInitial())) {
                    victim = victim_candidate;
                    break;
                }
            }

            if (victim == nullptr) {
                break;
            }

            const std::string* loop_re = nullptr;
            for (auto& [dst_re, dst_state] : victim->GetTransitions()) {
                if (dst_state == victim) {
                    loop_re = &dst_re;
                    break;
                }
            }

            for (auto& [src_re, src_state] : victim->GetBackTransitions()) {
                if (src_state == victim) {
                    continue;
                }

                for (auto& [dst_re, dst_state] : victim->GetTransitions()) {
                    if (dst_state == victim) {
                        continue;
                    }

                    std::string shortcut_re;
                    if (loop_re == nullptr && dst_re == "1") {
                        shortcut_re = fmt::format("{}", src_re);
                    }
                    else if (loop_re == nullptr && src_re == "1") {
                        shortcut_re = fmt::format("{}", dst_re);
                    }
                    else if (src_re == "1" && dst_re == "1" && loop_re != nullptr) {
                        shortcut_re = fmt::format("({})*", *loop_re);
                    }
                    else if (loop_re == nullptr || *loop_re == "1") {
                        shortcut_re = fmt::format("{}{}", AutoBrace(src_re), AutoBrace(dst_re));
                    }
                    else if (src_re == "1") {
                        shortcut_re = fmt::format("({})*{}", *loop_re, AutoBrace(dst_re));
                    }
                    else if (dst_re == "1") {
                        shortcut_re = fmt::format("{}({})*", AutoBrace(src_re), *loop_re);
                    } else {
                        shortcut_re = fmt::format("{}({})*{}", AutoBrace(src_re), *loop_re, AutoBrace(dst_re));
                    }

                    src_state->AddTransition(shortcut_re, dst_state);
                }
            }

            victim->Remove();
            CollapseMultipleEdges(automaton);
        }

        assert(automaton.GetStates().size() <= 2);
    }

    bool DFAReadWord(Automaton &dfa, const std::string& word) {
        assert(dfa.IsDFA());

        AutomatonState* curr_state = dfa.GetInitialState();
        for (char letter : word) {
            auto iter = curr_state->GetTransitions().equal_range(std::string(1, letter)).first;
            if (iter->first != std::string(1, letter)) {
                return false;
            } else {
                curr_state = iter->second;
            }
        }

        return curr_state->IsFinal();
    }
}