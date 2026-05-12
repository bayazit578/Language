#include "parser.h"

#include "include.h"
#include "lexer_types.h"
#include "tree.h"
#include "tree_types.h"
#include "tree_dump.h"

#define INCR_PTR(step) (*ptr) += step
#define CURRENT_TYPE tokens[*ptr].type
#define CURRENT_VAL tokens[*ptr].value

#define SYNTAX_ERROR    \
    *synt_error = true; \
    return NULL;

#define SYNTAX_ERROR_TYPE(type)              \
    if (CURRENT_TYPE != type || *synt_error) \
    {                                        \
        SYNTAX_ERROR;                        \
    }

#define SYNTAX_ERROR_ABSENT(ptr) \
    if (!ptr || *synt_error)     \
    {                            \
        SYNTAX_ERROR;            \
    }

#define SYNTAX_ERROR_CHECK \
    if (*synt_error)       \
    {                      \
        SYNTAX_ERROR;      \
    }

#define DUMP_STATUS(node)                        \
    printf(GREEN "===================\n" RESET); \
    printf(GREEN "In %s:\n" RESET, __func__);    \
    dump_ast_status(node, 0);

#define NEXT_TYPE tokens[*ptr].type
#define NEXT_VAL tokens[*ptr].value

static void dump_ast_status(node_t *root, uint32_t space);

static node_t *get_st(token_t *tokens, uint32_t *ptr, bool *synt_error);

