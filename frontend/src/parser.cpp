#include "parser.h"
#include "hash_table.h"
#include "lexer_types.h"
#include <cstring>

#define INCR_PTR(step) (*(arg.ptr)) += step
#define CURRENT_TYPE arg.tokens[*(arg.ptr)].type
#define CURRENT_VAL arg.tokens[*(arg.ptr)].value

#define SYNTAX_ERROR          \
    *(arg.synt_error) = true; \
    return NULL;

#define SYNTAX_ERROR_TYPE(type)                    \
    if (CURRENT_TYPE != type || *(arg.synt_error)) \
    {                                              \
        SYNTAX_ERROR;                              \
    }

#define SYNTAX_ERROR_ABSENT(ptr)   \
    if (!ptr || *(arg.synt_error)) \
    {                              \
        SYNTAX_ERROR;              \
    }

#define SYNTAX_ERROR_CHECK \
    if (*(arg.synt_error)) \
    {                      \
        SYNTAX_ERROR;      \
    }

#define DUMP_STATUS(node)                                        \
    printf(GREEN "=================================="            \
                 "===================================\n" RESET); \
    printf(GREEN "In %s:\n" RESET, __func__);                    \
    dump_ast_status(node, 0);

#define NEXT_TYPE arg.tokens[*(arg.ptr) + 1].type
#define NEXT_VAL arg.tokens[*(arg.ptr) + 1].value

static void         dump_ast_status(tree_node_t *root, uint32_t space);

static tree_node_t *get_g(parser_t arg);
static tree_node_t *get_def(parser_t arg);
static tree_node_t *get_call(parser_t arg);
static tree_node_t *get_parameter(parser_t arg, uint32_t *param_count);

static tree_node_t *get_st(parser_t arg);
static tree_node_t *get_compound_st(parser_t arg);
static tree_node_t *get_structured_st(parser_t arg);
static tree_node_t *get_simple_st(parser_t arg);

static tree_node_t *get_w(parser_t arg);
static tree_node_t *get_if(parser_t arg);
static tree_node_t *get_else_if(parser_t arg);
static tree_node_t *get_else(parser_t arg);
static tree_node_t *get_for(parser_t arg);
static tree_node_t *get_for_cond(parser_t arg);
static tree_node_t *get_do(parser_t arg);

static tree_node_t *get_a(parser_t arg);

static tree_node_t *get_e(parser_t arg);
static tree_node_t *get_and(parser_t arg);
static tree_node_t *get_comp(parser_t arg);
static tree_node_t *get_arith(parser_t arg);
static tree_node_t *get_t(parser_t arg);
static tree_node_t *get_f(parser_t arg);

void dump_func_table(list_t *func_table) {
    printf(MAGENTA "==================================="
                   "==================================\n" RESET);
    for (uint32_t i = 1; i < func_table->size; i++) {
        func_t *current_value = (func_t *)func_table->contents[i].value;
        printf(MAGENTA "%s:%u\n" RESET, current_value->func_name, 
                                        current_value->argc); 
    }

    printf(MAGENTA "==================================="
                   "==================================\n" RESET);
}

static void dump_ast_status(tree_node_t *root, uint32_t space) {
    if (!root)
        return;

    space += 5;

    dump_ast_status(root->right, space);

    printf("\n");
    for (uint32_t i = 5; i < space; i++) {
        printf(" ");
    }

    val_print(root);

    dump_ast_status(root->left, space);
}

tree_t *parse_program(token_t *tokens, uint32_t *ptr, bool *synt_error, 
                      list_t *func_table, list_t *symbol_table) {
    *synt_error = false;

    parser_t arg = {tokens, ptr, synt_error, func_table, symbol_table};

    tree_node_t *root = get_g(arg);
    SYNTAX_ERROR_CHECK;

    SYNTAX_ERROR_TYPE(END_OF_FILE);

    tree_t *lang_tree = (tree_t *)calloc(1, sizeof(tree_t));
    tree_create(lang_tree, root);
    return lang_tree;
}

