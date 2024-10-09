#include <gtest/gtest.h>

#include "regex/Regex.hpp"

TEST(RegexExprTests, UnterminatedParentheses) {
  ASSERT_THROW(Regex::Matcher("ds(fd*(c)d"), Regex::ParseError);
}

// Binding closure to empty should be a parse error
TEST(RegexExprTests, ClosureOfNothing) {
  ASSERT_THROW(Regex::Matcher("*"), Regex::ParseError);
  ASSERT_THROW(Regex::Matcher("((()))*"), Regex::ParseError);
  ASSERT_THROW(Regex::Matcher("abcd()*"), Regex::ParseError);
}

TEST(RegexMatcherTests, SimpleExpr) {
  Regex::Matcher reg("a*b(c|d)");

  ASSERT_TRUE(reg.Match("abd"));
  ASSERT_TRUE(reg.Match("aabc"));
  ASSERT_TRUE(reg.Match("bd"));
  ASSERT_FALSE(reg.Match("ad"));
  ASSERT_FALSE(reg.Match("aaabdsod"));
}

// Closure should bind to the preceding primary, allowing it to occur 0 or more
// times
TEST(RegexMatcherTests, Closure) {
  Regex::Matcher reg("d(a|b)*");

  // 0 times is fine
  ASSERT_TRUE(reg.Match("d"));

  // Valid
  ASSERT_TRUE(reg.Match("daaaaaa"));
  ASSERT_TRUE(reg.Match("dbbbbbb"));
  ASSERT_TRUE(reg.Match("dabababba"));

  // One and only one d concat to the left
  ASSERT_FALSE(reg.Match("aad"));
  ASSERT_FALSE(reg.Match("abbaaa"));
  ASSERT_FALSE(reg.Match("ddbaa"));

  // Not "(d(a|b))*"
  ASSERT_FALSE(reg.Match("dadbdbdada"));
}

TEST(RegexMatcherTests, ClosureBeatsAlternation) {
  Regex::Matcher reg("cat|(dog)*");

  // Valid left branch
  ASSERT_TRUE(reg.Match("cat"));

  // Valid right branch
  ASSERT_TRUE(reg.Match(""));
  ASSERT_TRUE(reg.Match("dog"));
  ASSERT_TRUE(reg.Match("dogdogdogdog"));

  // Not "(cat|dog)*"
  ASSERT_FALSE(reg.Match("dogcat"));
  ASSERT_FALSE(reg.Match("catcat"));
}

TEST(RegexMatchTests, EmptyMatchesNothing) {
  Regex::Matcher reg("");

  ASSERT_FALSE(reg.Match("dj"));
  ASSERT_FALSE(reg.Match(""));
}

TEST(RegexMatcherTests, GreedyShouldGiveBack) {
  Regex::Matcher reg("a*a");
  reg.PrintFsa();

  ASSERT_TRUE(reg.Match("aaa"));
}




TEST(Unrelated, Stuff) {
  std::vector<int> vec = {1, 8, 6, 2, 5, 4, 8, 3, 7};
  std::cout << maxArea(vec);
}
