//
//  token.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#ifndef token_h
#define token_h

#include <stdio.h>

typedef enum TokenType {
    PLUS,
    MINUS,
    NUMBER,
    STRING,
    STAR,
    FORWARD_SLASH,
    MODULO,
    END_OF_FILE,
    PUTS,
    IDENTIFIER,
    EQUAL,
    EQUAL_EQUAL,
    TRUE_TOK,
    FALSE_TOK,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    BANG,
    BANG_EQUAL,
    IF,
    ELSE,
    END,
    NULL_TOKEN
} TokenType;

typedef struct Token {
  TokenType type;
  char *lexeme;
  int length;
  int line;
} Token;

void printToken(Token *token);

#endif /* token_h */
