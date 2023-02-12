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

Object *execute(Stmt *stmt, HashTable *env) {
    Object *object = initObject(NIL_OBJECT);

    switch (stmt->type) {
        case PUTS_STMT:
            object = visitPuts(stmt, env);
            break;
        case IF_STMT:
            object =visitIf(stmt, env);
            break;
        case WHILE_STMT:
            object =visitWhile(stmt, env);
            break;
        case FOR_STMT:
            object =visitFor(stmt, env);
            break;
        case DEF_STMT:
            object = visitDef(stmt, env);
            break;
        case EXPR_STMT:
            object = evaluate(stmt->exprStmt, env);
            break;
    }

    return object;
}

Object *visitPuts(Stmt *stmt, HashTable *env) {
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
        case NIL_OBJECT:
            printf("nil\n");
            break;
    }
    
    return initObject(NIL_OBJECT);
}

Object *visitIf(Stmt *stmt, HashTable *env) {
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

    return initObject(NIL_OBJECT);
}

Object *visitWhile(Stmt *stmt, HashTable *env) {
    while (evaluate(stmt->as.whileStmt.condition, env)->as.boolean.value) {
        Stmt *statement;

        for(int i = 0; i < stmt->as.whileStmt.statements->size; i++) {
            statement = stmt->as.whileStmt.statements->list[i];
            execute(statement, env);
        }
    }
    
    return initObject(NIL_OBJECT);
}

Object *visitFor(Stmt *stmt, HashTable *env) {
    Expr *range = stmt->as.forStmt.range;
    double end = strcmp(range->as.range.type, "inclusive") == 0 ? range->as.range.end + 1 : range->as.range.end;
    
    char *name = stmt->as.forStmt.identifier->as.identifierExp.string;
    int length = stmt->as.forStmt.identifier->as.identifierExp.length;
    Object *object = initObject(NUMBER_OBJ);
    Stmt *statement;
    for(int i = range->as.range.start; i < end; i++) {
        object->as.number.value = i;
        insertEntry(env, name, length, object);

        for(int j = 0; j < stmt->as.forStmt.statements->size; j++) {
            statement = stmt->as.forStmt.statements->list[j];
            execute(statement, env);
        }
    }
    
    return initObject(NIL_OBJECT);
}

Object *visitDef(Stmt *stmt, HashTable *env) {
    Object *object = initObject(METHOD_OBJ);
    HashTable *defEnv = initHashTable();

    object->as.method.name = stmt->as.defStmt.name;
    object->as.method.nameLength = stmt->as.defStmt.nameLength;

    object->as.method.arguments = stmt->as.defStmt.arguments;
    object->as.method.statements = stmt->as.defStmt.statements;
    object->as.method.env = defEnv;

    insertEntry(env, object->as.method.name,  object->as.method.nameLength, object);
    
    return initObject(NIL_OBJECT);
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
        case IDENTIFIER_EXP:
            return visitIdentifierExpression(exp, env);
        case METHOD_CALL_EXP:
            return visitMethodCall(exp, env);
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

Object *visitIdentifierExpression(Expr *exp, HashTable *env) {
//    There's probably some work to do here to handle functions
    Object *object;
    object = getEntry(exp->as.identifierExp.string, exp->as.identifierExp.length, env);

    return object;
}

Object *visitMethodCall(Expr *exp, HashTable *env) {
    char *methodName = exp->as.methodCall.name;
    int nameLength = exp->as.methodCall.length;

    Object *methodDefinition = getEntry(methodName, nameLength, env);
    
    ExprArray *values = exp->as.methodCall.arguments;
    ExprArray *arguments = methodDefinition->as.method.arguments;
    
    if (arguments->size != values->size) {
        printf("Arity does not match!");
        exit(1);
    }

    HashTable *methodEnv = methodDefinition->as.method.env;
     
    for(int i = 0; i < methodDefinition->as.method.arguments->size; i++) {
        char *name = arguments->list[i]->as.methodCall.name;
        int length = arguments->list[i]->as.methodCall.length;
        Object *value = evaluate(values->list[i], env);
        insertEntry(methodEnv, name, length, value);
    }
    
    StmtArray *statements = methodDefinition->as.method.statements;
    
    if (statements->size == 0) {
        return initObject(NIL_OBJECT);
    }
    
    Object *result;
    for (int i = 0; i < statements->size; i++) {
        result = execute(statements->list[i], methodEnv);
    }

    return result;
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
