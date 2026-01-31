#pragma once

#include "include.h"
#include "lexer_types.h"

Token* lexer(char** ptr, uint32_t* count);
void init_dfa();
void init_keyword_dfa();
void init_ident_dfa();
void init_num_dfa();
