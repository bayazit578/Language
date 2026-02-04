#pragma once

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree_types.h"

Tree* parse_program(Token* tokens, uint32_t* ptr, bool* synt_error);
void destruct_tokens(Token* tokens);

#define INCR_PTR(step)  (*ptr) += step
#define CURRENT_TYPE    tokens[*ptr].type
#define CURRENT_VAL     tokens[*ptr].value
#define SYNTAX_ERROR    *synt_error = true;\
                        return NULL;
