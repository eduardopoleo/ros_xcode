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
#include "array.h"

typedef enum ExprType {
    BINARY,
    NUMBER_LITERAL,
    STRING_LITERAL,
    BOOLEAN,
    VAR_EXP,
    VAR_ASSIGNMENT,
    RANGE
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

        struct {
            bool value;
        } boolExp;

        struct {
            char *type;
            double start;
            double end;
        } range;

        struct {
            char *name;
            int length;
            struct Expr *value;
        } varAssignment;
    } as;
} Expr;

typedef enum StmtType {
    PUTS_STMT,
    EXPR_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    DEF_STMT
} StmtType;


typedef struct Stmt {
    StmtType type;
    int line;
    union {
        struct {
            Expr *exp;
        } puts;
      
        struct {
            struct ConditionalArray *conditionals;
        } ifStmt;
        
        struct {
            Expr *condition;
            struct StmtArray *statements;
        } whileStmt;
        
        struct {
            Expr *identifier;
            Expr *range;
            struct StmtArray *statements;
        } forStmt;

        struct {
            Expr *name;
            struct ExprArray *arguments;
            struct StmtArray *statements;
        } defStmt;

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
}   StmtArray;

typedef struct ExprArray {
    Expr **list;
    int size;
    int capacity;
} ExprArray;

typedef struct Conditional {
    Expr *condition;
    struct StmtArray *statements;
} Conditional;

typedef struct ConditionalArray {
    Conditional **list;
    int size;
    int capacity;
}   ConditionalArray;


StmtArray *initStmtArray(void);
ExprArray *initExprArray(void);
ConditionalArray *initConditionalArray(void);

StmtArray *parse(Scanner *scanner);
Stmt *statement(Scanner *scanner);
Stmt *parsePuts(Scanner *scanner);
Stmt *parseIf(Scanner *scanner);
Stmt *parseWhile(Scanner *scanner);
Stmt *newStmt(int line, StmtType type);
Stmt *parseFor(Scanner *scanner);
Stmt *parseDef(Scanner *scanner);

Expr *expression(Scanner *scanner);
Expr *assignment(Scanner *scanner);
Expr *equality(Scanner *scanner);
Expr *comparison(Scanner *scanner);
Expr *term(Scanner *scanner);
Expr *factor(Scanner *scanner);
Expr *primary(Scanner *scanner);
Expr *newExpr(int line, ExprType type);

Conditional *newConditional(void);
Expr *newBinary(Expr *left, Expr *right, TokenType op, int line);
Expr *newBooleanExpr(Token token, bool value);
Expr *newNumberLiteral(Token *token);
Expr *newStringLiteral(Token *token);
Expr *newVarExpression(Token token);
Expr *newVarAssignment(int line, Expr *identifier, Expr *value);
Expr *newRangeExpression(Token token);

void freeStatements(StmtArray *array);
void freeStatement(Stmt *stmt);
void freeExpression(Expr *exp);

#endif /* parser_h */
