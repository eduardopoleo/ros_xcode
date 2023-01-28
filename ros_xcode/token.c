//
//  token.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include "token.h"

void printToken(Token *token) {
  printf("Type: %d, Line: %d, ", token->type, token->line);
  printf("Literal: %.*s\n", token->length, token->lexeme);
}
