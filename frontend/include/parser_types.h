#pragma once 

#include <stdint.h>
#include "lexer_types.h"
#include "list_types.h"

typedef struct {
    token_t  *tokens;
    uint32_t *ptr;
    bool     *synt_error;
    list_t   *func_table;
    list_t   *symbol_table;
} parser_t;

typedef struct {
    char    *func_name;
    uint32_t argc;
} func_t;
