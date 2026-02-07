#include "include.h"

#include "lexer/lexer.h"
#include "lexer/lexer_types.h"
#include "read_file/read_file.h"
#include "tree/tree_types.h"
#include "tree/tree.h"
#include "error_handling/verify.h"
#include "parser/parser.h"

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

    char* ptr_buff = buff;
    uint64_t count = 0;
    Token* tokens = lexer(&ptr_buff, &count);
    if (!tokens) {
        fprintf(stderr, RED "Error of lexical analyzer!\n" RESET);
        return 1;
    }

    dump_tokens(tokens, count);

    bool synt_error = false;
    uint32_t ptr = 0;
    Tree* program_tree = parse_program(tokens, &ptr, &synt_error);
    if (synt_error) {
        goto syntax_error;
    }


    destroy_tokens(tokens, count);
    free(buff);
    burn_the_tree(program_tree->root);
    free(program_tree);
    return 0;

    syntax_error:
        print_error(ptr);
        destroy_tokens(tokens, count);
        free(buff);
        free(program_tree);
        return 1;
}
