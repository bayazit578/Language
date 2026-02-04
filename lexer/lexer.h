#pragma once

#include "include.h"
#include "lexer_types.h"

Token* lexer(char** ptr, uint32_t* count);

#define CURRENT_TYPE tokens[*count].type
#define CURRENT_VAL  tokens[*count].value
