#include "parser.h"

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree.h"
#include "tree/tree_types.h"

static Node* get_st(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_compound_st(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_structured_st(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_simple_st(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_w(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_if(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_else_if(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_else(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_for(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_do(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_a(Token* tokens, uint32_t* ptr, bool* synt_error);
// Node* get_break(Token* tokens, uint32_t* ptr);
// Node* get_continue(Token* tokens, uint32_t* ptr);

static Node* get_e(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_or(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_and(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_comp(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_arith(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_t(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_f(Token* tokens, uint32_t* ptr, bool* synt_error);

Tree* parse_program(Token* tokens, uint32_t* ptr, bool* synt_error) {
    *synt_error = false;
    Node* root = get_compound_st(tokens, ptr, synt_error);

    INCR_PTR(1);
    if (CURRENT_TYPE != END_OF_FILE) {
        if (CURRENT_TYPE == INT) {
            printf(RED "[%d]:%d\n" RESET, CURRENT_TYPE, CURRENT_VAL.intg);
        }

        else if (CURRENT_TYPE == FLOAT) {
            printf(RED "[%d]:%0.2f\n" RESET, CURRENT_TYPE, CURRENT_VAL.dubl);
        }

        else {
            printf(RED "[%d]:%s\n" RESET, CURRENT_TYPE, CURRENT_VAL.str);
        }
        SYNTAX_ERROR;
    }

    Tree* lang_tree = (Tree*)calloc(1, sizeof(Tree));
    tree_create(lang_tree, root);
    return lang_tree;
}

static Node* get_compound_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* root = NULL;
    Node* current = NULL;

    while (CURRENT_TYPE != END_OF_FILE) {
        Node* st_body = get_st(tokens, ptr, synt_error);
        if (!st_body) {
            SYNTAX_ERROR;
        }

        Node* st_node = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                                st_body, NULL);

        if (!root) {
            root = st_node;
            current = root;
        }

        else {
            current->right = st_node;
            current = st_node;
        }

        INCR_PTR(1);
    }

    return root;
}

static Node* get_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* st_node = get_simple_st(tokens, ptr, synt_error);

    if (st_node && CURRENT_TYPE != RPAREN) {
        SYNTAX_ERROR;
    }

    if (!st_node) {
        if (*synt_error == true) {
            return NULL;
        }

        st_node = get_structured_st(tokens, ptr, synt_error);
        if (!st_node) {
            SYNTAX_ERROR;
        }
    }

    return st_node;
}

static Node* get_simple_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* simple_st_node = NULL;
    simple_st_node = get_a(tokens, ptr, synt_error);
    return simple_st_node;
}

static Node* get_structured_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* struct_st_node = NULL;

    switch (CURRENT_TYPE) {
        case WHILE:
            struct_st_node = get_w(tokens, ptr, synt_error);
            break;

        case IF:
            struct_st_node = get_if(tokens, ptr, synt_error);
            break;

        case FOR:
            struct_st_node = get_for(tokens, ptr, synt_error);
            break;

        case DO:
            struct_st_node = get_do(tokens, ptr, synt_error);
            break;

        default:
            SYNTAX_ERROR;
    }

    return struct_st_node;
}

static Node* get_w(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* while_node = new_node(NULL, (Tag_val){IF, {.str = "if"}}, NULL, NULL);

    INCR_PTR(1);
    if (CURRENT_TYPE != LBRACE) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    Node* cond = get_e(tokens, ptr, synt_error);
    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);
    if (CURRENT_TYPE != RBRACE) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    Node* body = get_compound_st(tokens, ptr, synt_error);
    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    while_node->left = cond;
    while_node->right = body;
    return while_node;
}

static Node* get_if(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* if_node = new_node(NULL, (Tag_val){WHILE, {.str = "while"}}, NULL, NULL);

    INCR_PTR(1);
    if (CURRENT_TYPE != LBRACE) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    Node* cond = get_e(tokens, ptr, synt_error);
    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);
    if (CURRENT_TYPE != RBRACE) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    if (CURRENT_TYPE != RBRACE) {
        SYNTAX_ERROR;
    }

    Node* body = get_compound_st(tokens, ptr, synt_error);
    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
    cond->left = body;

    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    if_node->left = cond;
    INCR_PTR(1);

    Node* else_if_node = get_else_if(tokens, ptr, synt_error);
    if_node->right = else_if_node;

    return if_node;
}

static Node* get_else_if(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* root = NULL;
    Node* current = NULL;

    while (CURRENT_TYPE == FOR) {
        Node* else_if_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}},
                                      NULL, NULL);

        INCR_PTR(1);
        if (CURRENT_TYPE != LBRACE) {
            SYNTAX_ERROR;
        }

        INCR_PTR(1);
        Node* cond = get_e(tokens, ptr, synt_error);
        cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

        if (CURRENT_TYPE != RBRACE) {
            SYNTAX_ERROR;
        }

        INCR_PTR(1);
        if (CURRENT_TYPE != SEMICOLON) {
            SYNTAX_ERROR;
        }

        INCR_PTR(1);
        Node* body = get_compound_st(tokens, ptr, synt_error);
        body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
        cond->left = body;

        if (CURRENT_TYPE != SEMICOLON) {
            SYNTAX_ERROR;
        }
        INCR_PTR(1);

        else_if_node->left = cond;

        if (!root) {
            root = else_if_node;
            current = root;
        }

        else {
            current->right = else_if_node;
            current = else_if_node;
        }
    }

    if (CURRENT_TYPE == DO) {
        current->right = get_else(tokens, ptr, synt_error);
    }

    return root;
}

static Node* get_else(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* else_node = NULL;

    INCR_PTR(1);
    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    Node* body = get_compound_st(tokens, ptr, synt_error);
    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    else_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, body);
    return else_node;
}

static Node* get_for(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* for_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}}, NULL, NULL);

    INCR_PTR(1);
    if (CURRENT_TYPE != LBRACE) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    Node* init = NULL;
    if (CURRENT_TYPE != RPAREN) {
        init = get_a(tokens, ptr, synt_error);
    }
    init = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, init);

    if (CURRENT_TYPE != RPAREN) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    Node* cond = NULL;
    if (CURRENT_TYPE != RPAREN) {
        cond = get_e(tokens, ptr, synt_error);
    }
    cond = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, cond);
    init->left = cond;

    if (CURRENT_TYPE != RPAREN) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    Node* incr = NULL;
    if (CURRENT_TYPE != RPAREN) {
        incr = get_a(tokens, ptr, synt_error);
    }
    incr = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, incr);
    cond->left = incr;

    INCR_PTR(1);
    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    INCR_PTR(1);
    Node* body = get_compound_st(tokens, ptr, synt_error);
    if (!body) {
        SYNTAX_ERROR;
    }
    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    // Структура: for -> (init -> (condition -> (increment -> body)))
    for_node->left = init;
    for_node->right = body;

    return for_node;
}

