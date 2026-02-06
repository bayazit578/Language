#pragma once

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree_types.h"
#include "tree/tree_dump.h"

Tree* parse_program(Token* tokens, uint32_t* ptr, bool* synt_error);
void destruct_tokens(Token* tokens);
