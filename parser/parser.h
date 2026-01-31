#pragma once

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree_types.h"

Tree* get_g(Token* tokens, uint32_t* ptr);
Node* get_st(Token* tokens, uint32_t* ptr);
Node* get_w(Token* tokens, uint32_t* ptr);
Node* get_a(Token* tokens, uint32_t* ptr);
Node* get_s(Token* tokens, uint32_t* ptr);
Node* get_m(Token* tokens, uint32_t* ptr);
Node* get_p(Token* tokens, uint32_t* ptr);
