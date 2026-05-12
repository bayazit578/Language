#pragma once 

typedef enum {
    FUNC_TYPE ,
    VAR_TYPE  ,
    PARAM_TYPE,
} IdentType;

typedef struct {
    IdentType type;
    char*     name;
} symbol_t;
