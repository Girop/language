#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum {
    FUNC_TOKEN, // fnc
    LOCAL_VAR_TOKEN, // let 
    GLOBAL_VAR_TOKEN, // glob
    PRINT_TOKEN, // print
    NIL_TOKEN, // nil

    PLUS_TOKEN, // +
    MINUS_TOKEN, // -
    SLASH_TOKEN, // /
    STAR_TOKEN, // *
    PERCENT_TOKEN, // %
    LEFT_PAREN, // (
    RIGHT_PAREN, // )
    LEFT_BRACE, // {
    RIGHT_BRACE, // }

    BANG_TOKEN, // !     
    QUESTION_TOKEN, // ?
    SEMICOLON_TOKEN, // ;
    COMMA_TOKEN, // , 
                 
    LESS_TOKEN, // <
    LESS_EQUAL_TOKEN, // <=
    MORE_TOKEN, // >
    MORE_EQUAL_TOKEN, // >=
    EQUAL_TOKEN, // =
    DOUBLE_EQUAL_TOKEN, // ==
    BANG_EQUAL_TOKEN, // !=

    CONCAT_TOKEN, // ..
    OR_TOKEN, // ||
    AND_TOKEN, // &&

    NUMBER_TOKEN,
    IDENTIFIER_TOKEN,
    STRING_TOKEN,
    TRUE_TOKEN,
    FALSE_TOKEN,

    INVALID_TOKEN,
    EOF_TOKEN,
} TokenType;


typedef struct {
    union {
        struct {
            unsigned int len;
            const char* position;
        } string;
        double number;
        const char* error_msg;
    };

    TokenType type; 
    unsigned int line;
} Token;

void init_tokenizer(const char* source);
Token get_token(void);

#endif
