#include <gtest/gtest.h>

#include <algorithm>
#include <execution>

#include "regex/FSA.hpp"

TEST(FSATests, ConsumeStringEvenNumberOfOnes) {
  FSA fsa; // 0 - "0", 1 - "1"
  fsa.AddStates(2);

  fsa.AddTransition(0, 0, 0);
  fsa.AddTransition(0, 1, 1);
  fsa.AddTransition(1, 1, 0);
  fsa.AddTransition(1, 0, 1);

  fsa.AcceptState(0);

  auto tokenized = [](std::string input_str) {
    std::vector<int64_t> tokenized(input_str.size());
    std::transform(std::execution::par, input_str.begin(), input_str.end(),
                   tokenized.begin(), [](char c) {
                     switch (c) {
                     case '0':
                       return 0;
                     case '1':
                       return 1;
                     default:
                       return 100;
                     }
                   });

    return tokenized;
  };

  ASSERT_FALSE(fsa.ConsumeString(tokenized("1101")));
  ASSERT_TRUE(fsa.ConsumeString(tokenized("11101")));
}

TEST(FSATests, ConcatenationTest) {
  FSA fsa1;
  fsa1.AddStates(2);
  fsa1.AddTransition(0, 1, 1);
  fsa1.AcceptState(1);

  FSA fsa2;
  fsa2.AddStates(2);
  fsa2.AddTransition(0, 1, 2);
  fsa2.AcceptState(1);

  FSA fsa3 = FSA::Concatenate(fsa1, fsa2);
  std::cout << fsa3 << std::endl;
  fsa3.Determinize();
  std::cout << fsa3 << std::endl;

  ASSERT_TRUE(fsa3.ConsumeString({1, 2}));
  ASSERT_FALSE(fsa3.ConsumeString({2, 1}));
}

TEST(FSATests, ClosureTest) {
  FSA fsa;
  fsa.AddStates(2);
  fsa.AddTransition(0, 1, 0);
  fsa.AcceptState(1);

  FSA closure = FSA::Closure(fsa);
  std::cout << closure << std::endl;
  closure.Determinize();
  std::cout << closure << std::endl;

  ASSERT_TRUE(closure.ConsumeString({}));
  ASSERT_TRUE(closure.ConsumeString({0, 0, 0}));
  ASSERT_FALSE(closure.ConsumeString({0, 1, 0}));
}

TEST(FSATests, UnionTest) {
  FSA fsa1;
  fsa1.AddStates(2);
  fsa1.AddTransition(0, 1, 0);
  fsa1.AcceptState(1);

  FSA fsa2;
  fsa2.AddStates(2);
  fsa2.AddTransition(0, 1, 1);
  fsa2.AcceptState(1);

  FSA fsa3 = FSA::Union(fsa1, fsa2);
  std::cout << fsa3 << std::endl;
  fsa3.Determinize();
  std::cout << fsa3 << std::endl;

  ASSERT_TRUE(fsa3.ConsumeString({0}));
  ASSERT_TRUE(fsa3.ConsumeString({1}));
  ASSERT_FALSE(fsa3.ConsumeString({0, 0}));
  ASSERT_FALSE(fsa3.ConsumeString({}));
}
