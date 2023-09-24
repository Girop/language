#include "virtual_machine.h"
#include "tokenizer.h"
#include "parser.h"
#include "chunk.h"

typedef struct {
    Chunk* chunk;
    Token current;
    bool had_error;
} Parser;


static Parser parser;
#define emit_code(instruction) write_instruction(parser.chunk, instruction)

typedef struct {
    uint8_t left_bp;
    uint8_t right_bp;
} ParseRule;

ParseRule parse_table[] = {
    [PLUS_TOKEN] = {1, 2},
    [MINUS_TOKEN] = {1, 2},
    [SLASH_TOKEN] = {3, 4},
    [STAR_TOKEN] = {3, 4},
    [DOUBLE_EQUAL_TOKEN] = {5, 6},
    [BANG_EQUAL_TOKEN] = {5, 6},
    [EOF_TOKEN] = {0, 0}
};

static Token look(void) {
    return parser.current;
}

static Token consume(void) {
    Token res = parser.current;
    parser.current = get_token();
    return res;
}

static void parse_expr(uint8_t min_bp);

static void parse_number(Token token) {
    uint8_t addr = add_constant(parser.chunk, token.number);
    if (addr >= UINT8_MAX) {
        printf("Too many constants!\n");
        exit(-1);
    }

    emit_code(OP_CONSTANT);
    emit_code(addr);
}

static void parse_primary(void) {
    switch (parser.current.type) {
        case LEFT_PAREN: 
            consume(); 
            parse_expr(0);
            if (consume().type != RIGHT_PAREN) {
                printf("Unclosed parentheses\n");
                exit(-1);
            }
            break;
        case NUMBER_TOKEN: parse_number(consume()); break;
        default: printf("Parsing error\n"); exit(-1);
    } 
}

static OpCode find_operator(Token tok) {
    switch (tok.type) {
        case PLUS_TOKEN: return OP_ADD;
        case MINUS_TOKEN: return OP_SUB;
        case SLASH_TOKEN: return OP_DIV;
        case STAR_TOKEN: return OP_MUL;
        case DOUBLE_EQUAL_TOKEN: return OP_EQUAL;
        case BANG_EQUAL_TOKEN: return OP_NOT_EQUAL; 
        default: return -1;
    }
}

static void parse_expr(uint8_t min_bp) {
    parse_primary();

    for (;;) {
        Token op_tok = look();
        OpCode operator = find_operator(op_tok);
        if (operator == -1) return;

        ParseRule rule = parse_table[op_tok.type];
        if (rule.left_bp < min_bp) {
            break;
        }

        consume(); // operator
        parse_expr(rule.right_bp);
        emit_code(operator);
    }
}

bool compile(const char* source, Chunk* chunk) {
    init_tokenizer(source); 
    parser.current = get_token();

    parser.had_error = false;
    parser.chunk = chunk;
    parse_expr(0);

    emit_code(OP_RETURN);
    return !parser.had_error; // do something better than this
}
