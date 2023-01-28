//
//  scanner.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "token.h"
#include <stdbool.h>

void initScanner(Scanner *scanner, char *code) {
    scanner->start = code;
    scanner->current = code;
    scanner->line = 1;
    Token token = initToken(scanner);
    scanner->peek = token;
}

Token newToken(TokenType type, int line, int length, char *lexeme) {
    Token token;
    token.type = type;
    token.line = line;
    token.length = length;
    token.lexeme = lexeme;

    return token;
}

Token initToken(Scanner *scanner) {
    Token token = calculateToken(scanner);
    scanner->peek = token;
    return token;
}

// Advances the token but returns the previously current token
Token advanceToken(Scanner *scanner) {
    Token prev = scanner->peek;
    scanner->peek = calculateToken(scanner);
    scanner->peek_prev = prev;
    return prev;
}

Token rewindToken(Scanner *scanner) {
    scanner->start = scanner->peek.lexeme;
    scanner->peek = scanner->peek_prev;
    
    return scanner->peek;
}

bool atEnd(Scanner *scanner) {
    if (scanner->start[0] == EOF || scanner->start[0] == '\0') {
        return true;
    }

    return false;
}

Token calculateToken(Scanner *scanner) {
    // Handles white spaces,
    while(isWhiteSpace(scanner->start[0])) {
        scanner->start++;
    }

    while(isNewLine(scanner->start[0])) {
        scanner->line++;
        scanner->start++;
    }

    // handles single line comments
    if(scanner->start[0] == '#') {
        while(scanner->start[0] != '\n' && !atEnd(scanner)) {
            scanner->start++;
        }
        scanner->line++;
        /*
          After we're out of a comments we need to re check all the other
          previous conditions again. This is not efficient because we're building
          a recursive stack but whatever.
        */
        return calculateToken(scanner);
    }

    scanner->current = scanner->start + 1;
    Token token;
    switch (scanner->start[0]) {
        case '+':
            token = newToken(PLUS, scanner->line, 1, scanner->start);
            break;
        case '-':
            token = newToken(MINUS, scanner->line, 1, scanner->start);
            break;
        case '*':
            token = newToken(STAR, scanner->line, 1, scanner->start);
            break;
        case '/':
            token = newToken(FORWARD_SLASH, scanner->line, 1, scanner->start);
            break;
        case '%':
            token = newToken(MODULO, scanner->line, 1, scanner->start);
            break;
        case '=':
            if (scanner->current[0] == '=') {
                token = newToken(EQUAL_EQUAL, scanner->line, 1, scanner->start);
                /*
                    account for the extra equal so that we do not parse it next time around
                */
                scanner->current++;
            } else {
                token = newToken(EQUAL, scanner->line, 1, scanner->start);
            }
            break;
        case '!':
            if (scanner->current[0] == '=') {
                token = newToken(BANG_EQUAL, scanner->line, 1, scanner->start);
                scanner->current++;
            } else {
                token = newToken(BANG, scanner->line, 1, scanner->start);
            }
            break;
        case '>':
            if (scanner->current[0] == '=') {
                token = newToken(GREATER_EQUAL, scanner->line, 1, scanner->start);
                scanner->current++;
            } else {
                token = newToken(GREATER, scanner->line, 1, scanner->start);
            }
            break;
        case '<':
            if (scanner->current[0] == '=') {
                token = newToken(LESS_EQUAL, scanner->line, 1, scanner->start);
                scanner->current++;
            } else {
                token = newToken(LESS, scanner->line, 1, scanner->start);
            }
            break;
        case '\0':
            token = newToken(END_OF_FILE, scanner->line, 1, scanner->start);
            break;
    }

    if (scanner->start[0] == '"') {
        captureFullString(scanner);
        int length = (int)(scanner->current - scanner->start);
        token = newToken(STRING, scanner->line, length, scanner->start);
    }

    if (isAlpha(scanner->start[0])) {
        captureFullIdentifier(scanner);
        int length = (int)(scanner->current - scanner->start);
        // set a pointer to the beginning of the array and name it keyword
        // add 1 ++ till the end cuz we have the sentinel.
        bool isKeyword = false;
        for(Keyword *keyword = keywords; keyword->name != NULL; keyword++) {
            if ((keyword->length == length) && memcmp(scanner->start, keyword->name, length) == 0) {
                isKeyword = true;
                token = newToken(keyword->type, scanner->line, length, scanner->start);
            }
        }
        
        if (!isKeyword) {
            token = newToken(IDENTIFIER, scanner->line, length, scanner->start);
        }
    }

    if(isNumber(scanner->start[0])) {
        captureFullNumber(scanner);
        int length = (int)(scanner->current - scanner->start);
        token = newToken(NUMBER, scanner->line, length, scanner->start);
    }

    if (!atEnd(scanner)) {
        scanner->start = scanner->current;
    }
    return token;
}

void captureFullIdentifier(Scanner *scanner) {
    while(isAllowedIdentifier(scanner->current[0])) {
        scanner->current++;
    }
}

void captureFullNumber(Scanner *scanner) {
    // Current could be a . if we have 1 digit e.g 1.5
    while(isNumber(scanner->current[0]) || scanner->current[0] == '.') {
        scanner->current++;
        // If we land into a . we can to keep iterating to gather all the decimal points
        if(scanner->current[0] == '.') {
            scanner->current++;
        }
  }
}

void captureFullString(Scanner *scanner) {
    while(scanner->current[0] != '"') {
        scanner->current++;
    }
    // Moves past the "
    scanner->current++;
}

bool isNewLine(char c) {
    if(c == '\n') {
        return true;
    }

    return false;
}

bool isWhiteSpace(char c) {
    if(c == ' ' || c == '\t' || c == '\r') {
        return true;
    }

    return false;
}

bool isAllowedIdentifier(char c) {
    return isAlpha(c) || isNumber(c) || c == '_';
}

bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool match(Scanner *scanner, TokenType type) {
    if (scanner->peek.type == type) {
        advanceToken(scanner);
        return true;
    }

    return false;
}

void printIdentifier(char *identifier, int lenght) {
    for(int i = 0; i < lenght; i++) {
        printf("%c", identifier[i]);
    }
    printf("\n");
}
