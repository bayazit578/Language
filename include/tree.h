#pragma once

#include "include.h"
#include "lexer_types.h"
#include "tree_types.h"

void tree_create(tree_t* tree, tree_node_t* root);
tree_node_t* new_node(tree_node_t* parent, Tag_val tagged_value, 
                      tree_node_t* left, tree_node_t* right);
void burn_the_tree(tree_node_t* node);
void optimize_tree(tree_node_t* node);

