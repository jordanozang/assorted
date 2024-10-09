#include <cassert>
#include <vector>

#include "FSA.hpp"
#include "LangFrontend.hpp"
#include "Regex.hpp"

namespace Regex {

Matcher::Matcher(std::string_view expression) {
  Lexer lex(expression);
  std::vector<Token> toks = lex.Lex();
  Parser parser(toks);
  fsa = parser.Parse();
  fsa.Determinize();
  fsa.Minimize();
}

bool Matcher::Match(std::string_view str) {
	std::vector<int64_t> chars(str.begin(), str.end());
	return fsa.ConsumeString(chars);
}



} // namespace Regex
