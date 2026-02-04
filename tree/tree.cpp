#include "tree.h"

#include "include.h"

#include "lexer/lexer_types.h"
#include "tree/tree_types.h"

void tree_create(Tree* tree, Node* root) {
    tree->root = root;
    tree->size = 1;
}

Node* new_node(Node* parent, Tag_val tagged_value,
                      Node* left, Node* right) {
    Node* new_node = (Node*)calloc(1, sizeof(Node));

    assert(new_node && "Error of new_node allocation.");

    switch(tagged_value.type) {
        case KEYWORD:
        case IDENT:
        case PLUS:
        case MINUS:
        case MULTIPLY:
        case DIVIDE:
        case ASSIGN:
        case EQUAL:
        case NOT_EQUAL:
        case AND:
        case OR:
        case LESS:
        case GREATER:
        case LPAREN:
        case RPAREN:
        case LBRACE:
        case RBRACE:
        case COMMA:
        case SEMICOLON:
        case DOT:
        case IF:
        case ELSE_IF:
        case ELSE:
        case WHILE:
        case FOR:
        case DO:
        case END_OF_FILE:
            new_node->value = tagged_value.value;
            break;

        case INT:
            new_node->value = tagged_value.value;
            break;

        case FLOAT:
            new_node->value = tagged_value.value;
            break;

        default:
            fprintf(stderr,
                    RED "Syntax error because of unknown function.\n" RESET);
            return NULL;
    }

    new_node->parent = parent;
    new_node->left = left;
    new_node->right = right;
    new_node->type = tagged_value.type;
    new_node->rank = 0;

    return new_node;
}

void burn_the_tree(Node* node) {
    if(node) {
        if(node->left)
            burn_the_tree(node->left);

        if(node->right)
            burn_the_tree(node->right);

        // if(node->type != INT && node->type != FLOAT) {
        //     free(node->value.str);
        // }

        free(node);
    }
}
