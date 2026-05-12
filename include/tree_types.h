#pragma once

#include "include.h"
#include "lexer_types.h"

typedef struct node_t node_t;

struct node_t{
    TokenType type;
    value_t   value;
    node_t*   parent;
    node_t*   left;
    node_t*   right;
    uint32_t  rank;
    };

typedef struct {
    node_t* root;
    size_t  size;
    } tree_t;

typedef struct {
    TokenType type;
    value_t   value;
    } Tag_val;
