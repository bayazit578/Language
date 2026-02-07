#include "parser.h"

#include "include.h"
#include "lexer/lexer_types.h"
#include "tree/tree.h"
#include "tree/tree_types.h"
#include "tree/tree_dump.h"

#define INCR_PTR(step)              (*ptr) += step
#define CURRENT_TYPE                tokens[*ptr].type
#define CURRENT_VAL                 tokens[*ptr].value

#define SYNTAX_ERROR                *synt_error = true; \
                                    return NULL;

#define SYNTAX_ERROR_TYPE(type)     if (CURRENT_TYPE != type || *synt_error) { \
                                        SYNTAX_ERROR;                           \
                                    }

#define SYNTAX_ERROR_ABSENT(ptr)    if (!ptr || *synt_error) { \
                                    SYNTAX_ERROR;               \
                                    }

#define SYNTAX_ERROR_CHECK          if (*synt_error) { \
                                        SYNTAX_ERROR;   \
                                    }

#define DUMP_STATUS(node)           printf(GREEN "===================\n" RESET); \
                                    printf(GREEN "In %s:\n" RESET, __func__);     \
                                    dump_tree_status(node, 0);

#define NEXT_TYPE                   tokens[*ptr].type
#define NEXT_VAL                    tokens[*ptr].value


static void dump_tree_status(Node* root, uint32_t space);

