#include "FSA.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <set>
#include <vector>

void FSA::AddStates(uint64_t how_many) {
  transitions.resize(transitions.size() + how_many);
}

void FSA::AddTransition(uint64_t from, uint64_t to, int64_t label) {
  assert(from < transitions.size());
  assert(to < transitions.size());

  transitions[from].emplace_back(label, to);
}

void FSA::AcceptState(uint64_t state) {
  assert(state < transitions.size());

  accept_states.insert(state);
}

void FSA::StartState(uint64_t state) {
  assert(state < transitions.size());

  start_state = state;
}

bool FSA::ConsumeRange(std::vector<int64_t>::const_iterator begin,
                       std::vector<int64_t>::const_iterator end,
                       uint64_t state) const {

  if (begin == end) {
    if (accept_states.count(state))
      return true;

    return false;
  }

  auto transition = transitions[state].begin();

  while ((transition = std::find_if(transition, transitions[state].end(),
                                    [begin](const Transition &x) {
                                      return x.label == *begin;
                                    })) != transitions[state].end()) {

    if (ConsumeRange(begin + 1, end, transition->to))
      return true;

    ++transition;
  }

  return false;
}

bool FSA::ConsumeString(std::vector<int64_t> toks) const {
  uint64_t state = start_state;

  if (transitions.empty())
    return false;

  return ConsumeRange(toks.begin(), toks.end(), state);

  for (int64_t tok : toks) {
    auto transition =
        std::find_if(transitions[state].begin(), transitions[state].end(),
                     [tok](const Transition &x) { return x.label == tok; });

    if (transition == transitions[state].end())
      return false;

    state = transition->to;
  }

  if (!accept_states.count(state))
    return false;

  return true;
}

/*
 * Just using naive algorithm rather than offline one for now
 */
std::set<uint64_t> FSA::EpsilonClosure(uint64_t state) const {
  assert(state < transitions.size());

  std::set<uint64_t> res{};

  std::queue<uint64_t> work{};
  work.push(state);

  while (!work.empty()) {
    uint64_t curr_state = work.front();
    work.pop();

    res.insert(curr_state);

    for (const Transition &trans : transitions[curr_state]) {
      if (trans.label == Eps && !res.contains(trans.to))
        work.push(trans.to);
    }
  }

  return res;
}

/*
 *
 */
void FSA::Determinize() {
  if (transitions.size() == 0)
    return;

  std::vector<std::set<uint64_t>> new_states;
  std::vector<std::vector<Transition>> new_transitions;

  new_states.push_back(EpsilonClosure(start_state));
  new_transitions.resize(1);

  std::queue<uint64_t> to_handle;
  to_handle.push(0);

  while (!to_handle.empty()) {
    uint64_t curr_state = to_handle.front();
    std::set<uint64_t> curr = new_states[curr_state];
    to_handle.pop();

    for (uint64_t src_state : curr) {
      for (const Transition &src_transition : transitions[src_state]) {
        if (src_transition.label == Eps)
          continue;

        std::set<uint64_t> trans_closure = EpsilonClosure(src_transition.to);

        uint64_t found_state{0};
        for (; found_state < new_states.size(); ++found_state) {
          if (std::includes(new_states[found_state].begin(),
                            new_states[found_state].end(),
                            trans_closure.begin(), trans_closure.end()))
            break;
        }

        if (found_state == new_states.size()) {
          new_states.push_back(trans_closure);
          new_transitions.resize(new_transitions.size() + 1);
          new_transitions[curr_state].emplace_back(src_transition.label,
                                                   new_states.size() - 1);

          to_handle.push(new_states.size() - 1);
        } else {
          new_transitions[curr_state].emplace_back(src_transition.label,
                                                   found_state);
        }
      }
    }
  }

  transitions = new_transitions;
  start_state = 0;

  std::set<uint64_t> new_accept_states;

  for (uint64_t i = 0; i < new_states.size(); ++i) {
    for (uint64_t src_state : new_states[i]) {
      if (accept_states.contains(src_state))
        new_accept_states.insert(i);
    }
  }

  accept_states = new_accept_states;
}

