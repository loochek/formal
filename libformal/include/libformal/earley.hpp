#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fmt/core.h>
#include <libformal/grammar.hpp>

namespace formal {
    struct EarleyState {
    public:
        explicit EarleyState(const CFGrammarRule &rule, int rule_dot_pos, int parent_read_cnt, int read_cnt) :
                rule(rule), rule_dot_pos(rule_dot_pos), parent_read_cnt(parent_read_cnt), read_cnt(read_cnt) {}

        EarleyState(const EarleyState& other) = default;

        bool operator==(const EarleyState &other) const {
            return rule == other.rule && rule_dot_pos == other.rule_dot_pos &&
                   parent_read_cnt == other.parent_read_cnt && read_cnt == other.read_cnt;
        }

        explicit operator std::string() const {
            std::string dotted_rhs = rule.rhs;
            dotted_rhs.insert(rule_dot_pos, ".");
            return fmt::format("({} => {}, {}) in D({})", rule.lhs, dotted_rhs, parent_read_cnt, read_cnt);
        }

    public:
        const CFGrammarRule &rule;

        /// Rule symbol index which follows the dot (or whole RHS size if the dot is at the end)
        int rule_dot_pos;
        int parent_read_cnt;
        int read_cnt;
    };
}

MAKE_HASHABLE(formal::EarleyState, t.rule, t.rule_dot_pos, t.parent_read_cnt, t.read_cnt);

namespace formal {
    /**
     * Specialized state storage with indexing based on symbol that follows the dot (D[X])
     */
    class EarleyStateStorage {
    public:
        EarleyStateStorage() : empty_(true) {}

        EarleyStateStorage(const EarleyStateStorage& other) = default;
        EarleyStateStorage& operator=(const EarleyStateStorage& other) = default;

        EarleyStateStorage(EarleyStateStorage&& other) noexcept :
            states_(std::move(other.states_)), empty_(other.empty_) {
            other.empty_ = true;
        }

        EarleyStateStorage& operator=(EarleyStateStorage&& other) noexcept {
            states_ = std::move(other.states_);
            empty_ = other.empty_;
            other.empty_ = true;

            return *this;
        }

        bool AddState(EarleyState state) {
            empty_ = false;

            CFRuleSymbol dot_follower = '$';
            if (state.rule_dot_pos != state.rule.rhs.size()) {
                dot_follower = state.rule.rhs[state.rule_dot_pos];
            }

            return states_[dot_follower].insert(state).second;
        }

        const auto& GetStates() const {
            return states_;
        }

        bool Empty() const {
            return empty_;
        }

        const auto& operator[](CFRuleSymbol symbol) const {
            if (states_.find(symbol) == states_.end()) {
                return empty_set_;
            }

            return states_.find(symbol)->second;
        }

        bool Merge(const EarleyStateStorage& other) {
            bool added = false;
            for (auto& [symbol, states] : other.GetStates()) {
                for (const EarleyState& state : states) {
                    added |= AddState(state);
                }
            }

            return added;
        }

        explicit operator std::string() const {
            std::string result = fmt::format("EarleyStateStorage at {}:\n", static_cast<const void*>(this));

            for (auto& [dot_follower, states] : states_) {
                result += fmt::format("\tDot followed by {}:\n", dot_follower);

                for (const EarleyState& state : states) {
                    result += fmt::format("\t\t{}\n", static_cast<std::string>(state));
                }
            }

            return result;
        }

    private:
        std::unordered_map<CFRuleSymbol, std::unordered_set<EarleyState>> states_;
        static const std::unordered_set<EarleyState> empty_set_;

        bool empty_;
    };

    class EarleyParseError : public std::runtime_error
    {
    public:
        explicit EarleyParseError(const std::string& what = "") : std::runtime_error(what) {}
    };

    class EarleyParser {
    public:
        explicit EarleyParser(const CFGrammar& grammar);

        bool parse(const std::string& word);

    private:
        /// Applies Scan to D[j]
        void Scan(int j);
        /// Applies Predict to given states
        EarleyStateStorage Predict(const EarleyStateStorage& states);
        /// Applies Complete to given states
        EarleyStateStorage Complete(const EarleyStateStorage& states);

        /// Incrementally applies Predict and Complete to D_j while applicable
        void Expand(int j);

    private:
        const CFGrammar& grammar_;
        const CFGrammarRule* start_rule_;

        std::string_view word_;
        /// D_j arrays
        std::vector<EarleyStateStorage> states_;
    };
}