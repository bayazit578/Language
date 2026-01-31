#include "lexer.h"

#include "include.h"
#include "lexer_types.h"

State table[S_ERROR][128];

Token* lexer(char** ptr, uint32_t* count) {
    init_dfa();

    *count = 0;
    Token* tokens = (Token*)calloc(100, sizeof(Token));
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

        if (next_state == S_START && state != S_START) {
            int len = current - token_start;

            switch (state) {
                case S_PLUS:
                    tokens[*count].type = PLUS;
                    break;

                case S_MINUS:
                    tokens[*count].type = MINUS;
                    break;

                case S_STAR:
                    tokens[*count].type = MULTIPLY;
                    break;

                case S_SLASH:
                    tokens[*count].type = DIVIDE;
                    break;

                case S_EQ:
                    tokens[*count].type = ASSIGN;
                    break;

                case S_EQ2:
                    tokens[*count].type = EQUAL;
                    break;

                case S_NE:
                    tokens[*count].type = NOT_EQUAL;
                    break;

                case S_AND:
                    tokens[*count].type = AND;
                    break;

                case S_OR:
                    tokens[*count].type = OR;
                    break;

                case S_SEMIC:
                    tokens[*count].type = SEMICOLON;
                    break;

                case S_COMMA:
                    tokens[*count].type = COMMA;
                    break;

                case S_LPAREN:
                    tokens[*count].type = LPAREN;
                    break;

                case S_RPAREN:
                    tokens[*count].type = RPAREN;
                    break;

                case S_LBRACE:
                    tokens[*count].type = LBRACE;
                    break;

                case S_RBRACE:
                    tokens[*count].type = RBRACE;
                    break;

                case S_ID:
                    tokens[*count].type = IDENT;
                    break;

                case S_NUM:
                    tokens[*count].type = INT;
                    break;

                case S_FLOAT:
                    tokens[*count].type = FLOAT;
                    break;

                case S_IF:
                    tokens[*count].type = WHILE;
                    break;

                case S_ELSE_IF:
                    tokens[*count].type = FOR;
                    break;

                case S_ELSE:
                    tokens[*count].type = DO;
                    break;

                case S_WHILE:
                    tokens[*count].type = ELSE;
                    break;

                case S_FOR:
                    tokens[*count].type = ELSE_IF;
                    break;

                case S_DO:
                    tokens[*count].type = IF;
                    break;

                case S_GT:
                    tokens[*count].type = GREATER;
                    break;

                case S_LT:
                    tokens[*count].type = LESS;
                    break;

                default:
                    continue;
            }

            if (tokens[*count].type == INT) {
                tokens[*count].value.intg = atoi(token_start);
                printf(RED "%d\n" RESET, tokens[*count].value.intg);
            }

            else if (tokens[*count].type == FLOAT) {
                tokens[*count].value.dubl = atof(token_start);
                printf(RED "%0.1f\n" RESET, tokens[*count].value.dubl);
            }

            else {
                tokens[*count].value.str = (char*)calloc(len + 1, sizeof(char));
                strncpy(tokens[*count].value.str, token_start, len);
                tokens[*count].value.str[len] = '\0';
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

    tokens[*count].type = END_OF_FILE;
    tokens[*count].value.str = (char*)calloc(1, sizeof(char));
    tokens[*count].value.str[0] = '\0';
    (*count)++;

    *ptr = current;

    return tokens;
}

void init_dfa() {
    for (int i = 0; i < S_ERROR; i++) {
        for (int j = 0; j < 128; j++) {
            table[i][j] = S_ERROR;
        }
    }

    for (int c = 0; c < 128; c++) {
        if (isspace(c)) {
            table[S_START][c] = S_START;
        }
    }

    table[S_START]['+'] = S_PLUS;
    table[S_START]['-'] = S_MINUS;
    table[S_START]['*'] = S_STAR;
    table[S_START]['/'] = S_SLASH;
    table[S_START]['='] = S_EQ;
    table[S_START]['!'] = S_BANG;
    table[S_START]['>'] = S_GT;
    table[S_START]['<'] = S_LT;
    table[S_START]['&'] = S_AMP;
    table[S_START]['|'] = S_PIPE;
    table[S_START][';'] = S_SEMIC;
    table[S_START][','] = S_COMMA;
    table[S_START]['('] = S_LPAREN;
    table[S_START][')'] = S_RPAREN;
    table[S_START]['{'] = S_LBRACE;
    table[S_START]['}'] = S_RBRACE;

    for (int i = S_PLUS; i < S_W; i++) {
        for (int c = 0; c < 128; c++) {
            table[i][c] = S_START; // распознавание // /* == !=
        }
    }

    table[S_EQ]['='] = S_EQ2;
    table[S_BANG]['='] = S_NE;
    table[S_SLASH]['/'] = S_COMMENT;
    table[S_SLASH]['*'] = S_COMMENT_ML;
    table[S_STAR]['/'] = S_COMMENT_ML;
    table[S_AMP]['&'] = S_AND;
    table[S_PIPE]['|'] = S_OR;

    init_ident_dfa();
    init_keyword_dfa();
    init_num_dfa();
}

void init_keyword_dfa() {
    table[S_START]['w'] = S_W;
    table[S_START]['f'] = S_F;
    table[S_START]['d'] = S_D;
    table[S_START]['i'] = S_I;
    table[S_START]['e'] = S_E;

    table[S_ELSE][' '] = S_ELSE_SPACE;
    table[S_ELSE_SPACE]['i'] = S_ELSE_I;
    table[S_ELSE_I]['f'] = S_ELSE_IF;

    for (int c = 0; c < 128; c++) {
        if (isalpha(c) || isdigit(c) || c == '_') {
            table[S_W][c] = (c == 'h') ? S_WH : S_ID;
            table[S_WH][c] = (c == 'i') ? S_WHI : S_ID;
            table[S_WHI][c] = (c == 'l') ? S_WHIL : S_ID;
            table[S_WHIL][c] = (c == 'e') ? S_WHILE : S_ID;
            table[S_WHILE][c] = S_ID;

            table[S_F][c] = (c == 'o') ? S_FO : S_ID;
            table[S_FO][c] = (c == 'r') ? S_FOR : S_ID;
            table[S_FOR][c] = S_ID;

            table[S_D][c] = (c == 'o') ? S_DO : S_ID;
            table[S_DO][c] = S_ID;

            table[S_I][c] = (c == 'f') ? S_IF : S_ID;
            table[S_IF][c] = S_ID;

            table[S_E][c] = (c == 'l') ? S_EL : S_ID;
            table[S_EL][c] = (c == 's') ? S_ELS : S_ID;
            table[S_ELS][c] = (c == 'e') ? S_ELSE : S_ID;
            table[S_ELSE][c] = S_ID;
        }

        else if (isspace(c)) {
            table[S_WHILE][c] = S_START;
            table[S_FOR][c] = S_START;
            table[S_DO][c] = S_START;
            table[S_ELSE_IF][c] = S_START;

            if (c != ' ') {
                table[S_ELSE][c] = S_START;
            }
        }

        else if (c == '{' || c == '}') {
            table[S_WHILE][c] = S_START;
            table[S_FOR][c] = S_START;
            table[S_ELSE][c] = S_START;
            table[S_ELSE_SPACE][c] = S_START;
            table[S_ELSE_IF][c] = S_START;
        }
    }
}

void init_ident_dfa() {
    for (int c = 0; c < 128; c++) {
        if (isalpha(c)) {
            table[S_START][c] = S_ID;
            table[S_ID][c] = S_ID;
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

void init_num_dfa() {
    table[S_NUM]['.'] = S_DOT_FLOAT;
    table[S_START]['.'] = S_DOT_FLOAT;

    for (char c = 0; c < 127; c++) {
        if (isdigit(c)) {
            table[S_START][c] = S_NUM;
            table[S_DOT_FLOAT][c] = S_FLOAT;
            table[S_FLOAT][c] = S_FLOAT;
        }

        else if (c == '{' || c == '}' || isspace(c) ||
                 c == '=' || c == '!' || c == '>'   ||
                 c == '<' || c == '&' || c == '|'   ||
                 c == ')' || c == '/' || c == '*'   ||
                 c == '+' || c == '-') {
            table[S_NUM][c] = S_START;
            table[S_FLOAT][c] = S_START;
        }
    }
}
