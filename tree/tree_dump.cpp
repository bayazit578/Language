#include "tree_dump.h"

#include "include.h"
#include "tree/tree_types.h"
#include "lexer/lexer_types.h"

static void node_dump(Node* node, FILE* dot_file);

void val_print(Node* node) {
    if (node->type == INT) {
        printf(RED "%d\n" RESET, node->type, node->value.intg);
    }

    else if (node->type == FLOAT) {
        printf(RED "%0.2f\n" RESET, node->type, node->value.dubl);
    }

    else {
        printf(RED "%s\n" RESET, node->type, node->value.str);
    }
}

void tree_dump(Tree* tree, const char* filename) {
    if (tree->root == NULL) {
        fprintf(stderr, RED "Tree is NULL" RESET "\n");
        return;
    }

    FILE* dot_file = fopen(filename, "w");
    if (dot_file == NULL) {
        fprintf(stderr, RED "Cannot create DOT file" RESET "\n");
        return;
    }

    fprintf(dot_file, "digraph G {\n"
                      "    rankdir=TB;\n"
                      "    bgcolor=\"transparent\";\n"
                      "    node [shape=plaintext, "
                                "style=filled, "
                                "color=\"#ffffffff\", "
                                "fontname=\"Times New Roman\", "
                                "fontcolor=\"white\", "
                                "charset=\"UTF-8\"; "
                                "fontsize=15];\n\n"
                      "    edge[color = \"red\"];\n");
    node_dump(tree->root, dot_file);

    fprintf(dot_file, "}\n");
    fclose(dot_file);
}

static void node_dump(Node* node, FILE* dot_file) {
    if(!node)
        return;

    fprintf(dot_file, "    node%u [label=<<TABLE BORDER=\"0\""
                      " CELLBORDER=\"1\" CELLSPACING=\"0\">\n", node);

    switch (node->type) {
        case INT:
            fprintf(dot_file, "           "
                              "<TR><TD COLSPAN=\"2\">NUM</TD></TR>\n"
                              "           "
                              "<TR><TD COLSPAN=\"2\">%d</TD></TR>\n",
                              node->value.intg);
            break;

        case FLOAT:
            fprintf(dot_file, "           "
                            "<TR><TD COLSPAN=\"2\">FLOAT</TD></TR>\n"
                            "           "
                            "<TR><TD COLSPAN=\"2\">%.3lf</TD></TR>\n",
                            node->value.dubl);
            break;

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
            fprintf(dot_file, "           "
                              "<TR><TD COLSPAN=\"2\">VAR</TD></TR>\n"
                              "           "
                              "<TR><TD COLSPAN=\"2\">%s</TD></TR>\n",
                              node->value.str);
            break;

        default:
            fprintf(stderr, RED "Unknown type of node value\n" RESET);
            break;
    }

    if (node->left != NULL)
        fprintf(dot_file, "           <TR><TD>%p</TD>", node);
    else
        fprintf(dot_file, "           <TR><TD>0</TD>");

    if (node->right != NULL)
        fprintf(dot_file, "           <TD>%p</TD></TR>\n", node);
    else
        fprintf(dot_file, "           <TD>0</TD></TR>\n");

    fprintf(dot_file, "           </TABLE>>, fillcolor=\"#26f950ff\"];\n");

    fprintf(dot_file, "    {\n"
                      "        rank=%u;\n"
                      "        node%u;\n"
					  "    }\n", node->rank, node);

    if(node->left) {
        node_dump(node->left, dot_file);
        fprintf(dot_file, "    node%u->node%u\n", node, node->left);
    }

    if(node->right) {
        node_dump(node->right, dot_file);
        fprintf(dot_file, "    node%u->node%u\n", node, node->right);
    }
}

