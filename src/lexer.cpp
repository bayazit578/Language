#include "lexer.h"

#include "include.h"
#include "lexer_types.h"

static void init_dfa();
static void init_keyword_dfa();
static void init_ident_dfa();
static void init_num_dfa();

#define CURRENT_TYPE tokens[*count].type
#define CURRENT_VAL  tokens[*count].value

State table[S_ERROR][128];

int state_to_token[] = {};

void destroy_tokens(token_t* tokens, uint32_t count) {
    for (uint64_t i = 0; i < count; i++) {
        if (tokens[i].type != INT && tokens[i].type != FLOAT) {
            free(tokens[i].value.str);
        }
    }

    free(tokens);
}

void dump_tokens(token_t* tokens, uint32_t count) {
    printf(YELLO "==================================="
                 "==================================\n" RESET);
    for (uint32_t i = 0; i < count; i++) {
        if (tokens[i].type == INT) {
            printf(YELLO "[%u]:%d\n" RESET, i, tokens[i].value.intg);
        }

        else if (tokens[i].type == FLOAT) {
            printf(YELLO "[%u]:%0.2f\n" RESET, i, tokens[i].value.dubl);
        }

        else {
            printf(YELLO "[%u]:%s\n" RESET, i, tokens[i].value.str);
        }
    }
    printf(YELLO "==================================="
                 "==================================\n" RESET);
}

token_t* lexer(char** ptr, uint32_t* count) {
    init_dfa();

    *count = 0;
    token_t* tokens = (token_t*)calloc(100, sizeof(token_t));
    char* current = *ptr;
    State state = S_START;
    char* token_start = current;

    while (*current) {
        unsigned char c = (unsigned char)*current;
        State next_state = table[state][c];

        if (state == S_START && isspace(c)) {
            current++;
            token_start = current;
            continue;
        }

        if (state == S_COMMENT) {
            if (c == '\n') {
                state = S_START;
                token_start = current + 1;
            }
            current++;
            continue;
        }

        if (state == S_COMMENT_ML) {
            if (c == '*' && *(current + 1) == '/') {
                state = S_START;
                current += 2;
                token_start = current;
                continue;
            }
            current++;
            continue;
        }

        if (next_state == S_START && state != S_START) {
            uint64_t len = (uint64_t)(current - token_start);
            CURRENT_TYPE = (TokenType)state; 

            if (CURRENT_TYPE == INT) {
                CURRENT_VAL.intg = atoi(token_start);
            }

            else if (CURRENT_TYPE == FLOAT) {
                CURRENT_VAL.dubl = atof(token_start);
            }

            else {
                CURRENT_VAL.str = (char*)calloc(len + 1, sizeof(char));
                strncpy(CURRENT_VAL.str, token_start, len); 
                CURRENT_VAL.str[len] = '\0';
            }

            (*count)++;
            state = S_START;
            token_start = current;
        }

        else if (next_state == S_ERROR) {
            state = S_START;
            token_start = current + 1;
            current++;
        }

        else {
            state = next_state;
            current++;
        }
    }

    CURRENT_TYPE = END_OF_FILE;
    CURRENT_VAL.str = (char*)calloc(1, sizeof(char));
    CURRENT_VAL.str[0] = '\0';
    (*count)++;

    *ptr = current;

    return tokens;
}

static void init_dfa() {
    for (unsigned char i = 0; i < S_ERROR; i++) {
        for (int j = 0; j < 128; j++) {
            table[i][j] = S_ERROR;
        }
    }

    for (unsigned char c = 0; c < 128; c++) {
        if (isspace(c)) {
            table[S_START][c] = S_START;
        }
    }

    table[S_START]['+'] = S_PLUS  ;
    table[S_START]['-'] = S_MINUS ;
    table[S_START]['*'] = S_STAR  ;
    table[S_START]['/'] = S_SLASH ;
    table[S_START]['='] = S_EQ    ;
    table[S_START]['!'] = S_BANG  ;
    table[S_START]['>'] = S_GT    ;
    table[S_START]['<'] = S_LT    ;
    table[S_START]['&'] = S_AMP   ;
    table[S_START]['|'] = S_PIPE  ;
    table[S_START][';'] = S_SEMIC ;
    table[S_START][','] = S_COMMA ;
    table[S_START]['('] = S_LPAREN;
    table[S_START][')'] = S_RPAREN;
    table[S_START]['{'] = S_LBRACE;
    table[S_START]['}'] = S_RBRACE;

    for (unsigned char i = S_PLUS; i < S_DOT_FLOAT; i++) {
        for (unsigned char c = 0; c < 128; c++) {
            table[i][c] = S_START;
        }
    }

    table[S_EQ   ]['='] = S_EQ2       ;
    table[S_BANG ]['='] = S_NE        ;
    table[S_SLASH]['/'] = S_COMMENT   ;
    table[S_SLASH]['*'] = S_COMMENT_ML;
    table[S_STAR ]['/'] = S_COMMENT_ML;
    table[S_AMP  ]['&'] = S_AND       ;
    table[S_PIPE ]['|'] = S_OR        ;
    table[S_GT   ]['='] = S_GT_EQ     ;
    table[S_LT   ]['='] = S_LT_EQ     ;

    init_ident_dfa();
    init_keyword_dfa();
    init_num_dfa();
}

