//
//  main.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"
#include "file.h"
#include "hash_table.h"

/*
  Feature list:
  Stage 1:
  - [x] Multi digit numbers
  - [x] Floating Point support
  - [x] Multiplication
  - [x] Division
  - [x] Module
  - [x] ReadFile
  - [x] Blank spaces
  - [x] Multi line statement
  - [x] Comments
  - [x] Strings
  - [x] Multiple statements
  - [x] Free objects: ast, dynamic arrays
  Stage 2:
  - [x] puts
  - [x] variables
  - [x] boolean related binary operation
    - ">" "<" ">=" "<=" "==" "!="
  - control flow (if, for, while)
  - functions
  - closures (blocks)
  Stage 3:
  - arrays
  - hashes
  - classes (optional)
*/
int main(int argc, char *argv[]) {
    // Prep
    char *buffer = readFile(argv[1]);
    Scanner scanner;
    
    // Interpret program
    initScanner(&scanner, buffer);
    StmtArray *statements = parse(&scanner);
    HashTable *globalEnv = initHashTable();
    interpret(statements, globalEnv);

    // Free all objects
    // Todo need to free the hash tables and its related
    // value objects
    freeStatements(statements);
    free(statements->list);
    free(buffer);

    return 0;
}
