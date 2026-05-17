#pragma once

#include <cinttypes>
#include <stdio.h>
#include <string.h>
#include <hashtable.h>

#include "list.h"
#include "hash_table_types.h"

typedef uint32_t (*hash_func_t) (elem_t elem); 

extern const uint32_t TABLE_SIZE;
extern const uint32_t MAX_NAME_LEN;

list_t **hash_table_create();
void     hash_table_destroy(list_t **hash_table);
uint32_t rol_hash_5(elem_t elem);
void     hash_table_insert(list_t **hash_table, elem_t elem);
bool     hash_table_search(list_t **hash_table, elem_t elem);