static void init_keyword_dfa() {
    table[S_START]['w'] = S_W;
    table[S_START]['f'] = S_F;
    table[S_START]['d'] = S_D;
    table[S_START]['i'] = S_I;
    table[S_START]['e'] = S_E;

    table[S_ELSE      ][' '] = S_ELSE_SPACE;
    table[S_ELSE_SPACE]['i'] = S_ELSE_I    ;
    table[S_ELSE_I    ]['f'] = S_ELSE_IF   ;

    for (unsigned char c = 0; c < 128; c++) {
        if (isalpha(c) || isdigit(c) || c == '_') {
            table[S_W    ][c] = (c == 'h') ? S_WH    : S_ID;
            table[S_WH   ][c] = (c == 'i') ? S_WHI   : S_ID;
            table[S_WHI  ][c] = (c == 'l') ? S_WHIL  : S_ID;
            table[S_WHIL ][c] = (c == 'e') ? S_WHILE : S_ID;
            table[S_WHILE][c] =              S_ID          ;

            table[S_F    ][c] = (c == 'o') ? S_FO    : S_ID;
            table[S_FO   ][c] = (c == 'r') ? S_FOR   : S_ID;
            table[S_FOR  ][c] =              S_ID          ;

            table[S_D    ][c] = (c == 'o') ? S_DO    : S_ID;
            table[S_DO   ][c] =              S_ID          ;

            table[S_I    ][c] = (c == 'f') ? S_IF    : S_ID;
            table[S_IF   ][c] =              S_ID          ;

            table[S_E    ][c] = (c == 'l') ? S_EL    : S_ID;
            table[S_EL   ][c] = (c == 's') ? S_ELS   : S_ID;
            table[S_ELS  ][c] = (c == 'e') ? S_ELSE  : S_ID;
            table[S_ELSE ][c] =              S_ID          ;
        }

        else if (isspace(c)) {
            table[S_WHILE  ][c] = S_START;
            table[S_FOR    ][c] = S_START;
            table[S_DO     ][c] = S_START;
            table[S_IF     ][c] = S_START;
            table[S_ELSE_IF][c] = S_START;

            if (c != ' ') {
                table[S_ELSE][c] = S_START;
            }
        }

        else if (c == '{') {
            table[S_WHILE     ][c] = S_START;
            table[S_FOR       ][c] = S_START;
            table[S_ELSE      ][c] = S_START;
            table[S_ELSE_SPACE][c] = S_START;
            table[S_ELSE_IF   ][c] = S_START;
        }
    }
}

static void init_ident_dfa() {
    for (unsigned char c = 0; c < 128; c++) {
        if (isalpha(c)) {
            table[S_START][c] = S_ID;
            table[S_ID   ][c] = S_ID;
        }

        else if (isdigit(c) || c == '_') {
            table[S_ID][c] = S_ID;
        }

        else if (c == '{' || c == '}' || isspace(c) ||
                 c == '=' || c == '!' || c == '>'   ||
                 c == '<' || c == '&' || c == '|'   ||
                 c == ')' || c == '/' || c == '*'   ||
                 c == '+' || c == '-') {
            table[S_ID][c] = S_START;
        }
    }
}

static void init_num_dfa() {
    table[S_NUM  ]['.'] = S_DOT_FLOAT;
    table[S_START]['.'] = S_DOT_FLOAT;

    for (unsigned char c = 0; c < 128; c++) {
        if (isdigit(c)) {
            table[S_START    ][c] = S_NUM  ;
            table[S_NUM      ][c] = S_NUM  ;
            table[S_DOT_FLOAT][c] = S_FLOAT;
            table[S_FLOAT    ][c] = S_FLOAT;
            table[S_PLUS     ][c] = S_NUM  ;
            table[S_MINUS    ][c] = S_NUM  ;
        }

        else if (c == '{' || c == '}' || isspace(c) ||
                 c == '=' || c == '!' || c == '>'   ||
                 c == '<' || c == '&' || c == '|'   ||
                 c == ')' || c == '/' || c == '*'   ||
                 c == '+' || c == '-') {
            table[S_NUM  ][c] = S_START;
            table[S_FLOAT][c] = S_START;
        }
    }
}