static tree_node_t *get_g(parser_t arg) {
    tree_node_t *root = NULL;
    tree_node_t *current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE == IDENT) {
        tree_node_t *def_body = get_def(arg);
        SYNTAX_ERROR_ABSENT(def_body);

        tree_node_t *def_node = new_node(NULL,
                                         (Tag_val){SEMICOLON, {.str = ";"}},
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

static tree_node_t *get_def(parser_t arg) {
    tree_node_t *def_node = new_node(NULL, (Tag_val){FUNC_DEF, CURRENT_VAL},
                                     NULL, NULL);

    func_t *func_value = (func_t *)calloc(1, sizeof(func_t));
    func_value->func_name = CURRENT_VAL.str;

    list_push_back(arg.func_table, func_value);

    SYNTAX_ERROR_TYPE(IDENT);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);
    uint32_t param_count = 0;
    tree_node_t *param_node = get_parameter(arg, &param_count);
    func_value->argc = param_count;

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    INCR_PTR(1);
    tree_node_t *body_node = get_compound_st(arg);
    SYNTAX_ERROR_CHECK;

    SYNTAX_ERROR_TYPE(SEMICOLON);

    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

    list_pop_back(arg.symbol_table);

    INCR_PTR(1);
    def_node->left = param_node;
    def_node->right = body_node;

    DUMP_STATUS(def_node);
    return def_node;
}

static tree_node_t *get_call(parser_t arg) {
    tree_node_t *call_node = new_node(NULL, (Tag_val){FUNC_CALL, CURRENT_VAL},
                                 NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);
    uint32_t param_count = 0;
    tree_node_t *param_node = get_parameter(arg, &param_count);
    SYNTAX_ERROR_CHECK;

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LPAREN);

    INCR_PTR(1);

    call_node->left = param_node;

    DUMP_STATUS(call_node);
    return call_node;
}

static tree_node_t *get_parameter(parser_t arg, uint32_t *param_count) {
    tree_node_t *root = NULL;
    tree_node_t *current = NULL;
    
    if (CURRENT_TYPE == RBRACE) {
        return NULL;
    }
    
    SYNTAX_ERROR_TYPE(IDENT);
    tree_node_t *first_param = new_node(NULL, (Tag_val){IDENT, CURRENT_VAL},
                                        NULL, NULL);
    (*param_count)++;
    INCR_PTR(1);
    
    root = new_node(NULL, (Tag_val){COMMA, CURRENT_VAL},
                    first_param, NULL);
    current = root;
    
    while (CURRENT_TYPE == COMMA) {
        INCR_PTR(1);
        
        SYNTAX_ERROR_TYPE(IDENT);
        tree_node_t *next_param = new_node(NULL, (Tag_val){IDENT, CURRENT_VAL},
                                           NULL, NULL);
        (*param_count)++;
        INCR_PTR(1);
        
        tree_node_t *param_node = new_node(NULL, (Tag_val){COMMA, CURRENT_VAL},
                                           next_param, NULL);
        
        current->right = param_node;
        current = param_node;
    }

    SYNTAX_ERROR_TYPE(RBRACE);
    
    DUMP_STATUS(root);
    return root;
}

