//
//  parser.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include "scanner.h"
#include "token.h"

typedef enum ExprType {
    BINARY,
    NUMBER_LITERAL,
    STRING_LITERAL,
    VAR_EXP
} ExprType;

typedef struct Expr {
    ExprType type;
    int line;
    union {
        struct {
            struct Expr *left;
            struct Expr *right;
            TokenType op;
        } binary;

        struct {
            double number;
        } numberLiteral;

        struct {
            char *string;
            int length;
        } stringLiteral;
        
        struct {
            char *string;
            int length;
        } varExp;
    } as;
} Expr;

typedef enum StmtType {
    PUTS_STMT,
    EXPR_STMT,
    VAR_ASSIGNMENT_STMT
} StmtType;

typedef struct Stmt {
  StmtType type;
  int line;
  union {
      struct {
          Expr *exp;
      } puts;
      
      struct {
          int length;
          char *identifier;
      } varAssignment;
  } as;
  Expr *exprStmt;
} Stmt;

typedef struct StmtArray {
  Stmt **list;
  int size;
  int capacity;
} StmtArray;

void initStmtArray(StmtArray *array);
void writeStmtArray(StmtArray *array, Stmt *stmt);
int growStmtCapacity(int capacity);
Stmt **growStmtArray(StmtArray *array, int newCapacity);

StmtArray parse(Scanner *scanner);
Stmt *statement(Scanner *scanner);
Expr *expression(Scanner *scanner);
Expr *comparison(Scanner *scanner);
Expr *term(Scanner *scanner);
Expr *factor(Scanner *scanner);
Expr *primary(Scanner *scanner);
Expr *newExpr(int line, ExprType type);
Expr *newBinary(Expr *left, Expr *right, TokenType op, int line);
Expr *newNumberLiteral(Token *token);
Expr *newStringLiteral(Token *token);
Expr *newVarExpression(Token token);

Stmt *newStmt(int line, StmtType type);
Stmt *newPuts(int line, Expr *exp);
Stmt *newVarAssignment(int line, Token identifier, Expr *exp);


void freeStatements(StmtArray *array);
void freeStatement(Stmt *stmt);
void freeExpression(Expr *exp);

#endif /* parser_h */
