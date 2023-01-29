//
//  parser.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

StmtArray *parse(Scanner *scanner) {
    StmtArray *array = initStmtArray();

    Stmt *stmt;
    while(!atEnd(scanner)) {
        stmt = statement(scanner);
        writeStmtArray(array, stmt);
    }

    return array;
}

Stmt *statement(Scanner *scanner) {
    if(match(scanner, PUTS)) {
        Expr *exp = expression(scanner);
        // change this to parsePuts
        return newPuts(scanner->line, exp);
    }
    
    if (match(scanner, IF)) {
        return parseIf(scanner);
    }

    Stmt *stmt = newStmt(scanner->line, EXPR_STMT);
    stmt->exprStmt = expression(scanner);
    return stmt;
}

Stmt *parseIf(Scanner *scanner) {
    Expr *condition = expression(scanner);
    StmtArray *ifStmts = initStmtArray();
    
    Stmt *stmt;
    while(!match(scanner, END)) {
        stmt = statement(scanner);
        writeStmtArray(ifStmts, stmt);
    }

    Stmt *ifStmt = newStmt(scanner->line, IF_STMT);
    ifStmt->as.ifStmt.condition = condition;
    ifStmt->as.ifStmt.ifStmts = ifStmts;

    return ifStmt;
}

Expr *expression(Scanner *scanner) {
    return assignment(scanner);
}

// assignment -> IDENTIFIER = expression
Expr *assignment(Scanner *scanner) {
    Expr *identifier = equality(scanner);

    if (match(scanner, EQUAL)) {
        // We probably need some validation here to ensure that we
        // do not have some weird L value. It should just be identifier
        Expr *value = assignment(scanner);
        return newVarAssignment(scanner->line, identifier, value);
    }

    return identifier;
}

// equality -> comparison((== | !=) comparison)*
Expr *equality(Scanner *scanner) {
    Expr *exp = comparison(scanner);
    while(match(scanner, EQUAL_EQUAL) ||
        match(scanner, BANG_EQUAL))
    {
      TokenType op = scanner->peek_prev.type;
      exp = newBinary(exp, comparison(scanner), op, scanner->line);
    }
    return exp;
}

// comparison -> term((> | < | >= | <=) term)*
Expr *comparison(Scanner *scanner) {
   Expr *exp = term(scanner);
   while(match(scanner, GREATER) ||
        match(scanner, GREATER_EQUAL) ||
        match(scanner, LESS) ||
        match(scanner, LESS_EQUAL))
   {
      TokenType op = scanner->peek_prev.type;
      exp = newBinary(exp, term(scanner), op, scanner->line);
   }
   return exp;
}

// term -> factor ((+|-) factor)*
Expr *term(Scanner *scanner) {
    Expr *exp = factor(scanner);
   
    while(match(scanner, PLUS) || match(scanner, MINUS)) {
        TokenType op = scanner->peek_prev.type;
        exp = newBinary(exp, factor(scanner), op, scanner->line);
    }
    return exp;
}

// factor -> primary ((*|/|%) primary)*
Expr *factor(Scanner *scanner) {
    Expr *exp = primary(scanner);
    
    while(match(scanner, STAR) || match(scanner, FORWARD_SLASH) || match(scanner, MODULO)) {
        TokenType op = scanner->peek_prev.type;
        exp = newBinary(exp, primary(scanner), op, scanner->line);
    }

    return exp;
}

// primary -> STRING | NUMBER | IDENTIFIER
Expr *primary(Scanner *scanner) {
    Token token = advanceToken(scanner);
    Expr *exp;
    switch (token.type){
        case STRING:
            exp = newStringLiteral(&token);
            break;
        case NUMBER:
            exp = newNumberLiteral(&token);
            break;
        case TRUE_TOK:
            exp = newBooleanExpr(token, true);
            break;
        case FALSE_TOK:
            exp = newBooleanExpr(token, false);
            break;
        case IDENTIFIER:
            exp = newVarExpression(token);
    }

  return exp;
}

