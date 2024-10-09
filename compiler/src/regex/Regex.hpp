#pragma once

#include "FSA.hpp"

namespace Regex {

struct ParseError {};

class Matcher {
private:
  FSA fsa;

public:
  /*
   * Ctor. May throw a ParseError
   */
  Matcher(std::string_view expression);

  /*
   * Check if the input string is in the language
   */
  bool Match(std::string_view);

  inline void PrintFsa() {
	  std::cout << fsa;
  }

};

} // namespace Regex
