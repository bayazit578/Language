#pragma once

#include "include.h"
#include "lexer/types_lexer.h"
#include "tree_types.h"

void  tree_create(Tree* tree, Node* root);
Node* new_node(Node* parent, Tagged_val tagged_value, Node* left,
               Node* right, List_oper* oper_table);
void  burn_the_tree(Node* node);
void  optimize_tree(Node* node, List_oper* oper_table);

