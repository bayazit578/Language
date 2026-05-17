#pragma once

#include "include.h"
#include "lexer_types.h"

token_t* lexer(char** ptr, uint32_t* count);
void dump_tokens(token_t* tokens, uint32_t count);
void destroy_tokens(token_t* tokens, uint32_t count);
