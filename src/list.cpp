#include "list.h"

static void* const POISON = (void*)(uintptr_t)0xDEFACED;

static const int MAX_CMD_LEN = 500;

list_t* list_create() {
    list_t* cnucok = (list_t*)calloc(1, sizeof(list_t));
    if (cnucok == NULL) {
        fprintf(stderr, RED "list_t allocation error." RESET);
        return NULL;
    }

    cnucok->contents = (node_t*)calloc(1, sizeof(node_t));
    cnucok->free = -1;
    cnucok->size = 1;
    cnucok->capacity = 1;

    node_t* node = cnucok->contents;
    node[0].value = 0;
    node[0].next = 0;
    node[0].prev = 0;

    get_free(cnucok);

    return cnucok;
}

void list_destroy(list_t* cnucok) {
    for (uint32_t i = 0; i < cnucok->capacity; i++) {
        if (!cnucok->contents[i].value) {
            free(cnucok->contents[i].value);
        }
    }

    free(cnucok->contents);
    cnucok->capacity = 0;
    cnucok->size = 0;
    cnucok->free = 0;
    free(cnucok);
}

void list_insert_after(list_t* cnucok, uint32_t ind, void* value) {
    uint32_t new_next_ind = cnucok->size;
    cnucok->size++;

    if (cnucok->size >= cnucok->capacity) {
        cnucok->capacity *= 2;
        cnucok->contents = (node_t*)realloc(cnucok->contents,
                                            cnucok->capacity * sizeof(node_t));
        for (uint32_t i = cnucok->size; i < cnucok->capacity; i++) {
            cnucok->contents[i].value = POISON;
            cnucok->contents[i].next = i + 1;
            cnucok->contents[i].prev = i - 1;
            if (i + 1 >= cnucok->capacity)
                cnucok->contents[i].next = cnucok->size;

            if (i - 1 < cnucok->size)
                cnucok->contents[i].prev = cnucok->capacity - 1;
        }
    }

    uint32_t old_next_ind = cnucok->contents[ind].next;

    cnucok->contents[new_next_ind].value = value;
    cnucok->contents[new_next_ind].next = 
        cnucok->contents[ind].next;
    cnucok->contents[new_next_ind].prev = 
        cnucok->contents[old_next_ind].prev;

    cnucok->contents[ind].next = new_next_ind;
    cnucok->contents[old_next_ind].prev = new_next_ind;
   
    get_free(cnucok);

    if (cnucok->capacity != cnucok->size) {
        uint32_t last_ind = cnucok->capacity - 1;
        cnucok->contents[last_ind].next = cnucok->free;
    }
}

void* list_erase(list_t* cnucok, uint32_t ind) {
    void* value = cnucok->contents[ind].value;

    cnucok->contents[ind].value = POISON;
    uint32_t ind_prev = cnucok->contents[ind].prev;
    uint32_t ind_next = cnucok->contents[ind].next;

    cnucok->contents[ind_prev].next = ind_next;
    cnucok->contents[ind_next].prev = ind_prev;

    uint32_t first_empty_ind = cnucok->free;
    uint32_t last_empty_ind = cnucok->contents[first_empty_ind].prev;
    cnucok->contents[ind].next = cnucok->free;
    cnucok->contents[ind].prev = last_empty_ind;

    cnucok->contents[first_empty_ind].prev = ind;
    cnucok->contents[last_empty_ind].next = ind;

    cnucok->free = ind;

    get_free(cnucok);
    return value;
}

void list_push_front(list_t* cnucok, void* value) {
    list_insert_after(cnucok, 0, value);
}

void list_push_back(list_t* cnucok, void* value) {
    list_insert_after(cnucok, cnucok->contents[0].prev, value);
}

void* list_pop_front(list_t* cnucok) {
    void* value = list_erase(cnucok, cnucok->contents[0].next);
    return value;
}

void* list_pop_back(list_t* cnucok) {
    void* value = list_erase(cnucok, cnucok->contents[0].prev);
    return value;
}

void get_free(list_t* cnucok) {
    uint32_t size = cnucok->size;
    uint32_t capacity = cnucok->capacity;

    if (size == capacity)
        cnucok->free = -1;
    else
        cnucok->free = size;
}

#define CLR_RED_LIGHT_   "\"#FFB0B0\""
#define CLR_GREEN_LIGHT_ "\"#B0FFB0\""
#define CLR_BLUE_LIGHT_  "\"#B0B0FF\""

#define CLR_RED_BOLD_    "\"#FF0000\""
#define CLR_GREEN_BOLD_  "\"#03c03c\""
#define CLR_BLUE_BOLD_   "\"#0000FF\""