static Node* get_st(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_compound_st(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_structured_st(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_simple_st(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_w(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_if(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_else_if(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_else(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_for(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_for_cond(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_do(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_a(Token* tokens, uint32_t* ptr, bool* synt_error);

static Node* get_e(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_and(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_comp(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_arith(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_t(Token* tokens, uint32_t* ptr, bool* synt_error);
static Node* get_f(Token* tokens, uint32_t* ptr, bool* synt_error);


static void dump_tree_status(Node* root, uint32_t space) {
    if (!root)
        return;

    space += 5;

    dump_tree_status(root->right, space);

    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }

    val_print(root);

    dump_tree_status(root->left, space);
}

Tree* parse_program(Token* tokens, uint32_t* ptr, bool* synt_error) {
    *synt_error = false;

    Node* root = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    SYNTAX_ERROR_TYPE(END_OF_FILE);

    Tree* lang_tree = (Tree*)calloc(1, sizeof(Tree));
    tree_create(lang_tree, root);
    return lang_tree;
}

static Node* get_compound_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* root = NULL;
    Node* current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE != SEMICOLON) {
        Node* st_body = get_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(st_body);

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
    }

    DUMP_STATUS(root);
    return root;
}

static Node* get_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* st_node = get_structured_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    if (!st_node) {
        st_node = get_simple_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(st_node);

        SYNTAX_ERROR_TYPE(RPAREN);

        INCR_PTR(1);
    }

    DUMP_STATUS(st_node);
    return st_node;
}

static Node* get_simple_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* simple_st_node = NULL;
    simple_st_node = get_a(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    DUMP_STATUS(simple_st_node);
    return simple_st_node;
}

static Node* get_structured_st(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* struct_st_node = NULL;

    switch (CURRENT_TYPE) {
        case IF:
            struct_st_node = get_w(tokens, ptr, synt_error);
            SYNTAX_ERROR_CHECK;
            break;

        case WHILE:
            struct_st_node = get_if(tokens, ptr, synt_error);
            SYNTAX_ERROR_CHECK;
            break;

        case ELSE_IF:
            struct_st_node = get_for(tokens, ptr, synt_error);
            SYNTAX_ERROR_CHECK;
            break;

        case ELSE:
            struct_st_node = get_do(tokens, ptr, synt_error);
            SYNTAX_ERROR_CHECK;
            break;

        default:
            return NULL;
    }

    DUMP_STATUS(struct_st_node);
    return struct_st_node;
}

static Node* get_w(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* while_node = new_node(NULL, (Tag_val){IF, {.str = "if"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    Node* cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    Node* body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    while_node->left = cond;
    while_node->right = body;

    DUMP_STATUS(while_node);
    return while_node;
}

static Node* get_if(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* if_node = new_node(NULL, (Tag_val){WHILE, {.str = "while"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    Node* cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);
    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    Node* body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
    cond->left = body;

    SYNTAX_ERROR_TYPE(SEMICOLON);

    if_node->left = cond;

    INCR_PTR(1);

    Node* else_if_node = get_else_if(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    if_node->right = else_if_node;

    DUMP_STATUS(if_node);
    return if_node;
}

static Node* get_else_if(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* root = NULL;
    Node* current = NULL;

    while (CURRENT_TYPE == FOR) {
        Node* else_if_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}},
                                      NULL, NULL);

        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(LBRACE);

        INCR_PTR(1);

        Node* cond = get_e(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;

        cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

        SYNTAX_ERROR_TYPE(RBRACE);

        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(SEMICOLON);

        INCR_PTR(1);

        Node* body = get_compound_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;

        body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
        cond->left = body;

        SYNTAX_ERROR_TYPE(SEMICOLON);

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
        SYNTAX_ERROR_CHECK;
    }

    DUMP_STATUS(root);
    return root;
}

static Node* get_else(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* else_node = NULL;

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    Node* body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    else_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, body);

    DUMP_STATUS(else_node);
    return else_node;
}

static Node* get_for(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* for_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    Node* cond = get_for_cond(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    Node* body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(body);

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    INCR_PTR(1);

    for_node->left = cond;
    for_node->right = body;

    DUMP_STATUS(for_node);
    return for_node;
}

static Node* get_for_cond(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* init = NULL;
    if (CURRENT_TYPE != RPAREN) {
        init = get_a(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }
    init = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, init);

    SYNTAX_ERROR_TYPE(RPAREN);

    INCR_PTR(1);

    Node* cond = NULL;
    if (CURRENT_TYPE != RPAREN) {
        cond = get_e(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }
    cond = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, cond);
    init->left = cond;

    SYNTAX_ERROR(RPAREN);

    INCR_PTR(1);

    Node* incr = NULL;
    if (CURRENT_TYPE != RPAREN) {
        incr = get_a(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }
    incr = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, incr);
    cond->left = incr;

    Node* cond_node = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, init);

    DUMP_STATUS(cond_node);
    return cond_node;
}

static Node* get_do(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* do_node = new_node(NULL, (Tag_val){ELSE, {.str = "else"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    Node* body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(IF);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    Node* cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(RPAREN);

    do_node->left = body;
    do_node->right = cond;

    DUMP_STATUS(do_node);
    return do_node;
}

static Node* get_a(Token* tokens, uint32_t* ptr, bool* synt_error) {
    SYNTAX_ERROR_TYPE(IDENT);
    Node* id_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(ASSIGN);

    Node* assign_node = new_node(NULL, (Tag_val){ASSIGN, {.str = "="}}, NULL, NULL);

    INCR_PTR(1);

    Node* val = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(val);

    assign_node->left = id_node;
    assign_node->right = val;

    DUMP_STATUS(assign_node);
    return assign_node;
}

static Node* get_e(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* e1_node = get_and(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(e1_node);

    while (NEXT_TYPE == OR) {
        e1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, e1_node);

        INCR_PTR(1);

        Node* e2_node = get_and(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(e2_node);

        e1_node->left = e2_node;

        INCR_PTR(1);
    }

    DUMP_STATUS(e1_node);
    return e1_node;
}

static Node* get_and(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* and1_node = get_comp(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(and1_node);

    while (CURRENT_TYPE == AND) {
        and1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                   and1_node, NULL);
        INCR_PTR(1);

        Node* and2_node = get_comp(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(and2_node);

        and1_node->right = and2_node;
    }

    DUMP_STATUS(and1_node);
    return and1_node;
}

static Node* get_comp(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* comp1_node = get_arith(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(comp1_node);

    while (CURRENT_TYPE == GREATER    || CURRENT_TYPE == LESS    ||
           CURRENT_TYPE == GREATER_EQ || CURRENT_TYPE == LESS_EQ ||
           CURRENT_TYPE == EQUAL      || CURRENT_TYPE == NOT_EQUAL) {
        comp1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                    comp1_node, NULL);
        INCR_PTR(1);

        Node* comp2_node = get_arith(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(comp2_node);

        comp1_node->right = comp2_node;
    }

    DUMP_STATUS(comp1_node);
    return comp1_node;
}

static Node* get_arith(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* arith1_node = get_t(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(arith1_node);

    while (CURRENT_TYPE == PLUS || CURRENT_TYPE == MINUS) {
        arith1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                    arith1_node, NULL);
        INCR_PTR(1);

        Node* arith2_node = get_t(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(arith2_node);

        arith1_node->right = arith2_node;
    }

    DUMP_STATUS(arith1_node);
    return arith1_node;
}

static Node* get_t(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* t1_node = get_f(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(t1_node);

    while (CURRENT_TYPE == MULTIPLY || CURRENT_TYPE == DIVIDE) {
        t1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                                 t1_node, NULL);
        INCR_PTR(1);

        Node* t2_node = get_f(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(t2_node);

        t1_node->right = t2_node;
    }

    DUMP_STATUS(t1_node);
    return t1_node;
}

static Node* get_f(Token* tokens, uint32_t* ptr, bool* synt_error) {
    Node* f_node = NULL;

    switch (CURRENT_TYPE) {
        case IDENT:
        case INT:
        case FLOAT:
            f_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                              NULL, NULL);
            break;

        case LBRACE:
            INCR_PTR(1);

            f_node = get_e(tokens, ptr, synt_error);
            SYNTAX_ERROR_ABSENT(f_node);

            INCR_PTR(1);

            SYNTAX_ERROR_TYPE(RBRACE);

            break;

        default:
            return NULL;
    }

    INCR_PTR(1);

    DUMP_STATUS(f_node);
    return f_node;
}
