#include "LangFrontend.hpp"
#include "Regex.hpp"

#include <cassert>

namespace Regex {
Parser::Parser(std::vector<Token> toks) : toks(toks) {
  this->current = this->toks.begin();
}
FSA Parser::Parse() {
  return Expression();
  if (current != toks.end())
    Error("Did not consume all tokens");
};

FSA Parser::Expression() { return Alternation(); }

FSA Parser::Alternation() {
  FSA expr = Concatenation();

  while (current != toks.end() && current->type == TokenType::Pipe) {
    ++current;

    FSA right = Concatenation();
    if (right.Empty())
      break;

    expr = FSA::Union(expr, right);
  }

  return expr;
}

FSA Parser::Concatenation() {
  FSA alt = Closure();

  while (current != toks.end()) {
    FSA right = Closure();
    if (right.Empty())
      break;

    alt = FSA::Concatenate(alt, right);
  }

  return alt;
}

FSA Parser::Closure() {
  FSA clos = Primary();

  if (current != toks.end() && current->type == TokenType::Star) {
    ++current;

    if (clos.Empty())
      Error("Cannot apply * closure to empty expression");
    clos = FSA::Closure(clos);
  }

  return clos;
}

// todo need a way of representing groupings to bind them to variables
FSA Parser::Primary() {
  FSA fsa;

  if (current == toks.end()) {
    return fsa;
  } else if (current->type == TokenType::Character) {
    fsa.AddStates(2);
    assert(current->lexeme[0] >= 0);
    assert(current->lexeme.size() == 1);
    fsa.AddTransition(0, 1, current->lexeme[0]);
    fsa.AcceptState(1);
    ++current;
  } else if (current->type == TokenType::OpenParen) {
    ++current;
    fsa = Expression();
    if (current == toks.end() || current->type != TokenType::CloseParen)
      Error("Unclosed parentheses");
    ++current;
  }

  return fsa;
}

Lexer::Lexer(std::string_view str) : str(str) { current = this->str.begin(); }

std::vector<Token> Lexer::Lex() {
  using enum TokenType;
  while (current != str.end()) {
    char cur = *(current++);

    switch (cur) {
    case '(':
      toks.emplace_back(OpenParen, "(");
      break;
    case ')':
      toks.emplace_back(CloseParen, ")");
      break;
    case '[':
      toks.emplace_back(OpenBrace, "[");
      break;
    case ']':
      toks.emplace_back(CloseBrace, "]");
      break;
    case '-':
      toks.emplace_back(Dash, "-");
      break;
    case '+':
      toks.emplace_back(Plus, "+");
      break;
    case '*':
      toks.emplace_back(Star, "*");
      break;
    case '|':
      toks.emplace_back(Pipe, "|");
      break;
    default:
      toks.emplace_back(Character, std::string(1, cur));
      break;
    }
  }
  return toks;
}

void Parser::Error(std::string_view msg) {
  std::cout << "Parse Error: " << msg << '\n';
  throw ParseError{};
}
} // namespace Regex