void merge_list_dumps(list_t* cnucok, const char* filename, 
                      uint32_t cell_number, uint32_t bucket_number, 
                      FILE* html_out) {
    fprintf(html_out,
        "<h2>Cell %u, bucket %u</h2>\n",
        cell_number, bucket_number
    );

    list_dump(cnucok, filename, cell_number, bucket_number);

    fprintf(html_out,
        "<img src=\"%s%u_%u.png\">\n",
        filename, cell_number, bucket_number
    );
}

void list_dump(list_t* cnucok, const char* filename, uint32_t cell_number, 
               uint32_t bucket_number) {
    if (!cnucok || !filename) return;

    char full_filename[256];
    snprintf(full_filename, sizeof(full_filename), "dot_files/%s%u_%u.dot", 
             filename, cell_number, bucket_number);
    FILE* dot_file = fopen(full_filename, "w");

    if (!dot_file) {
        fprintf(stderr, RED "Cannot open file %s for writing\n" RESET, 
                full_filename);
        return;
    }

    fprintf(dot_file, 
        "digraph {\n rankdir=LR;\nsplines=ortho;\n" 
        "nodesep=0.9;\nranksep=0.75;\n"
        "node [fontname=\"Fira Mono\","
        "color=" CLR_RED_BOLD_ ","
        "style=\"filled\","
        "shape=tripleoctagon,"
        "fillcolor=" CLR_RED_LIGHT_ ","
        "];\n"
    );

    fprintf(dot_file,
        "node_%u[shape=record,"
        "label=\"ind: 0 | data: NULL | { prev: %u | next: %u } \","
        "color=" CLR_BLUE_BOLD_ ","
        "style=\"filled,bold,rounded\","
        "fillcolor=" CLR_BLUE_LIGHT_ "];\n",
        0, cnucok->contents[0].prev,
        cnucok->contents[0].next
    );

    elem_t* value = NULL; 
    int node_ind = cnucok->contents[0].next;
    while (node_ind != 0) {
        value = (elem_t*)cnucok->contents[node_ind].value;
        fprintf(dot_file,
            "node_%u[shape=record,"
            "label=\" ind: %u | data: %s | { prev: %u | next: %u } \","
            "style=\"filled,rounded\","
            "color=" CLR_GREEN_BOLD_ ","
            "fillcolor=" CLR_GREEN_LIGHT_","
            "constraint=false];\n",  
            node_ind,
            node_ind,
            value->string,
            cnucok->contents[node_ind].prev,
            cnucok->contents[node_ind].next
        );
        node_ind = cnucok->contents[node_ind].next; 
    }

    // free
    if (cnucok->capacity > cnucok->size) {
        node_ind = cnucok->free;
        do {
            fprintf(dot_file,
                "node_%u[shape=record,"
                "label=\" ind: %u | data: POISON | { prev: %u | next: %u } \","
                "style=\"filled,rounded\","
                "color=" CLR_RED_BOLD_ ","
                "fillcolor=" CLR_RED_LIGHT_","
                "constraint=false];\n",  
                node_ind,
                node_ind,
                cnucok->contents[node_ind].prev,
                cnucok->contents[node_ind].next
            );
            node_ind = cnucok->contents[node_ind].next;
        } while ((int)node_ind != cnucok->free);
    }

    // used
    node_ind = 0;

    do {
        fprintf(dot_file,
            "node_%u -> node_%u [weight=1000, style=solid];\n",
            node_ind,
            cnucok->contents[node_ind].next
        );
        node_ind = cnucok->contents[node_ind].next;
    } while (node_ind != 0);

    fprintf(dot_file,
        "node_head [label=head,color=" CLR_GREEN_BOLD_ ","
        "shape=record,"
        "style=\"filled,rounded\","
        "fillcolor=" CLR_GREEN_LIGHT_ "];\n"
        "node_head -> node_%d [weight=100, style=solid];\n", 
        cnucok->contents[0].next
    );

    // free
    node_ind = cnucok->free;

    if (node_ind != -1) {
        do {
            fprintf(dot_file,
                "node_%u -> node_%u [weight=1000, style=solid];\n",
                node_ind,
                cnucok->contents[node_ind].next
            );
            node_ind = cnucok->contents[node_ind].next;
        } while ((int)node_ind != cnucok->free);

        fprintf(dot_file,
            "node_free [label=free,color=" CLR_RED_BOLD_ ","
            "shape=record,"
            "style=\"filled,rounded\","
            "fillcolor=" CLR_RED_LIGHT_ "];\n"
            "node_free -> node_%d [weight=100, style=solid];\n", 
            cnucok->free
        );
    }

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char cmd_string[MAX_CMD_LEN] = {};
    snprintf(cmd_string, MAX_CMD_LEN,
        "dot -Tpng %s -o photo_dumps/%s%u_%u.png", 
        full_filename, filename, cell_number, bucket_number);

    system(cmd_string);
}
