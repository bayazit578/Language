#include "tree.h"

void tree_create(tree_t* tree, tree_node_t* root) {
    tree->root = root;
    tree->size = 1;
}

tree_node_t* new_node(tree_node_t* parent, Tag_val tagged_value,
                      tree_node_t* left, tree_node_t* right) {
    tree_node_t* new_node = (tree_node_t*)calloc(1, sizeof(tree_node_t));

    assert(new_node && "Error of new_node allocation.");

    new_node->value  = tagged_value.value;

    new_node->parent = parent;
    new_node->left   = left;
    new_node->right  = right;
    new_node->type   = tagged_value.type;
    new_node->rank   = 0;

    return new_node;
}

void burn_the_tree(tree_node_t* node) {
    if(node) {
        if(node->left)
            burn_the_tree(node->left);

        if(node->right)
            burn_the_tree(node->right);
        
        free(node);
    }
}
