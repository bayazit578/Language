#pragma once

#include "include.h"
#include "lexer_types.h"

typedef struct tree_node_t tree_node_t;

struct tree_node_t {
    TokenType    type;
    value_t      value;
    tree_node_t* parent;
    tree_node_t* left;
    tree_node_t* right;
    uint32_t     rank;
};

typedef struct {
    tree_node_t* root;
    size_t       size;
} tree_t;

typedef struct {
    TokenType type;
    value_t   value;
} Tag_val;
