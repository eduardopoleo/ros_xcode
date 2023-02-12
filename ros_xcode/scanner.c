//
//  scanner.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include <stdbool.h>
#include <ctype.h>

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
//        printf("white space skipped\n");
        scanner->start++;
    }

    while(isNewLine(scanner->start[0])) {
//        printf("new line skipped skipped\n");
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
    Token token = newToken(EMPTY_TOKEN, scanner->line, 1, scanner->start);
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
        case '(':
            token = newToken(LEFT_PAREN, scanner->line, 1, scanner->start);
            break;
        case ')':
            token = newToken(RIGHT_PAREN, scanner->line, 1, scanner->start);
            break;
        case ',':
            token = newToken(COMMA, scanner->line, 1, scanner->start);
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
        token = handleNumberOrRange(scanner);
    }

    if (!atEnd(scanner)) {
        scanner->start = scanner->current;
    }
    
    if (token.type == EMPTY_TOKEN) {
        printf("exiting character %c not recognized!\n", scanner->start[0]);
        exit(1);
    }
    return token;
}

void captureFullIdentifier(Scanner *scanner) {
    while(isAllowedIdentifier(scanner->current[0])) {
        scanner->current++;
    }
}

void captureFullString(Scanner *scanner) {
    while(scanner->current[0] != '"') {
        scanner->current++;
    }
    // Moves past the "
    scanner->current++;
}

Token handleNumberOrRange(Scanner *scanner) {
    int length;
    
    while(isNumber(scanner->current[0])) {
        scanner->current++;
    }

//      We have a ruby range.
    if (scanner->current[0] == '.' && scanner->current[1]  == '.') {
        scanner->current++;
        scanner->current++;
        TokenType type = INCLUSIVE_RANGE;

        if (scanner->current[0] == '.') {
            scanner->current++;
            type = EXCLUSIVE_RANGE;
        }
        
//        We need to resolve the second part of the range
        while(isNumber(scanner->current[0])) {
            scanner->current++;
        }

        length = (int)(scanner->current - scanner->start);
        return newToken(type, scanner->line, length, scanner->start);
    }

    if (scanner->current[0] == '.') {
        scanner->current++;
//          we resolve the decimal part of the number
        while(isNumber(scanner->current[0])) {
            scanner->current++;
        }
    }

    length = (int)(scanner->current - scanner->start);
    return newToken(NUMBER, scanner->line, length, scanner->start);
}

bool isNewLine(char c) {
    if(c == '\n' ) {
        return true;
    }

    return false;
}

bool isWhiteSpace(char c) {
    if(isspace(c)) {
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

Token consume(Scanner *scanner, TokenType type) {
    if (scanner->peek.type == type) {
        advanceToken(scanner);
        return scanner->peek_prev;
    }

    printf("Expected token of type %d not found", type);
    exit(1);
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
