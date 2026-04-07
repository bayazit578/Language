#pragma once

enum State {
    S_ID        , // идент
    S_NUM       , // число
    S_DOT_FLOAT , // точка во флоат числе
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

    S_ERROR     ,
};

enum TokenType {
    KEYWORD     ,
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
    IF          ,
    ELSE_IF     ,
    ELSE        ,
    WHILE       ,
    FOR         ,
    DO          ,
    END_OF_FILE ,
    ERROR       ,
};

typedef union {
    char*  str;
    double dubl;
    int    intg;
} Value;

typedef struct {
    uint32_t line;
    uint32_t column;
} Loc;

typedef struct {
    TokenType type;
    Value     value;
    Loc       location;
} Token;
