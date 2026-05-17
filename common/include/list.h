#pragma once

#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>

#include "colors.h"
#include "list_types.h"
#include "hash_table_types.h"

list_t *list_create();
void    list_destroy(list_t *cnucok);
void    list_insert_after(list_t *cnucok, uint32_t ind, 
                          void *value);
void   *list_erase(list_t *cnucok, uint32_t ind);
void    list_push_front(list_t *cnucok, void *value);
void    list_push_back(list_t *cnucok, void *value);
void   *list_pop_front(list_t *cnucok);
void   *list_pop_back(list_t *cnucok);
void    get_free(list_t *cnucok);
void    merge_list_dumps(list_t *cnucok, const char *filename, 
                         uint32_t cell_number, uint32_t bucket_number, 
                         FILE *html_out);
void    list_dump(list_t *cnucok, const char *filename, uint32_t cell_number, 
                  uint32_t bucket_number);
