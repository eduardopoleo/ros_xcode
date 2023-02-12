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
// Returns a nil objevt for all these statements
Object *execute(Stmt *stmt, HashTable *env);
Object *visitPuts(Stmt *stmt, HashTable *env);
Object *visitIf(Stmt *stmt, HashTable *env);
Object *visitWhile(Stmt *stmt, HashTable *env);
Object *visitFor(Stmt *stmt, HashTable *env);
Object *visitDef(Stmt *stmt, HashTable *env);

Object *visitVarAssignment(Expr *exp, HashTable *env);
Object *evaluate(Expr *exp, HashTable *env);
Object *visitStringLiteral(Expr *exp);
Object *visitNumberLiteral(Expr *exp);
Object *visitBoolean(Expr *exp);
Object *visitRange(Expr *exp);
Object *visitBinary(Expr *exp, HashTable *env);
Object *visitIdentifierExpression(Expr *exp, HashTable *env);
Object *visitMethodCall(Expr *exp, HashTable *env);

#endif /* interpreter_h */
