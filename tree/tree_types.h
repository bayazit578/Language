#pragma once

#include "include.h"
#include "lexer/lexer_types.h"

typedef struct Node Node;

struct Node{
    TokenType type;
    Value     value;
    Node*     parent;
    Node*     left;
    Node*     right;
        };

typedef struct {
    Node*  root;
    size_t size;
        } Tree;

typedef struct {
    TokenType type;
    Value     value;
} Tag_val;
