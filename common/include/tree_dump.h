#pragma once

#include "include.h"
#include "tree_types.h"
#include "lexer_types.h"
#include <stdint.h>

void val_print(tree_node_t* node);
void tree_dump(tree_t* tree, const char* filename);
