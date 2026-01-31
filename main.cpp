#include "include.h"

#include "lexer/lexer.h"
#include "lexer/lexer_types.h"
#include "read_file/read_file.h"

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

    char* ptr = buff;
    uint32_t count = 0;
    Token* tokens = lexer(&ptr, &count);

    for (int i = 0; i < count; i++) {
        if (tokens[i].type == INT) {
            printf(YELLO "[%d]:%d\n" RESET, i, tokens[i].value.intg);
        }

        else if (tokens[i].type == FLOAT) {
            printf(YELLO "[%d]:%0.2f\n" RESET, i, tokens[i].value.dubl);
        }

        else {
            printf(YELLO "[%d]:%s\n" RESET, i, tokens[i].value.str);
        }
    }

    return 0;
}
