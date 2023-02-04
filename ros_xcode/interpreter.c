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
        case IF_STMT:
            visitIf(stmt, env);
            break;
        case WHILE_STMT:
            visitWhile(stmt, env);
            break;
        case FOR_STMT:
            visitFor(stmt, env);
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
        case BOOLEAN_OBJ:
            if (object->as.boolean.value == 1) {
                printf("true\n");
            } else {
                printf("false\n");
            }
            break;
    }
}

void visitIf(Stmt *stmt, HashTable *env) {
    int count = stmt->as.ifStmt.conditionals->size;
    
    for(int i = 0; i < count; i++) {
        Conditional *conditional = stmt->as.ifStmt.conditionals->list[i];
        Object *conditionMet = evaluate(conditional->condition, env);
        
        if(conditionMet->as.boolean.value == true) {
            int statementCount = conditional->statements->size;
            for(int i = 0; i < statementCount; i++) {
                execute(conditional->statements->list[i], env);
            }
            break;
        }
    }
}

void visitWhile(Stmt *stmt, HashTable *env) {
    while (evaluate(stmt->as.whileStmt.condition, env)->as.boolean.value) {
        Stmt *statement;

        for(int i = 0; i < stmt->as.whileStmt.statements->size; i++) {
            statement = stmt->as.whileStmt.statements->list[i];
            execute(statement, env);
        }
    }
}

void visitFor(Stmt *stmt, HashTable *env) {
    /*
        Evaluate range -> []
        
     */
}

Object *visitVarAssignment(Expr *exp, HashTable *env) {
    Object *object = evaluate(exp->as.varAssignment.value, env);
    insertEntry(env, exp->as.varAssignment.name, exp->as.varAssignment.length, object);
    return object;
}

Object *evaluate(Expr *exp, HashTable *env) {
    switch (exp->type) {
        case BINARY:
            return visitBinary(exp, env);
        case NUMBER_LITERAL:
            return visitNumberLiteral(exp);
        case STRING_LITERAL:
            return visitStringLiteral(exp);
        case BOOLEAN:
            return visitBoolean(exp);
        case RANGE:
            return visitRange(exp);
        case VAR_EXP:
            return visitVarExpression(exp, env);
        case VAR_ASSIGNMENT:
            return visitVarAssignment(exp, env);
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

Object *visitBoolean(Expr *exp) {
    Object *object = initObject(BOOLEAN_OBJ);
    object->as.boolean.value = exp->as.boolExp.value;
    return object;
}

Object *visitRange(Expr *exp) {
    Object *object = initObject(RANGE_OBJ);
    object->as.range.type   = exp->as.range.type;
    object->as.range.start  = exp->as.range.start;
    object->as.range.end  = exp->as.range.end;
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
        case GREATER:
            object->type = BOOLEAN_OBJ;
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value > evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case GREATER_EQUAL:
            object->type = BOOLEAN_OBJ;            
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value >= evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case LESS:
            object->type = BOOLEAN_OBJ;
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value < evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case LESS_EQUAL:
            object->type = BOOLEAN_OBJ;
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value <= evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case EQUAL_EQUAL:
            object->type = BOOLEAN_OBJ;
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value == evaluate(exp->as.binary.right, env)->as.number.value;
            break;
        case BANG_EQUAL:
            object->type = BOOLEAN_OBJ;
            object->as.boolean.value = evaluate(exp->as.binary.left, env)->as.number.value != evaluate(exp->as.binary.right, env)->as.number.value;
            break;
    }
    return object;
}
