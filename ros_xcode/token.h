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
    PLUS, // 0
    MINUS, // 1
    NUMBER, // 2
    STRING, // 3
    STAR, // 4
    FORWARD_SLASH, // 5
    MODULO, // 6
    END_OF_FILE, // 7
    PUTS, //8
    IDENTIFIER, // 9
    EQUAL, //10
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