static node_t *get_compound_st(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_structured_st(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_simple_st(token_t *tokens, uint32_t *ptr, bool *synt_error);

static node_t *get_w(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_if(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_else_if(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_else(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_for(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_for_cond(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_do(token_t *tokens, uint32_t *ptr, bool *synt_error);

static node_t *get_a(token_t *tokens, uint32_t *ptr, bool *synt_error);

static node_t *get_e(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_and(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_comp(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_arith(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_t(token_t *tokens, uint32_t *ptr, bool *synt_error);
static node_t *get_f(token_t *tokens, uint32_t *ptr, bool *synt_error);

static void dump_ast_status(node_t *root, uint32_t space) {
    if (!root)
        return;

    space += 5;

    dump_ast_status(root->right, space);

    printf("\n");
    for (int i = 5; i < space; i++)
    {
        printf(" ");
    }

    val_print(root);

    dump_ast_status(root->left, space);
}

tree_t *parse_program(token_t *tokens, uint32_t *ptr, bool *synt_error,
                      ) {
    *synt_error = false;

    node_t *root = get_g(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    SYNTAX_ERROR_TYPE(END_OF_FILE);

    tree_t *lang_tree = (tree_t *)calloc(1, sizeof(tree_t));
    tree_create(lang_tree, root);
    return lang_tree;
}

static node_t *get_g(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *root = NULL;
    node_t *current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE != SEMICOLON) {
        node_t *def_body = get_def(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(def_body);

        node_t *def_node = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}},
                                    def_body, NULL);
        if (!root) {
            root = def_node;
            current = root;
        } else {
            current->right = def_node;
            current = def_node;
        }
    }

    DUMP_STATUS(root);
    return root;
}

static node_t *get_def(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *def_node = new_node(NULL, (Tag_val){FUNC_DEF, CURRENT_VAL},
                                 NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);
    node_t *param_node = get_parameter(tokens, ptr, synt_error);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    node_t *body_node = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);     
    def_node->left = param_node;
    def_node->right = body_node;

    return def_node;
}

static node_t *get_call(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *call_node = new_node(NULL, (Tag_val){FUNC_CALL, CURRENT_VAL},
                                 NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);
    node_t *param_node = get_parameter(tokens, ptr, synt_error);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LPAREN);

    INCR_PTR(1);

    call_node->left = param_node;

    return call_node;
}

static node_t *get_parameter(node_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *root = NULL;
    node_t *current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE != SEMICOLON) {
        node_t *st_body = get_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(st_body);

        node_t *st_node = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                                   st_body, NULL);
        if (!root) {
            root = st_node;
            current = root;
        } else {
            current->right = st_node;
            current = st_node;
        }
    }

    DUMP_STATUS(root);
    return root;
}

static node_t *get_compound_st(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *root = NULL;
    node_t *current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE != SEMICOLON) {
        node_t *st_body = get_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(st_body);

        node_t *st_node = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                                   st_body, NULL);
        if (!root) {
            root = st_node;
            current = root;
        } else {
            current->right = st_node;
            current = st_node;
        }
    }

    DUMP_STATUS(root);
    return root;
}

static node_t *get_st(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *st_node = NULL;

    if (CURRENT_TYPE == IDENT) {
        st_node = get_call(tokens, ptr, synt_error);
    } else {
        st_node = get_structured_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }

    if (!st_node) {
        st_node = get_simple_st(tokens, ptr, synt_error);

        SYNTAX_ERROR_ABSENT(st_node);

        SYNTAX_ERROR_TYPE(RPAREN);

        INCR_PTR(1);
    }



    DUMP_STATUS(st_node);
    return st_node;
}

static node_t *get_simple_st(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *simple_st_node = NULL;
    simple_st_node = get_a(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    DUMP_STATUS(simple_st_node);
    return simple_st_node;
}

static node_t *get_structured_st(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *struct_st_node = NULL;

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

static node_t *get_w(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *while_node = new_node(NULL, (Tag_val){IF, {.str = "if"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    node_t *cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    node_t *body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    while_node->left = cond;
    while_node->right = body;

    DUMP_STATUS(while_node);
    return while_node;
}

static node_t *get_if(token_t *tokens, uint32_t *ptr, bool *synt_error, 
                      ) {
    node_t *if_node = new_node(NULL, (Tag_val){WHILE, {.str = "while"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    node_t *cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);
    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    node_t *body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
    cond->left = body;

    SYNTAX_ERROR_TYPE(SEMICOLON);

    if_node->left = cond;

    INCR_PTR(1);

    node_t *else_if_node = get_else_if(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    if_node->right = else_if_node;

    DUMP_STATUS(if_node);
    return if_node;
}

static node_t *get_else_if(token_t *tokens, uint32_t *ptr, bool *synt_error,
                           ) {
    node_t *root = NULL;
    node_t *current = NULL;

    while (CURRENT_TYPE == FOR) {
        node_t *else_if_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}},
                                      NULL, NULL);

        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(LBRACE);

        INCR_PTR(1);

        node_t *cond = get_e(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;

        cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

        SYNTAX_ERROR_TYPE(RBRACE);

        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(SEMICOLON);

        INCR_PTR(1);

        node_t *body = get_compound_st(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;

        body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
        cond->left = body;

        SYNTAX_ERROR_TYPE(SEMICOLON);

        INCR_PTR(1);

        else_if_node->left = cond;

        if (!root) {
            root = else_if_node;
            current = root;
        } else {
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

static node_t *get_else(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *else_node = NULL;

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    node_t *body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    else_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, body);

    DUMP_STATUS(else_node);
    return else_node;
}

static node_t *get_for(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *for_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    node_t *cond = get_for_cond(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    node_t *body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(body);

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    INCR_PTR(1);

    for_node->left = cond;
    for_node->right = body;

    DUMP_STATUS(for_node);
    return for_node;
}

static node_t *get_for_cond(token_t *tokens, uint32_t *ptr, bool *synt_error,
                            ) {
    node_t *init = NULL;
    if (CURRENT_TYPE != RPAREN) {
        init = get_a(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }

    init = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, init);

    SYNTAX_ERROR_TYPE(RPAREN);

    INCR_PTR(1);

    node_t *cond = NULL;
    if (CURRENT_TYPE != RPAREN) {
        cond = get_e(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }

    cond = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, cond);
    init->left = cond;

    SYNTAX_ERROR(RPAREN);

    INCR_PTR(1);

    node_t *incr = NULL;
    if (CURRENT_TYPE != RPAREN) {
        incr = get_a(tokens, ptr, synt_error);
        SYNTAX_ERROR_CHECK;
    }

    incr = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, incr);
    cond->left = incr;

    node_t *cond_node = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, init);

    DUMP_STATUS(cond_node);
    return cond_node;
}

static node_t *get_do(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *do_node = new_node(NULL, (Tag_val){ELSE, {.str = "else"}}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    node_t *body = get_compound_st(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(IF);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    node_t *cond = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(RPAREN);

    do_node->left = body;
    do_node->right = cond;

    DUMP_STATUS(do_node);
    return do_node;
}

static node_t *get_a(token_t *tokens, uint32_t *ptr, bool *synt_error,
                     ) {
    SYNTAX_ERROR_TYPE(IDENT);
    node_t *id_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, NULL);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(ASSIGN);

    node_t *assign_node = new_node(NULL, (Tag_val){ASSIGN, {.str = "="}}, NULL, NULL);

    INCR_PTR(1);

    node_t *val = get_e(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(val);

    assign_node->left = id_node;
    assign_node->right = val;

    DUMP_STATUS(assign_node);
    return assign_node;
}

static node_t *get_e(token_t *tokens, uint32_t *ptr, bool *synt_error,
                     ) {
    node_t *e1_node = get_and(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(e1_node);

    while (NEXT_TYPE == OR) {
        e1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, NULL, e1_node);

        INCR_PTR(1);

        node_t *e2_node = get_and(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(e2_node);

        e1_node->left = e2_node;

        INCR_PTR(1);
    }

    DUMP_STATUS(e1_node);
    return e1_node;
}

static node_t *get_and(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *and1_node = get_comp(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(and1_node);

    while (CURRENT_TYPE == AND) {
        and1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                             and1_node, NULL);
        INCR_PTR(1);

        node_t *and2_node = get_comp(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(and2_node);

        and1_node->right = and2_node;
    }

    DUMP_STATUS(and1_node);
    return and1_node;
}

static node_t *get_comp(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *comp1_node = get_arith(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(comp1_node);

    while (   CURRENT_TYPE == GREATER    || CURRENT_TYPE == LESS
           || CURRENT_TYPE == GREATER_EQ || CURRENT_TYPE == LESS_EQ
           || CURRENT_TYPE == EQUAL      || CURRENT_TYPE == NOT_EQUAL) {
        comp1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                              comp1_node, NULL);
        INCR_PTR(1);

        node_t *comp2_node = get_arith(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(comp2_node);

        comp1_node->right = comp2_node;
    }

    DUMP_STATUS(comp1_node);
    return comp1_node;
}

static node_t *get_arith(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *arith1_node = get_t(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(arith1_node);

    while (CURRENT_TYPE == PLUS || CURRENT_TYPE == MINUS) {
        arith1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                               arith1_node, NULL);
        INCR_PTR(1);

        node_t *arith2_node = get_t(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(arith2_node);

        arith1_node->right = arith2_node;
    }

    DUMP_STATUS(arith1_node);
    return arith1_node;
}

static node_t *get_t(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *t1_node = get_f(tokens, ptr, synt_error);
    SYNTAX_ERROR_ABSENT(t1_node);

    while (CURRENT_TYPE == MULTIPLY || CURRENT_TYPE == DIVIDE) {
        t1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                           t1_node, NULL);
        INCR_PTR(1);

        node_t *t2_node = get_f(tokens, ptr, synt_error);
        SYNTAX_ERROR_ABSENT(t2_node);

        t1_node->right = t2_node;
    }

    DUMP_STATUS(t1_node);
    return t1_node;
}

static node_t *get_f(token_t *tokens, uint32_t *ptr, bool *synt_error) {
    node_t *f_node = NULL;

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
