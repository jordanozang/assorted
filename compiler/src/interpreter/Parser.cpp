#include "Parser.hpp"

#include <iostream>

Parser::Parser(std::vector<Token> toks) : toks(toks) {
  this->current = this->toks.begin();
}

void Parser::Error(std::string_view msg) {
  std::cout << "Parse error: " << msg << '\n';
  throw ParseError{};
}

ExprHandle Parser::Expression() { return Term(); }

ExprHandle Parser::Term() {
  ExprHandle expr = Factor();

  while (current != toks.end() && (current->type == TokenType::Plus ||
                                   current->type == TokenType::Minus)) {
    TokenType op = current->type;
    ++current;

    ExprHandle right = Factor();
    expr = std::make_shared<Expr>(BinaryExpr{op, expr, right});
  }

  return expr;
}

ExprHandle Parser::Factor() {
  ExprHandle expr = Unary();

  while (current != toks.end() && (current->type == TokenType::Star ||
                                   current->type == TokenType::Slash)) {
    TokenType op = current->type;
    ++current;

    ExprHandle right = Unary();
    expr = std::make_shared<Expr>(BinaryExpr{op, expr, right});
  }

  return expr;
}

ExprHandle Parser::Unary() {
  if (current != toks.end() && current->type == TokenType::Minus) {
    TokenType op = current->type;
    ++current;

    return std::make_shared<Expr>(UnaryExpr{op, Unary()});
  } else {
    return Primary();
  }
}

ExprHandle Parser::Primary() {
  if (current == toks.end()) {
    return nullptr;
  }

  if (current->type == TokenType::Integer) {

    ExprHandle val =
        std::make_shared<Expr>(IntegerLit{std::stoi(current->lexeme)});
    ++current;
    return val;
  }

  if (current->type == LeftParen) {
    ++current;

    ExprHandle expr = Expression();

    if (current == toks.end() || current->type != RightParen)
      Error("Unterminated parentheses");

    ++current;
    return expr;
  }

  return nullptr;
}

ExprHandle Parser::Parse() { return Expression(); }


int main() {
	std::string input = "(2 * 3) + 4";
	Lexer lexer(input);
	auto lexed = lexer.Lex();
	Parser parser(lexed);
	ExprHandle ast_root = parser.Parse();

	std::cout << "Input String: " <<  input << '\n';
	std::cout << "Output AST: " << std::visit(AstPrinter{}, *ast_root) << '\n';

}
