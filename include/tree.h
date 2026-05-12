#pragma once

#include "include.h"
#include "lexer_types.h"
#include "tree_types.h"

void tree_create(tree_t* tree, node_t* root);
node_t* new_node(node_t* parent, Tag_val tagged_value, node_t* left, node_t* right);
void burn_the_tree(node_t* node);
void optimize_tree(node_t* node);

