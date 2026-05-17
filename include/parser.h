#pragma once

#include "include.h"
#include "list.h"
#include "tree.h"
#include "lexer_types.h"
#include "tree_types.h"
#include "tree_dump.h"
#include "hash_table.h"
#include "hash_table_types.h"
#include "parser_types.h"

void    dump_func_table(list_t *func_table);
tree_t* parse_program(token_t* tokens, uint32_t* ptr, bool* synt_error, 
                      list_t *func_table, list_t *symbol_table);
void    destruct_tokens(token_t* tokens);
