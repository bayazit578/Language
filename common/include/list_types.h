#pragma once

#include <stdint.h>

typedef struct {
            void*    value;
            uint32_t next;
            uint32_t prev;
            } list_node_t;

typedef struct {
            int          free;
            list_node_t* contents;
            uint32_t     size;
            uint32_t     capacity;
            } list_t;
 
