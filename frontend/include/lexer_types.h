#pragma once

#include <stdint.h>

enum State {
    S_ID        , // идент
    S_NUM       , // число
    S_FLOAT     , // флоат число
    S_PLUS      , // +
    S_MINUS     , // -
    S_STAR      , // *
    S_SLASH     , // /
    S_EQ        , // =
    S_EQ2       , // ==
    S_NE        , // !=
    S_AND       , // &&
    S_OR        , // ||
    S_LT        , // <
    S_GT        , // >
    S_LT_EQ     , // <=
    S_GT_EQ     , // >=
    S_LPAREN    , // (
    S_RPAREN    , // )
    S_LBRACE    , // {
    S_RBRACE    , // }
    S_COMMA     , // ,
    S_SEMIC     , // ;
    S_BANG      , // !
    S_AMP       , // &
    S_PIPE      , // |
    S_COMMENT   , // коммент //
    S_COMMENT_ML, // коммент /*
    S_DOT_FLOAT , // точка во флоат числе

    S_START     , // начальное состояние

    S_W         ,
    S_WH        ,
    S_WHI       ,
    S_WHIL      ,
    S_WHILE     ,

    S_F         ,
    S_FO        ,
    S_FOR       ,

    S_D         ,
    S_DO        ,

    S_I         ,
    S_IF        ,

    S_E         ,
    S_EL        ,
    S_ELS       ,
    S_ELSE      ,
    S_ELSE_SPACE,
    S_ELSE_I    ,
    S_ELSE_IF   ,

    S_M         ,
    S_MA        ,
    S_MAI       ,
    S_MAIN      ,

    S_ERROR     ,
};

enum TokenType {
    IDENT       ,
    INT         ,
    FLOAT       ,
    PLUS        ,
    MINUS       ,
    MULTIPLY    ,
    DIVIDE      ,
    ASSIGN      ,
    EQUAL       ,
    NOT_EQUAL   ,
    AND         ,
    OR          ,
    LESS        ,
    GREATER     ,
    LESS_EQ     ,
    GREATER_EQ  ,
    LPAREN      ,
    RPAREN      ,
    LBRACE      ,
    RBRACE      ,
    COMMA       ,
    SEMICOLON   ,
    DOT         ,
    WHILE   = 33,
    FOR     = 36,
    DO      = 38,
    IF      = 40,
    ELSE    = 44,
    ELSE_IF = 47,
    MAIN    = 51,
    FUNC_DEF    ,
    FUNC_CALL   ,
    END_OF_FILE ,
    KEYWORD     ,
    ERROR       ,
};

typedef union {
    char*  str;
    double dubl;
    int    intg;
} value_t;

typedef struct {
    uint32_t line;
    uint32_t column;
} location_t;

typedef struct {
    TokenType  type;
    value_t    value;
    location_t location;
} token_t;
