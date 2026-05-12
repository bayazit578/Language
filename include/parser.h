#pragma once

#include "include.h"
#include "lexer_types.h"
#include "tree_types.h"
#include "tree_dump.h"
#include "parser_types.h"

tree_t* parse_program(token_t* tokens, uint32_t* ptr, bool* synt_error);
void destruct_tokens(token_t* tokens);
