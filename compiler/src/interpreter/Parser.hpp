#pragma once

#include "Lexer.hpp"

#include <memory>
#include <variant>

// build a simple interpreter where all variables are float and all declarations are global


/*
 * Program -> Decl*
 * Decl -> FnDecl | VarDecl | Stmt
 *
 * Stmt -> ExprStmt | WhileStmt | ForStmt | IfStmt | Block
 *
 * ExprStmt -> Expr ";"
 *
 * WhileStmt -> "while" "(" Expr ")" Stmt
 *
 * ForStmt -> "for" "(" (VarDecl" | ExprStmt | ";") Expr? ";" Expr? ")" Stmt
 *
 * Block -> "{" Decl* "}"
 *
 * Expr -> Equality
 * Equality -> Comparison (("==" | "!=") Comparison)*
 * Comparison -> Term ((">" | ">=" | "<" | "<=") Term)*
 * Term -> Factor (("-" | "+") Factor)*
 * Factor -> Unary (("/" | "*") Unary)*
 * Unary -> ("!" | "-")* Primary
 * Primary -> NUMBER | STRING | "true" | "false"
 */

/* Operator precedence (ascending) and associativity
 * Equality (left): ==, !=
 * Comparison (left): >, >=, <, <=
 * Term (left): -, +
 * Factor (left): /, *
 * Unary (right): !, -
 */

struct BinaryExpr;
struct UnaryExpr;
struct IntegerLit;

using Expr = std::variant<BinaryExpr, UnaryExpr, IntegerLit>;
using ExprHandle = std::shared_ptr<Expr>;

struct BinaryExpr {
  TokenType op;
  ExprHandle left;
  ExprHandle right;
};

struct UnaryExpr {
  TokenType op;
  ExprHandle operand;
};

struct IntegerLit {
  int value;
};

struct ParseError {};

class Parser {
private:
  std::vector<Token> toks;
  std::vector<Token>::const_iterator current;

  void Error(std::string_view msg);

  ExprHandle Expression();
  ExprHandle Term();
  ExprHandle Factor();
  ExprHandle Unary();
  ExprHandle Primary();

public:
  Parser(std::vector<Token> toks);

  ExprHandle Parse();
};

struct AstPrinter {
  std::string operator()(const BinaryExpr &expr) {
    return "BinaryExpr " + FormatTokenType(expr.op) + " (" +
           std::visit(AstPrinter{}, *expr.left) + ") (" +
           std::visit(AstPrinter{}, *expr.right) + ")";
  }
  std::string operator()(const UnaryExpr &expr) {
    return "UnaryExpr " + FormatTokenType(expr.op) + " (" +
           std::visit(AstPrinter{}, *expr.operand) + ")";
  }
  std::string operator()(const IntegerLit &lit) {
    return "IntegerLit " + std::to_string(lit.value);
  }
};
