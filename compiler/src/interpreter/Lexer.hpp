#pragma once

#include <string>
#include <vector>

enum TokenType { LeftParen, RightParen, Plus, Minus, Star, Slash, Integer };
std::string FormatTokenType(TokenType type);

struct Token {
  TokenType type;
  std::string lexeme;

  Token(TokenType type, std::string lexeme);
  Token(TokenType type);
};

struct LexError {};

class Lexer {
private:
  std::string input;
  std::string::const_iterator current;

  std::vector<Token> toks;

  void AddToken(TokenType type);

  void Integer();

  void Error(std::string_view message);

public:
  Lexer(std::string_view input);

  std::vector<Token> Lex();
};