static tree_node_t *get_compound_st(parser_t arg) {
    tree_node_t *root = NULL;
    tree_node_t *current = NULL;

    while (CURRENT_TYPE != END_OF_FILE && CURRENT_TYPE != SEMICOLON) {
        tree_node_t *st_body = get_st(arg);
        SYNTAX_ERROR_ABSENT(st_body);

        tree_node_t *st_node = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
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

static tree_node_t *get_st(parser_t arg) {
    tree_node_t *st_node = NULL;

    st_node = get_structured_st(arg);

    if (!st_node) {
        st_node = get_simple_st(arg);

        SYNTAX_ERROR_ABSENT(st_node);

        SYNTAX_ERROR_TYPE(RPAREN);

        INCR_PTR(1);
    }

    if (!st_node) {
        st_node = get_call(arg);
        SYNTAX_ERROR_CHECK;
    }

    DUMP_STATUS(st_node);
    return st_node;
}

static tree_node_t *get_simple_st(parser_t arg) {
    tree_node_t *simple_st_node = NULL;
    simple_st_node = get_a(arg);
    SYNTAX_ERROR_CHECK;

    DUMP_STATUS(simple_st_node);
    return simple_st_node;
}

static tree_node_t *get_structured_st(parser_t arg) {
    tree_node_t *struct_st_node = NULL;

    switch (CURRENT_TYPE) {
    case IF:
        struct_st_node = get_w(arg);
        SYNTAX_ERROR_CHECK;
        break;

    case WHILE:
        struct_st_node = get_if(arg);
        SYNTAX_ERROR_CHECK;
        break;

    case ELSE_IF:
        struct_st_node = get_for(arg);
        SYNTAX_ERROR_CHECK;
        break;

    case ELSE:
        struct_st_node = get_do(arg);
        SYNTAX_ERROR_CHECK;
        break;

    default:
        return NULL;
    }

    DUMP_STATUS(struct_st_node);
    return struct_st_node;
}

static tree_node_t *get_w(parser_t arg) {
    tree_node_t *while_node = new_node(NULL, (Tag_val){IF, {.str = "if"}}, 
                                       NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    tree_node_t *cond = get_e(arg);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    INCR_PTR(1);

    tree_node_t *body = get_compound_st(arg);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

    list_pop_back(arg.symbol_table);

    INCR_PTR(1);

    while_node->left = cond;
    while_node->right = body;

    DUMP_STATUS(while_node);
    return while_node;
}

static tree_node_t *get_if(parser_t arg) {
    tree_node_t *if_node = new_node(NULL, (Tag_val){WHILE, {.str = "while"}}, 
                                    NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    tree_node_t *cond = get_e(arg);
    SYNTAX_ERROR_CHECK;

    cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);
    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    INCR_PTR(1);

    tree_node_t *body = get_compound_st(arg);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
    cond->left = body;

    SYNTAX_ERROR_TYPE(SEMICOLON);

    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

    list_pop_back(arg.symbol_table);

    if_node->left = cond;

    INCR_PTR(1);

    tree_node_t *else_if_node = get_else_if(arg);
    SYNTAX_ERROR_CHECK;

    if_node->right = else_if_node;

    DUMP_STATUS(if_node);
    return if_node;
}

static tree_node_t *get_else_if(parser_t arg) {
    tree_node_t *root = NULL;
    tree_node_t *current = NULL;

    while (CURRENT_TYPE == FOR) {
        tree_node_t *else_if_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}},
                                      NULL, NULL);
        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(LBRACE);

        INCR_PTR(1);

        tree_node_t *cond = get_e(arg);
        SYNTAX_ERROR_CHECK;

        cond = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, NULL, cond);

        SYNTAX_ERROR_TYPE(RBRACE);

        INCR_PTR(1);

        SYNTAX_ERROR_TYPE(SEMICOLON);

        list_t **scope = hash_table_create();

        list_push_back(arg.symbol_table, scope);

        INCR_PTR(1);

        tree_node_t *body = get_compound_st(arg);
        SYNTAX_ERROR_CHECK;

        body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);
        cond->left = body;

        SYNTAX_ERROR_TYPE(SEMICOLON);

        uint32_t last_ind = arg.symbol_table->contents[0].prev;

        hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

        list_pop_back(arg.symbol_table);

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
        current->right = get_else(arg);
        SYNTAX_ERROR_CHECK;
    }

    DUMP_STATUS(root);
    return root;
}

static tree_node_t *get_else(parser_t arg) {
    tree_node_t *else_node = NULL;

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    INCR_PTR(1);

    tree_node_t *body = get_compound_st(arg);
    SYNTAX_ERROR_CHECK;

    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

    list_pop_back(arg.symbol_table);

    else_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, 
                         NULL, body);

    DUMP_STATUS(else_node);
    return else_node;
}

