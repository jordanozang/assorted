#pragma once

#include <cstdint>
#include <iostream>
#include <set>
#include <vector>

/*
 * Simple FSA implementation with state set {0, ..., n_states - 1} and alphabet
 * {0, ..., n_alphabet - 1}. Negative labels are reserved for special tokens.
 * Mostly just using vectors to keep things abundantly simple and because I
 * don't really know the usage patterns well enough to optimize until I actually
 * have more familiarity with the algorithms. I'm also using overkill stuff like
 * uint64_t rather than templating on the type or something.
 */
class FSA {
public:
  struct Transition {
    int64_t label;
    uint64_t to;
  };

  static constexpr int64_t Eps{-1};

private:
  uint64_t start_state{0};
  // transitions indexed by start. can't use a map for the transitions because
  // there can be multiple eps transitions. if we specialized for a DFA, we
  // could. Generally, I expect the transition matrix to be very sparse anyways
  // though. That's why I'm using this rather than an adjacency matrix
  // representation.
  std::vector<std::vector<Transition>> transitions{};

  std::set<uint64_t> accept_states{};

  std::set<uint64_t> EpsilonClosure(uint64_t state) const;

public:
  void AddStates(uint64_t how_many = 1);

  void AcceptState(uint64_t state);

  void StartState(uint64_t state);

  void AddTransition(uint64_t from, uint64_t to, int64_t label);

  // Consume a String. Currently ub if the FSA is not deterministic.
  bool ConsumeString(std::vector<int64_t> toks) const;

  bool ConsumeRange(std::vector<int64_t>::const_iterator begin, std::vector<int64_t>::const_iterator end, uint64_t state) const;

  void Determinize();

  inline void Minimize() {
    // TODO
  }

  bool Empty() const;

  static FSA Concatenate(const FSA &left, const FSA &right);

  static FSA Union(const FSA &left, const FSA &right);

  static FSA Closure(const FSA &left);

  friend std::ostream &operator<<(std::ostream &os, const FSA &fsa);
};

std::ostream &operator<<(std::ostream &os, const FSA &fsa);
