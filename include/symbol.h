#pragma once

#include <stdint.h> 
#include <stdlib.h>

typedef enum {
    FUNC_TYPE ,
    VAR_TYPE  ,
    PARAM_TYPE,
} IdentType;

typedef struct {
    
} vars_t;

typedef struct {
    char*    func_name;
    uint32_t argc;
} func_t;
