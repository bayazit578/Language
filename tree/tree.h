#pragma once

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree_types.h"

void  tree_create(Tree* tree, Node* root);
Node* new_node(Node* parent, Tag_val tagged_value, Node* left, Node* right);
void  burn_the_tree(Node* node);
void  optimize_tree(Node* node);