/*
 * Start with the left fsa. For each accept state, make an epsilon
 * transition to the start state of the right fsa. The accept states of the
 * final fsa will be the ones from the right fsa
 */
FSA FSA::Concatenate(const FSA &left, const FSA &right) {
  if (right.Empty())
    return left;

  FSA res{left};

  const uint64_t right_n_states = right.transitions.size();
  const uint64_t left_n_states = left.transitions.size();

  res.AddStates(right_n_states);

  for (size_t i = 0; i < right.transitions.size(); ++i) {
    for (size_t j = 0; j < right.transitions[i].size(); ++j) {
      res.AddTransition(i + left_n_states,
                        right.transitions[i][j].to + left_n_states,
                        right.transitions[i][j].label);
    }
  }

  res.accept_states.clear();

  for (uint64_t accepted : left.accept_states)
    res.AddTransition(accepted, right.start_state + left_n_states, FSA::Eps);

  for (uint64_t accepted : right.accept_states)
    res.accept_states.insert(accepted + left_n_states);

  return res;
}

/*/
 * Insert a new start state
 * Add a new final accept state
 * Add eps transitions from all accept states to new accept state
 * Add eps transitions from all accept states back to old start state
 * Add transitions from new start to the old start and the final state
 *
 */
FSA FSA::Closure(const FSA &left) {
  FSA res{left};

  res.AddStates(2);
  uint64_t new_accept = res.transitions.size() - 1;
  uint64_t new_start = res.transitions.size() - 2;

  res.AddTransition(new_start, res.start_state, FSA::Eps);

  res.AddTransition(new_start, new_accept, FSA::Eps);

  for (uint64_t acc : res.accept_states) {
    res.AddTransition(acc, new_accept, FSA::Eps);
    res.AddTransition(acc, res.start_state, FSA::Eps);
  }

  res.accept_states = {new_accept};
  res.start_state = new_start;

  return res;
}

FSA FSA::Union(const FSA &left, const FSA &right) {
  FSA res{left};
  res.AddStates(right.transitions.size() + 2);

  uint64_t new_accept{res.transitions.size() - 1};
  uint64_t new_start{res.transitions.size() - 2};

  for (size_t i = 0; i < right.transitions.size(); ++i) {
    for (size_t j = 0; j < right.transitions[i].size(); ++j) {
      res.AddTransition(i + left.transitions.size(),
                        right.transitions[i][j].to + left.transitions.size(),
                        right.transitions[i][j].label);
    }
  }

  res.AddTransition(new_start, left.start_state, FSA::Eps);
  res.AddTransition(new_start, left.transitions.size() + right.start_state,
                    FSA::Eps);
  res.start_state = new_start;

  res.accept_states = {new_accept};

  for (uint64_t acc : left.accept_states)
    res.AddTransition(acc, new_accept, FSA::Eps);

  for (uint64_t acc : right.accept_states)
    res.AddTransition(left.transitions.size() + acc, new_accept, FSA::Eps);

  return res;
}

std::ostream &operator<<(std::ostream &os, const FSA &fsa) {
  os << "start state: " << fsa.start_state << '\n';
  os << "accept states: ";
  for (uint64_t acc : fsa.accept_states)
    os << acc << " ";
  os << '\n';
  os << "transitions: \n";
  for (size_t i = 0; i < fsa.transitions.size(); ++i) {
    for (size_t j = 0; j < fsa.transitions[i].size(); ++j) {
      os << i << " " << fsa.transitions[i][j].to << " "
         << (fsa.transitions[i][j].label) << '\n';
    }
  }

  return os;
};

bool FSA::Empty() const {
  for (const auto &src : transitions) {
    if (!src.empty())
      return false;
  }

  return true;
}

// It might just make more sense to treat deterministic/non-deterministic
// FSA's as entirely different types.

// AddArc
// AddState
// AcceptState
// Alphabet (0 should be eps probably)
