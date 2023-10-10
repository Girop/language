#include "virtual_machine.h"
#include "tokenizer.h"
#include "chunk.h"
#include "parser.h"

typedef struct {
    Chunk* chunk;
    Token current;
    bool had_error;
} Parser;

typedef struct {
    Token name;
    int depth;
} Local;

#define UINT8_COUNT (UINT8_MAX+1)
typedef struct {
    Local locals[UINT8_COUNT];
    int local_count;
    int scope_depth;
} Compiler;

static Parser parser;
static Compiler* current_compiler;

#define emit_code(code) write_instruction(parser.chunk, code)

static Token look(void) {
    return parser.current;
}

static Token consume(void) {
    Token res = parser.current;
    parser.current = get_token();
    return res;
}

static void discard_expected(TokenType expected_type, const char* msg) {
    Token next = consume();
    if (next.type == expected_type) return;

    printf("%s\n", msg);
    exit(EXIT_FAILURE);
}

static bool check_type(TokenType type) {
    return parser.current.type == type;
}

static bool match(TokenType type) {
    if (!check_type(type)) return false;
    consume();
    return true;
}

static bool get_match(TokenType type, Token* res) {
    if (!check_type(type)) return false;
    *res = consume();
    return true;
}

static void init_compiler(Compiler* compiler) {
    compiler->local_count = 0;
    compiler->scope_depth = 0;
    current_compiler = compiler;
}

// Expression bottom - up parser
typedef enum {
    PREC_NONE,
    PREC_BASIC,
    PREC_ADDITIVE_LEFT,
    PREC_ADDITIVE_RIGT,
    PREC_MUL_LEFT,
    PREC_MUL_RIGHT,
    PREC_COMP_LEFT,
    PREC_COMP_RIGHT,
} Precedence;

typedef struct {
    Precedence left_bp;
    Precedence right_bp;
    OpCode code;
} ParseRule;

const ParseRule EXPRESSION_TABLE[] = {
    [PLUS_TOKEN] = {PREC_ADDITIVE_LEFT, PREC_ADDITIVE_RIGT, OP_ADD},
    [MINUS_TOKEN] = {PREC_ADDITIVE_LEFT, PREC_ADDITIVE_RIGT, OP_SUB},
    [CONCAT_TOKEN] = {PREC_ADDITIVE_LEFT, PREC_ADDITIVE_RIGT, OP_CONCAT},
    [SLASH_TOKEN] = {PREC_MUL_LEFT, PREC_MUL_RIGHT, OP_DIV},
    [PERCENT_TOKEN] = {PREC_MUL_LEFT, PREC_MUL_RIGHT, OP_DIV},
    [STAR_TOKEN] = {PREC_MUL_LEFT, PREC_MUL_RIGHT, OP_MUL},
    [DOUBLE_EQUAL_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_EQUAL},
    [BANG_EQUAL_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_NOT_EQUAL}, 
    [LESS_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_LESS},
    [LESS_EQUAL_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_LESS_EQUAL},
    [MORE_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_MORE},
    [MORE_EQUAL_TOKEN] = {PREC_COMP_LEFT, PREC_COMP_RIGHT, OP_MORE_EQUAL},
};

static void parse_expr(uint8_t min_bp);
#define new_expression() parse_expr(PREC_BASIC)

static uint8_t add_string_value(Token token) {
    StringObj* identifier = create_string(token.string.position, token.string.len);
    uint8_t glob_addr = add_constant(parser.chunk, from_string(identifier));

    if (glob_addr > UINT8_MAX) {
        printf("Too many constatns!\n");
        exit(EXIT_FAILURE);
    }

    return glob_addr;
}

static void parse_constant(Token token) {
    Value val;
    switch (token.type) {
        case NUMBER_TOKEN:
            val.type = NumberValue;
            val.as.number = token.number;
            break;
        case TRUE_TOKEN:
            val.type = BoolValue;
            val.as.boolean = true;
            break;
        case FALSE_TOKEN:
            val.type = BoolValue;
            val.as.boolean = false; 
            break;
        case STRING_TOKEN:
            val.type = ObjValue;
            val.as.object = (Obj*) create_string(token.string.position, token.string.len);
            break;
        case NIL_TOKEN:
            val.type = NilValue;
            break;
        default:
            exit(EXIT_FAILURE);
    } 

    uint8_t addr = add_constant(parser.chunk, val);

    if (addr >= UINT8_MAX) {
        printf("Too many constants!\n");
        exit(EXIT_FAILURE);
    }

    emit_code(OP_CONSTANT);
    emit_code(addr);
}

static void parse_primary(void) {
    Token res;
    if (get_match(IDENTIFIER_TOKEN, &res)) {
        uint8_t addr = add_string_value(res);

        emit_code(OP_GET_GLOBAL);
        emit_code(addr);
        return;    
    }

    if (match(LEFT_PAREN)) {
        new_expression();
        discard_expected(RIGHT_PAREN, "Undelimited expression");
        return;
    }
    parse_constant(consume());
}

static void parse_unary(void) {  
    // TODO Parsing for: - !
    /* for (;;) {      */
    /*     if (match(BANG_TOKEN)) { */
    /*      */
    /*     } */
    /*     if (match(MINUS_TOKEN)) {  */
    /*          */
    /*     } */
    /* } */

    parse_primary();
}

static void parse_expr(uint8_t min_bp) {
    parse_unary();

    for (;;) {
        Token op_tok = look();
        ParseRule rule = EXPRESSION_TABLE[op_tok.type];

        if (rule.code == -1) return;
        if (rule.left_bp < min_bp) break;

        consume(); // operator
        parse_expr(rule.right_bp);
        emit_code(rule.code);
    }
}

// Statement parser
static void parse_global(void) {
    Token name = consume();

    if (name.type != IDENTIFIER_TOKEN) {

        printf("Expected global name\n");
        exit(EXIT_FAILURE);
    }

    uint8_t glob_addr = add_string_value(name);
    if (match(EQUAL_TOKEN)) {
        new_expression();
    } else {
       parse_constant((Token) {.type = NIL_TOKEN});
    }

    emit_code(OP_NEW_GLOBAL);
    emit_code(glob_addr);
}

static void parse_statement(void) {
    if (match(PRINT_TOKEN)) {
        new_expression();
        emit_code(OP_PRINT);
    } else if (match(GLOBAL_VAR_TOKEN)) { 
        parse_global();
    } else {
        new_expression();
        emit_code(OP_POP);
    }
    discard_expected(SEMICOLON_TOKEN, "Expected ';' at end of expected statement");
}

bool compile(const char* source, Chunk* chunk) {
    init_tokenizer(source);
    Compiler compiler;
    init_compiler(&compiler);

    parser.current = get_token();
    parser.had_error = false;
    parser.chunk = chunk;

    while (!match(EOF_TOKEN)) {
        parse_statement(); 
    }

    emit_code(OP_RETURN);
    return !parser.had_error; // do something better than this
}
