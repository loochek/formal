#include <libformal/earley.hpp>
#include <fmt/core.h>

namespace formal {
    inline CFNonTerminal START_SYMBOL = 'S';

    const std::unordered_set<EarleyState> EarleyStateStorage::empty_set_;

    EarleyParser::EarleyParser(const CFGrammar &grammar): grammar_(grammar) {
        int start_count = grammar_.GetRules().count(START_SYMBOL);
        if (start_count != 1) {
            throw EarleyParseError(fmt::format("Bad grammar - exactly one rule with start non-terminal symbol "
                                   "\"{}\" on the left side must be present", START_SYMBOL));
        }

        start_rule_ = &grammar_[START_SYMBOL].first->second;
        if (!(start_rule_->rhs.size() == 1 && !IsTerminal(start_rule_->rhs[0]))) {
            throw EarleyParseError("Bad grammar - the start rule must have "
                                   "single non-terminal symbol on the right side");
        }
    }

    bool EarleyParser::parse(const std::string& word) {
        word_ = word;
        states_.clear();
        states_.resize(word_.size() + 1);

        states_[0].AddState(EarleyState(*start_rule_, 0, 0, 0));
        Expand(0);

#ifndef NDEBUG
        fmt::print("D(0): {}\n", static_cast<std::string>(states_[0]));
#endif

        for (int i = 1; i <= word.size(); i++) {
            Scan(i);
            Expand(i);
#ifndef NDEBUG
            fmt::print("D({}): {}\n", i, static_cast<std::string>(states_[i]));
#endif
        }

        return states_[word_.size()]['$'].contains(EarleyState(*start_rule_, 1, 0, word_.size()));
    }

    void EarleyParser::Scan(int j) {
        EarleyStateStorage new_states;
        for (const EarleyState& state : states_[j - 1][word_[j - 1]]) {
            new_states.AddState(EarleyState(state.rule, state.rule_dot_pos + 1,
                                            state.parent_read_cnt, state.read_cnt + 1));
        }

        states_[j].Merge(new_states);
    }

    EarleyStateStorage EarleyParser::Predict(const EarleyStateStorage &storage) {
        EarleyStateStorage new_states;
        for (auto& [dot_follower, states] : storage.GetStates()) {
            for (const EarleyState& state : states) {
                auto rules = grammar_[dot_follower];
                for (auto rule_iter = rules.first; rule_iter != rules.second; rule_iter++) {
                    const CFGrammarRule& rule = rule_iter->second;
                    new_states.AddState(EarleyState(rule, 0, state.read_cnt, state.read_cnt));
                }
            }
        }

        return new_states;
    }

    EarleyStateStorage EarleyParser::Complete(const EarleyStateStorage &storage) {
        EarleyStateStorage new_states;
        for (const EarleyState& comp_state : storage['$']) {
            for (const EarleyState& parent_candidate : states_[comp_state.parent_read_cnt][comp_state.rule.lhs]) {
                new_states.AddState(EarleyState(parent_candidate.rule, parent_candidate.rule_dot_pos + 1,
                                                parent_candidate.parent_read_cnt, comp_state.read_cnt));
            }
        }

        return new_states;
    }

    void EarleyParser::Expand(int j) {
        EarleyStateStorage curr_states = states_[j];
        do {
            EarleyStateStorage proc_states = std::move(curr_states);

            curr_states = Complete(proc_states);
            curr_states.Merge(Predict(proc_states));
        } while (states_[j].Merge(curr_states));
    }
}
