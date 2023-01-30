//
//  scanner.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#ifndef scanner_h
#define scanner_h

#include <stdio.h>
#include "token.h"
#include <stdbool.h>

typedef struct Scanner {
    char *start;
    char *current;
    Token peek;
    Token peek_prev;
    Token peek_next;
    int line;
} Scanner;

typedef struct Keyword {
  char *name;
  int length;
  TokenType type;
} Keyword;

static Keyword keywords[] = {
    {"puts", 4, PUTS},
    {"true", 4, TRUE_TOK},
    {"false", 5, FALSE_TOK},
    {"if", 2, IF},
    {"elsif", 5, ELSIF},
    {"else", 4, ELSE},
    {"end", 3, END},
    // sentinel
    {NULL, 0, END_OF_FILE}
};

void initScanner(Scanner *scanner, char *code);
Token newToken(TokenType type, int line, int length, char *lexeme);
Token initToken(Scanner *scanner);
bool isWhiteSpace(char c);
bool isNewLine(char c);
Token advanceToken(Scanner *scanner);
Token rewindToken(Scanner *scanner);
Token calculateToken(Scanner *scanner);
bool atEnd(Scanner *scanner);
bool match(Scanner *scanner, TokenType type);
bool isNumber(char c);
bool isAlpha(char c);
bool isAllowedIdentifier(char c);
void captureFullIdentifier(Scanner *scanner);
void captureFullNumber(Scanner *scanner);
void captureFullString(Scanner *scanner);
void printIdentifier(char *identifier, int lenght);

#endif /* scanner_h */