static tree_node_t *get_for(parser_t arg) {
    tree_node_t *for_node = new_node(NULL, (Tag_val){FOR, {.str = "for"}}, 
                                     NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    tree_node_t *cond = get_for_cond(arg);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    INCR_PTR(1);

    tree_node_t *body = get_compound_st(arg);
    SYNTAX_ERROR_ABSENT(body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_destroy((list_t **)(arg.symbol_table->contents[last_ind].value));

    list_pop_back(arg.symbol_table);


    body = new_node(NULL, (Tag_val){SEMICOLON, {.str = ";"}}, NULL, body);

    INCR_PTR(1);

    for_node->left = cond;
    for_node->right = body;

    DUMP_STATUS(for_node);
    return for_node;
}

static tree_node_t *get_for_cond(parser_t arg) {
    tree_node_t *init = NULL;
    if (CURRENT_TYPE != RPAREN) {
        init = get_a(arg);
        SYNTAX_ERROR_CHECK;
    }

    init = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, init);

    SYNTAX_ERROR_TYPE(RPAREN);

    INCR_PTR(1);

    tree_node_t *cond = NULL;
    if (CURRENT_TYPE != RPAREN) {
        cond = get_e(arg);
        SYNTAX_ERROR_CHECK;
    }

    cond = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, cond);
    init->left = cond;

    SYNTAX_ERROR_TYPE(RPAREN);

    INCR_PTR(1);

    tree_node_t *incr = NULL;
    if (CURRENT_TYPE != RPAREN) {
        incr = get_a(arg);
        SYNTAX_ERROR_CHECK;
    }

    incr = new_node(NULL, (Tag_val){RPAREN, {.str = ")"}},
                    NULL, incr);
    cond->left = incr;

    tree_node_t *cond_node = new_node(NULL, (Tag_val){LBRACE, {.str = "{"}}, 
                                      NULL, init);
    DUMP_STATUS(cond_node);
    return cond_node;
}

static tree_node_t *get_do(parser_t arg) {
    tree_node_t *do_node = new_node(NULL, (Tag_val){ELSE, {.str = "else"}}, 
                                    NULL, NULL);
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    list_t **scope = hash_table_create();

    list_push_back(arg.symbol_table, scope);

    tree_node_t *body = get_compound_st(arg);
    SYNTAX_ERROR_ABSENT(body);

    SYNTAX_ERROR_TYPE(SEMICOLON);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(IF);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(LBRACE);

    INCR_PTR(1);

    tree_node_t *cond = get_e(arg);
    SYNTAX_ERROR_ABSENT(cond);

    SYNTAX_ERROR_TYPE(RBRACE);

    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(RPAREN);

    do_node->left = body;
    do_node->right = cond;

    DUMP_STATUS(do_node);
    return do_node;
}

static tree_node_t *get_a(parser_t arg) {
    SYNTAX_ERROR_TYPE(IDENT);
    tree_node_t *id_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                               NULL, NULL);
    uint32_t last_ind = arg.symbol_table->contents[0].prev;

    hash_table_insert((list_t **)(arg.symbol_table->contents[last_ind].value),
                      (elem_t){CURRENT_VAL.str, (uint32_t)strlen(CURRENT_VAL.str)});
    
    INCR_PTR(1);

    SYNTAX_ERROR_TYPE(ASSIGN);

    tree_node_t *assign_node = new_node(NULL, (Tag_val){ASSIGN, {.str = "="}},
                                        NULL, NULL);

    INCR_PTR(1);

    tree_node_t *val = get_e(arg);
    if (!val) {
        return NULL;
    }

    assign_node->left = id_node;
    assign_node->right = val;

    DUMP_STATUS(assign_node);
    return assign_node;
}

