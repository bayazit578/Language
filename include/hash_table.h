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

void hash_table_dump(const char* output_filename, uint32_t hash_number);
void hash_table_dump_buckets(FILE* list_len_out, uint32_t hash_number, uint32_t word_count);
bool hash_table_search1(elem_t elem, uint32_t hash_number);
void hash_table_insert(elem_t elem, uint32_t hash_number);