// Supporting functions: Should go into another file
Stmt *newPuts(int line, Expr *exp) {
    Stmt *stmt = newStmt(line, PUTS_STMT);
    stmt->as.puts.exp = exp;
    return stmt;
}

Expr *newVarAssignment(int line, Expr *identifier, Expr *value) {
    Expr *exp = newExpr(line, VAR_ASSIGNMENT);
    exp->as.varAssignment.name = identifier->as.varExp.string;
    exp->as.varAssignment.length = identifier->as.varExp.length;
    exp->as.varAssignment.value = value;
    return exp;
}

Expr *newBooleanExpr(Token token, bool value) {
    Expr *exp = newExpr(token.line, BOOLEAN);
    exp->as.boolExp.value = value;
    return exp;
}

Expr *newVarExpression(Token token) {
    Expr *exp = newExpr(token.line, VAR_EXP);
    exp->as.varExp.length = token.length;
    exp->as.varExp.string = token.lexeme;
    
    return exp;
}

Expr *newBinary(Expr *left, Expr *right, TokenType op, int line) {
    Expr *exp = newExpr(line, BINARY);
    exp->as.binary.left = left;
    exp->as.binary.right = right;
    exp->as.binary.op = op;
    return exp;
}

Expr *newNumberLiteral(Token *token) {
    Expr *exp = newExpr(token->line, NUMBER_LITERAL);
    double number = strtod(token->lexeme, NULL);
    exp->as.numberLiteral.number = number;
    return exp;
}

Expr *newStringLiteral(Token *token) {
    Expr *exp = newExpr(token->line, STRING_LITERAL);
    exp->as.stringLiteral.string = token->lexeme;
    exp->as.stringLiteral.length = token->length;
    return exp;
}

Stmt *newStmt(int line, StmtType type) {
    Stmt *stmt = (Stmt*)malloc(sizeof(*stmt));
    stmt->line = line;
    stmt->type = type;
    return stmt;
}

Expr *newExpr(int line, ExprType type) {
    Expr *exp = (Expr*)malloc(sizeof(*exp));
    exp->line = line;
    exp->type = type;
    return exp;
}

void freeStatements(StmtArray *array) {
    for(int i = 0; i < array->size; i++) {
       freeStatement(array->list[i]);
    }
}

void freeStatement(Stmt *stmt) {
    switch (stmt->type) {
        case PUTS_STMT:
            freeExpression(stmt->as.puts.exp);
            free(stmt);
            break;
        case EXPR_STMT:
            freeExpression(stmt->exprStmt);
            break;
  }
}

void freeExpression(Expr *exp) {
    switch (exp->type) {
        case BINARY:
            freeExpression(exp->as.binary.left);
            freeExpression(exp->as.binary.right);
            break;
        case NUMBER_LITERAL:
            free(exp);
            break;
        case STRING_LITERAL:
            free(exp);
            break;
  }
}

////////////// STMT ARRAY ////////////////////
StmtArray *initStmtArray(void) {
    StmtArray *array = malloc(sizeof(StmtArray));

    array->list = NULL;
    array->capacity = 0;
    array->size = 0;

    return array;
}

void writeStmtArray(StmtArray *array, Stmt *stmt) {
    if (array->size + 1 > array->capacity) {
        int newCapacity = growStmtCapacity(array->capacity);
        array->list = growStmtArray(array, newCapacity);
        array->capacity = newCapacity;
    }

    array->list[array->size] = stmt;
    array->size++;
}

int growStmtCapacity(int capacity) {
    if (capacity < 8) {
        return 8;
    } else {
        return 2 * capacity;
    }
}

Stmt **growStmtArray(StmtArray *array, int newCapacity) {
    return (Stmt**)realloc(array->list, newCapacity * sizeof(Stmt));
}
//////////////////////////////////////////////
