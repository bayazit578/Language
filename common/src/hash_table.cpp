#include "hash_table.h"

const uint32_t TABLE_SIZE = 4000;

const uint32_t MAX_NAME_LEN = 100;

list_t **hash_table_create() {
    list_t **hash_table = (list_t **)calloc(TABLE_SIZE, sizeof(list_t *));
    return hash_table;
}

void hash_table_destroy(list_t **hash_table) {
    for (uint32_t i = 0; i < TABLE_SIZE; i++) {
        if (hash_table[i]) {
            list_destroy(hash_table[i]);
        }
    }
}

uint32_t rol_hash_5(elem_t elem) {
    uint32_t hash = 0;
    for (uint32_t i = 0; i < elem.len; i++) {
        hash = (hash << 1) ^ (hash >> 31) ^ (uint32_t)elem.string[i];
    }

    return hash % TABLE_SIZE;
}

void hash_table_insert(list_t **hash_table, elem_t elem) {
    uint32_t ind = rol_hash_5(elem);

    if (!hash_table[ind]) {
        hash_table[ind] = list_create();
    }

    elem_t* alloced_elem = (elem_t*)calloc(1, sizeof(elem_t));
    *alloced_elem = elem;
    list_push_front(hash_table[ind], alloced_elem);
}

bool hash_table_search(list_t **hash_table, elem_t elem) {
    uint32_t bucket_index = rol_hash_5(elem);

    list_t* list = hash_table[bucket_index];
    if (!list) {
        return false;
    }

    list_node_t* contents = list->contents;
    uint32_t list_ind = contents[0].next;
    while (list_ind != 0) {
        elem_t* stored = (elem_t*)contents[list_ind].value;

        if (stored && !strcmp(stored->string, elem.string)) {
            return true;        
        }
        list_ind = contents[list_ind].next;
    }

    return false;
}
