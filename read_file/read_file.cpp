#include "read_file.h"

char* read_file(FILE* in_file, uint32_t* size) {
    int fd = fileno(in_file);

    struct stat input_info;
    fstat(fd, &input_info);
    if (!input_info.st_size) {
        fprintf(stderr, "Empty expression\n");
        return NULL;
    }

    char* buff = (char*)calloc((uint32_t)input_info.st_size + 1, sizeof(char));

    uint32_t bytes_read = fread(buff, sizeof(buff[0]),
                               (uint32_t)input_info.st_size, in_file);
    assert(bytes_read <= (uint32_t)input_info.st_size);


    buff[bytes_read] = '\0';

    *size = bytes_read;

    return buff;
}
