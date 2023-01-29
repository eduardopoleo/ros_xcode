//
//  interpreter.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#ifndef interpreter_h
#define interpreter_h

#include "parser.h"
#include "hash_table.h"
#include "object.h"

void interpret(StmtArray *array, HashTable *env);
void execute(Stmt *stmt, HashTable *env);
void visitPuts(Stmt *stmt, HashTable *env);
void visitIf(Stmt *stmt, HashTable *env);

Object *visitVarAssignment(Expr *exp, HashTable *env);
Object *evaluate(Expr *exp, HashTable *env);
Object *visitStringLiteral(Expr *exp);
Object *visitNumberLiteral(Expr *exp);
Object *visitBoolean(Expr *exp);
Object *visitBinary(Expr *exp, HashTable *env);
Object *visitVarExpression(Expr *exp, HashTable *env);

#endif /* interpreter_h */
