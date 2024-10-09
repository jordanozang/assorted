#include "Lexer.hpp"

#include <cassert>
#include <format>
#include <iostream>

Token::Token(TokenType type, std::string lexeme) : type(type), lexeme(lexeme) {}
Token::Token(TokenType type) : type(type), lexeme("") {}

// enum TokenType { LeftParen, RightParen, Plus, Minus, Star, Slash, Integer,
std::string FormatTokenType(TokenType type) {
  switch (type) {
  case LeftParen:
    return "(";
  case RightParen:
    return ")";
  case Plus:
    return "+";
  case Minus:
    return "-";
  case Star:
    return "*";
  case Slash:
    return "/";
  case Integer:
    return "INTEGER";
  }

  assert(false);
  return "INVALID";
}

Lexer::Lexer(std::string_view input) : input(input) {
  this->current = this->input.begin();
}

void Lexer::Error(std::string_view message) {
  std::cout << "Lex Error: " << message << '\n';
  throw LexError{};
}

void Lexer::Integer() {
  std::string::const_iterator start = current - 1;

  while (current != input.end() && *current >= '0' && *current <= '9') {
    ++current;
  }

  toks.emplace_back(TokenType::Integer, std::string(start, current));
}

std::vector<Token> Lexer::Lex() {
  while (current != input.end()) {
    char cur = *(current++);

    switch (cur) {
    case '(':
      toks.emplace_back(TokenType::LeftParen);
      break;
    case ')':
      toks.emplace_back(TokenType::RightParen);
      break;
    case '+':
      toks.emplace_back(TokenType::Plus);
      break;
    case '-':
      toks.emplace_back(TokenType::Minus);
      break;
    case '*':
      toks.emplace_back(TokenType::Star);
      break;
    case '/':
      toks.emplace_back(TokenType::Slash);
      break;
    case ' ':
      break;
    default:
      if (cur >= '0' && cur <= '9')
        Integer();
      else {
        Error(std::format("Illegal input character '{}'", cur));
      }
      break;
    }
  }

  return toks;
}