static Node* get_do(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* do_node = new_node(NULL, (Tag_val){ELSE, {.str = "else"}}, NULL, NULL);

    INCR_PTR(1);
    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }

    Node* body = get_compound_st(tokens, ptr, synt_error);
    if (!body) {
        SYNTAX_ERROR;
    }

    if (CURRENT_TYPE != SEMICOLON) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    if (CURRENT_TYPE != IF) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    if (CURRENT_TYPE != LBRACE) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    Node* condition = get_e(tokens, ptr, synt_error);
    if (!condition) {
        SYNTAX_ERROR;
    }

    if (CURRENT_TYPE != RBRACE) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    if (CURRENT_TYPE != RPAREN) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    do_node->left = body;
    do_node->right = condition;
    return do_node;
}

static Node* get_a(Token* tokens, uint32_t* ptr, bool* synt_error) {
    if (CURRENT_TYPE != IDENT) {
        SYNTAX_ERROR;
    }
    Node* id_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, NULL);

    INCR_PTR(1);
    if (CURRENT_TYPE != ASSIGN) {
        SYNTAX_ERROR;
    }

    Node* assign_node = new_node(NULL, (Tag_val){ASSIGN, {.str = "="}}, NULL, NULL);

    INCR_PTR(1);

    Node* val = get_e(tokens, ptr, synt_error);
    if (!val) {
        SYNTAX_ERROR;
    }

    assign_node->left = id_node;
    assign_node->right = val;
    return assign_node;
}

