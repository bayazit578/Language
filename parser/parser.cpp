#include "parser.h"

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree_types.h"

Tree* get_g(Token* tokens, uint32_t* ptr) {
    Node* root = get_st(tokens, ptr);

    optimize_tree(root);

    Tree* lang_tree = (Tree*)calloc(1, sizeof(Tree));
    tree_create(lang_tree, root);
    return lang_tree;
}

Node* get_st(Token* tokens, uint32_t* ptr) {

}

Node* get_w(Token* tokens, uint32_t* ptr);
Node* get_a(Token* tokens, uint32_t* ptr);
Node* get_s(Token* tokens, uint32_t* ptr);
Node* get_m(Token* tokens, uint32_t* ptr);
Node* get_p(Token* tokens, uint32_t* ptr);
