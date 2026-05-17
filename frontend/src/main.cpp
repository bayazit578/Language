#include "include.h"
#include "lexer.h"
#include "lexer_types.h"
#include "list.h"
#include "read_file.h"
#include "tree_types.h"
#include "tree.h"
#include "verify.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        fprintf(stderr, RED "No input file. Enter it.\n" RESET);
        return 1;
    }

    if(argc > 2) {
        fprintf(stderr, RED "Invalid filename. Enter it correctly.\n" RESET);
        return 1;
    }

    FILE* in_file = fopen(argv[1], "r");
    if(!in_file) {
        fprintf(stderr, RED "No such file or directory.\n" RESET);
        return 1;
    }

    uint32_t size_buf = 0;
    char* buff = read_file(in_file, &size_buf);
    if (!buff) {
        fprintf(stderr, RED "File_reading error." RESET);
        return 1;
    }

    char* ptr_buff  = buff;
    uint32_t count  = 0;
    token_t* tokens = lexer(&ptr_buff, &count);
    if (!tokens) {
        fprintf(stderr, RED "Error of lexical analyzer!\n" RESET);
        return 1;
    }

    dump_tokens(tokens, count);

    bool synt_error = false;
    uint32_t ptr = 0;
    list_t *func_table   = list_create();
    list_t *symbol_table = list_create();
    tree_t* program_tree = parse_program(tokens, &ptr, &synt_error, 
                                         func_table, symbol_table);
    if (synt_error) {
        goto syntax_error;
    }

    dump_func_table(func_table);
    list_destroy(func_table);
    destroy_tokens(tokens, count);
    free(buff);
    burn_the_tree(program_tree->root);
    free(program_tree);
    return EXIT_SUCCESS;

    syntax_error:
        print_error(ptr);
        destroy_tokens(tokens, count);
        free(buff);
        free(program_tree);
        return EXIT_FAILURE;
}