static Node* get_e(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* e1_node = get_and(tokens, ptr, synt_error);
    if (!e1_node) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    while (CURRENT_TYPE == OR) {
        Node* e1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, e1_node);
        INCR_PTR(1);

        Node* e2_node = get_and(tokens, ptr, synt_error);
        if (!e2_node) {
            SYNTAX_ERROR;
        }

        e1_node->left = e2_node;
        INCR_PTR(1);
    }

    return e1_node;
}

static Node* get_and(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* and1_node = get_comp(tokens, ptr, synt_error);
    if (!and1_node) {
        SYNTAX_ERROR;
    }
    INCR_PTR(1);

    while (CURRENT_TYPE == AND) {
        Node* and1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                   NULL, and1_node);
        INCR_PTR(1);

        Node* and2_node = get_comp(tokens, ptr, synt_error);
        if (!and2_node) {
            SYNTAX_ERROR;
        }

        and1_node->left = and2_node;
    }

    return and1_node;
}

static Node* get_comp(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* comp1_node = get_arith(tokens, ptr, synt_error);
    if (!comp1_node) {
        return NULL;
    }

    while (CURRENT_TYPE == GREATER    || CURRENT_TYPE == LESS    ||
           CURRENT_TYPE == GREATER_EQ || CURRENT_TYPE == LESS_EQ ||
           CURRENT_TYPE == EQUAL      || CURRENT_TYPE == NOT_EQUAL) {
        Node* comp1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                    NULL, comp1_node);
        INCR_PTR(1);

        Node* comp2_node = get_arith(tokens, ptr, synt_error);
        if (!comp2_node) {
            SYNTAX_ERROR;
        }

        comp1_node->left = comp2_node;
    }

    return comp1_node;
}

static Node* get_arith(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* arith1_node = get_t(tokens, ptr, synt_error);
    if (!arith1_node) {
        return NULL;
    }

    while (CURRENT_TYPE == PLUS || CURRENT_TYPE == MINUS) {
        Node* arith1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                    NULL, arith1_node);
        INCR_PTR(1);

        Node* arith2_node = get_t(tokens, ptr, synt_error);
        if (!arith2_node) {
            SYNTAX_ERROR;
        }

        arith1_node->left = arith2_node;
    }

    return arith1_node;
}

static Node* get_t(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* t1_node = get_f(tokens, ptr, synt_error);
    if (!t1_node) {
        return NULL;
    }

    while (CURRENT_TYPE == MULTIPLY || CURRENT_TYPE == DIVIDE) {
        Node* t1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                 NULL, t1_node);
        INCR_PTR(1);

        Node* t2_node = get_f(tokens, ptr, synt_error);
        if (!t2_node) {
            SYNTAX_ERROR;
        }

        t1_node->left = t2_node;
    }

    return t1_node;
}

static Node* get_f(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* f_node = NULL;

    switch (CURRENT_TYPE) {
        case IDENT:
        case INT:
        case FLOAT:
            INCR_PTR(1);
            f_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, NULL);
            break;

        case LBRACE:
            INCR_PTR(1);
            f_node = get_e(tokens, ptr, synt_error);
            if (!f_node) {
                SYNTAX_ERROR;
            }

            if (CURRENT_TYPE != RBRACE) {
                SYNTAX_ERROR;
            }
            INCR_PTR(1);

            break;

        default:
            return NULL;
    }

    return f_node;
}