static tree_node_t *get_e(parser_t arg) {
    tree_node_t *e1_node = get_and(arg);
    SYNTAX_ERROR_ABSENT(e1_node);

    while (NEXT_TYPE == OR) {
        e1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL}, 
                           NULL, e1_node);

        INCR_PTR(1);

        tree_node_t *e2_node = get_and(arg);
        SYNTAX_ERROR_ABSENT(e2_node);

        e1_node->left = e2_node;

        INCR_PTR(1);
    }

    DUMP_STATUS(e1_node);
    return e1_node;
}

static tree_node_t *get_and(parser_t arg) {
    tree_node_t *and1_node = get_comp(arg);
    SYNTAX_ERROR_ABSENT(and1_node);

    while (CURRENT_TYPE == AND) {
        and1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                             and1_node, NULL);
        INCR_PTR(1);

        tree_node_t *and2_node = get_comp(arg);
        SYNTAX_ERROR_ABSENT(and2_node);

        and1_node->right = and2_node;
    }

    DUMP_STATUS(and1_node);
    return and1_node;
}

static tree_node_t *get_comp(parser_t arg) {
    tree_node_t *comp1_node = get_arith(arg);
    SYNTAX_ERROR_ABSENT(comp1_node);

    while (   CURRENT_TYPE == GREATER    || CURRENT_TYPE == LESS
           || CURRENT_TYPE == GREATER_EQ || CURRENT_TYPE == LESS_EQ
           || CURRENT_TYPE == EQUAL      || CURRENT_TYPE == NOT_EQUAL) {
        comp1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                              comp1_node, NULL);
        INCR_PTR(1);

        tree_node_t *comp2_node = get_arith(arg);
        SYNTAX_ERROR_ABSENT(comp2_node);

        comp1_node->right = comp2_node;
    }

    DUMP_STATUS(comp1_node);
    return comp1_node;
}

static tree_node_t *get_arith(parser_t arg) {
    tree_node_t *arith1_node = get_t(arg);
    SYNTAX_ERROR_ABSENT(arith1_node);

    while (CURRENT_TYPE == PLUS || CURRENT_TYPE == MINUS) {
        arith1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                               arith1_node, NULL);
        INCR_PTR(1);

        tree_node_t *arith2_node = get_t(arg);
        SYNTAX_ERROR_ABSENT(arith2_node);

        arith1_node->right = arith2_node;
    }

    DUMP_STATUS(arith1_node);
    return arith1_node;
}

static tree_node_t *get_t(parser_t arg) {
    tree_node_t *t1_node = get_f(arg);
    SYNTAX_ERROR_ABSENT(t1_node);

    while (CURRENT_TYPE == MULTIPLY || CURRENT_TYPE == DIVIDE) {
        t1_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                           t1_node, NULL);
        INCR_PTR(1);

        tree_node_t *t2_node = get_f(arg);
        SYNTAX_ERROR_ABSENT(t2_node);

        t1_node->right = t2_node;
    }

    DUMP_STATUS(t1_node);
    return t1_node;
}

static tree_node_t *get_f(parser_t arg) {
    tree_node_t *f_node = NULL;

    switch (CURRENT_TYPE) {
        case IDENT: {
            uint32_t last_ind   = arg.symbol_table->contents[0].prev;
            elem_t   elem_ident = {CURRENT_VAL.str, 
                                        (uint32_t)strlen(CURRENT_VAL.str)};
            bool     found      = false;

            for (uint32_t i = last_ind; i > 0 ; i--) {
                list_t **current_hash_table = 
                    (list_t **)arg.symbol_table->contents[last_ind].value;
                found = hash_table_search(current_hash_table, elem_ident);

                if (found) {
                    break;
                }
            }

            if (!found) {
                SYNTAX_ERROR;
            }
        }
        case INT:
        case FLOAT:
            f_node = new_node(NULL, (Tag_val){CURRENT_TYPE, CURRENT_VAL},
                              NULL, NULL);
            break;

        case LBRACE:
            INCR_PTR(1);

            f_node = get_e(arg);
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
