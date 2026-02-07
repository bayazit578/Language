#pragma once

#include "include.h"
#include "lexer_types.h"

Token* lexer(char** ptr, uint32_t* count);
void dump_tokens(Token* tokens, uint32_t count);
void destroy_tokens(Token* tokens, uint32_t count);
