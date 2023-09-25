#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tokenizer.h"

typedef struct {
    const char* text;
    const char* start;
    const char* current;
    int line;
} Tokenizer;

Tokenizer tokenizer;


void init_tokenizer(const char *source) {
    tokenizer.line = 1;
    tokenizer.text = source;
    tokenizer.current = source;
    tokenizer.start = source;
}

static char consume(void) {
    return *(tokenizer.current++);
}

static char look(void) {
    return *(tokenizer.current);
}

bool is_end(void) {
    return look() == '\0';
}

static bool is_digit(char chr) {
    return chr >= '0' && chr <= '9';
}

static void skip_comment(void) {
    while (!is_end() && look() != '\n') {
        tokenizer.line++;
        consume(); 
    }
}

#define COMMENT_CHAR '#'
static void skip_whitespace(void) {
    for (;;) {
        char current_char = look();    
        switch (current_char) {
            case '\n':
                tokenizer.line++;
            case ' ':
            case '\t':
            case '\r':
                consume();
                break;
            case COMMENT_CHAR:
                skip_comment();
                break;
            default:
                return;
        }
    }
}

static Token create_token(TokenType type) {
    Token res = {
        .type = type,
        .line = tokenizer.line,
    };
    return res;
}

static bool match_token(char chr) {
    if (is_end()) return false; 
    if (look() == chr) {
        consume();
        return true;
    }
    return false;
}

static Token create_error(char* msg) {
    Token res = create_token(INVALID_TOKEN);
    res.error_msg = msg;    
    return res;
}

static Token parse_string(void) {
    for (;;) {
        char next = look(); 

        if (next == '\"' || is_end()) {
            break;
        }
        if (next == '\n') tokenizer.line++;
        consume();
    }

    if (is_end()) return create_error("Undelimited string"); 
    consume();

    Token res = create_token(STRING_TOKEN);
    res.string.len = (unsigned int) (tokenizer.current - tokenizer.start - 2); // Adjusting for "
    res.string.position = tokenizer.start + 1;
    return res;
}


static const char* KEYWORDS[] = {
    [FUNC_TOKEN] = "fnc",
    [LOCAL_VARIABLE] = "let",
    [GLOBAL_VARIABLE] = "glob",
};

static Token parse_identifier(void) {
    for (;;) {
        char chr = look(); 
        if (isalpha(chr) || is_digit(chr)) {
            consume();
        } else {
            break;
        }
    }

    int len = tokenizer.current - tokenizer.start;
    for (int i = 0; i < (sizeof(KEYWORDS) / sizeof(char*)); i++) {
        if (strncmp(tokenizer.start, KEYWORDS[i], len) == 0) {
            return create_token((TokenType) i); 
        }
    }

    Token res = create_token(IDENTIFIER_TOKEN);
    res.string.position = tokenizer.start;
    res.string.len = len;
    return res;
}


static Token parse_number(void) {
    while (is_digit(look())) consume();

    if (look() == '.') {
        consume();
        while (is_digit(look())) consume();
    }
    
    Token res = create_token(NUMBER_TOKEN);
    res.number = strtod(tokenizer.start, NULL);
    return res;
}

Token get_token(void) {
    skip_whitespace(); 
    tokenizer.start = tokenizer.current;
    char next_char;

    if(is_end()) {
        return create_token(EOF_TOKEN);
    }

    next_char = consume(); 
    if (is_digit(next_char)) return parse_number();
    if (isalpha(next_char)) return parse_identifier();

    switch (next_char) {
        case '+': return create_token(PLUS_TOKEN);
        case '-': return create_token(MINUS_TOKEN);
        case '/': return create_token(SLASH_TOKEN);
        case '*': return create_token(STAR_TOKEN);
        case '(': return create_token(LEFT_PAREN);
        case ')': return create_token(RIGHT_PAREN);
        case '{': return create_token(LEFT_BRACE);
        case '}': return create_token(RIGHT_BRACE);

        case '?': return create_token(QUESTION_TOKEN);
        case ';': return create_token(SEMICOLON_TOKEN);
        case ',': return create_token(COMMA_TOKEN);

        case '!': return create_token(match_token('=') ? BANG_EQUAL_TOKEN : BANG_TOKEN);
        case '<': return create_token(match_token('=') ? LESS_TOKEN : LESS_EQUAL_TOKEN);
        case '>': return create_token(match_token('=') ? MORE_TOKEN : MORE_EQUAL_TOKEN);
        case '=': return create_token(match_token('=') ? EQUAL_TOKEN : DOUBLE_EQUAL_TOKEN);
        case '&': return match_token('&') ? create_token(AND_TOKEN) : create_error("Expected second '&'");
        case '|': return match_token('|') ? create_token(AND_TOKEN) : create_error("Expected second '|'");
        case '.': return match_token('.') ? create_token(CONCAT_TOKEN) : create_error("Unexpected .");
        case '\"': return parse_string();

        default: return create_error("Unexpected symbol");
    }
}
