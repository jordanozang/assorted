#pragma once

/*
 * Implementation of the language frontend for regex. Includes lexer and
 * recursive descent parser. The parser constructs the fsa in place by applying
 * Thompson's constructions to the sub-trees.
 */

#include <string>
#include <vector>

#include "FSA.hpp"

namespace Regex {
enum class TokenType {
  OpenParen,
  CloseParen,
  OpenBrace,
  CloseBrace,
  Dash,
  Plus,
  Star,
  Pipe,
  Character,
};

struct Token {
  TokenType type;
  std::string lexeme;
};

class Lexer {
private:
  std::string str;
  std::string::const_iterator current;

  std::vector<Token> toks;

public:
  Lexer(std::string_view str);

  std::vector<Token> Lex();
};

// character (lr) > closure (bind one on the right)  > concatenation(lr) >
// alternation (lr)

/*
 * Expression -> Alternation
 * Alternation -> Concatenation ("|"  Concatenation)*
 * Concatenation -> Closure Closure*
 * Closure -> Primary "*"?
 * Primary -> CHARACTER | "(" Expression ")"
 */
class Parser {
private:
  std::vector<Token> toks;
  std::vector<Token>::const_iterator current;

  void Error(std::string_view msg);

  FSA Expression();
  FSA Alternation();
  FSA Concatenation();
  FSA Closure();
  FSA Primary();

public:
  Parser(std::vector<Token> toks);
  FSA Parse();
};

} // namespace Regex
