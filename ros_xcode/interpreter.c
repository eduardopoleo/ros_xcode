//
//  interpreter.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "token.h"
#include "parser.h"

void interpret(StmtArray *array, HashTable *env) {
    for(int i = 0; i < array->size; i++) {
        execute(array->list[i], env);
    }
}

void execute(Stmt *stmt, HashTable *env) {
    switch (stmt->type) {
        case PUTS_STMT:
            visitPuts(stmt, env);
            break;
        case VAR_ASSIGNMENT_STMT:
            visitVarAssignment(stmt, env);
            break;
        case EXPR_STMT:
            evaluate(stmt->exprStmt, env);
            break;
    }
}

void visitPuts(Stmt *stmt, HashTable *env) {
    Object *object = evaluate(stmt->as.puts.exp, env);
    switch (object->type) {
        case NUMBER_OBJ:
            printf("%f\n", object->as.number.value);
            break;
        case STRING_OBJ:
            for(int i = 1; i < object->as.string.length - 1; i++) {
                printf("%c", object->as.string.value[i]);
            }
            printf("\n");
            break;
    }
}

void visitVarAssignment(Stmt *stmt, HashTable *env) {
    Object *object = evaluate(stmt->exprStmt, env);
    printf(
           "key %c, length: %d, value %f\n, object p %p\nAyu[dfgh[",
           stmt->as.varAssignment.identifier[0],
           stmt->as.varAssignment.length,
           object->as.number.value,
           &object
    );
    insertEntry(env, stmt->as.varAssignment.identifier, stmt->as.varAssignment.length, object);
//    printTable(env);
}

Object *evaluate(Expr *exp, HashTable *env) {
    switch (exp->type) {
        case BINARY:
            return visitBinary(exp, env);
        case NUMBER_LITERAL:
            return visitNumberLiteral(exp);
        case STRING_LITERAL:
            return visitStringLiteral(exp);
        case VAR_EXP:
            return visitVarExpression(exp, env);
  }
}

Object *visitStringLiteral(Expr *exp) {
    Object *object = initObject(STRING_OBJ);
    object->as.string.value = exp->as.stringLiteral.string;
    object->as.string.length = exp->as.stringLiteral.length;
    return object;
}

Object *visitNumberLiteral(Expr *exp) {
    Object *object = initObject(NUMBER_OBJ);
    object->as.number.value = exp->as.numberLiteral.number;
    return object;
}

Object *visitVarExpression(Expr *exp, HashTable *env) {
    return getEntry(exp->as.varExp.string, exp->as.varExp.length, env);
}

Object *visitBinary(Expr *exp, HashTable *env) {
    Object *object = initObject(NUMBER_OBJ);

    switch (exp->as.binary.op) {
        case PLUS:
            object->as.number.value = evaluate(exp->as.binary.left, env)->as.number.value + evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case MINUS:
            object->as.number.value = evaluate(exp->as.binary.left, env)->as.number.value - evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case STAR:
            object->as.number.value = evaluate(exp->as.binary.left, env)->as.number.value * evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case FORWARD_SLASH:
            object->as.number.value = evaluate(exp->as.binary.left, env)->as.number.value / evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case MODULO:
            object->as.number.value = (int)evaluate(exp->as.binary.left, env)->as.number.value % (int)evaluate(exp->as.binary.right, env)->as.number.value;
            break;
    }
    return object;
}
