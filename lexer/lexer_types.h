#pragma once

enum State {
    S_START      = 0,   // начальное состояние
    S_ID         = 1,   // идент
    S_NUM        = 2,   // число
    S_DOT_FLOAT  = 3,   // точка во флоат числе
    S_FLOAT      = 4,   // флоат число
    S_PLUS       = 5,   // +
    S_MINUS      = 6,   // -
    S_STAR       = 7,   // *
    S_SLASH      = 8,   // /
    S_EQ         = 9,   // =
    S_EQ2        = 10,  // ==
    S_GT         = 11,  // >
    S_GT_EQ      = 12,  // >=
    S_LT         = 13,  // <
    S_LT_EQ      = 14,  // <=
    S_BANG       = 15,  // !
    S_NE         = 16,  // !=
    S_AMP        = 17,  // &
    S_AND        = 18,  // &&
    S_PIPE       = 19,  // |
    S_OR         = 20,  // ||
    S_SEMIC      = 21,  // ;
    S_COMMA      = 22,  // ,
    S_LPAREN     = 23,  // (
    S_RPAREN     = 24,  // )
    S_LBRACE     = 25,  // {
    S_RBRACE     = 26,  // }
    S_COMMENT    = 27,  // коммент //
    S_COMMENT_ML = 28,  // коммент /*

    S_W          = 29,
    S_WH         = 30,
    S_WHI        = 31,
    S_WHIL       = 32,
    S_WHILE      = 33,

    S_F          = 34,
    S_FO         = 35,
    S_FOR        = 36,

    S_D          = 37,
    S_DO         = 38,

    S_I          = 39,
    S_IF         = 40,

    S_E          = 41,
    S_EL         = 42,
    S_ELS        = 43,
    S_ELSE       = 44,
    S_ELSE_SPACE = 45,
    S_ELSE_I     = 46,
    S_ELSE_IF    = 47,

    S_ERROR      = 48, // ошибка
};

enum TokenType {
    KEYWORD,
    IDENT,
    INT,
    FLOAT,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    AND,
    OR,
    LESS,
    GREATER,
    LESS_EQ,
    GREATER_EQ,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COMMA,
    SEMICOLON,
    DOT,
    IF,
    ELSE_IF,
    ELSE,
    WHILE,
    FOR,
    DO,
    END_OF_FILE,
    ERROR,
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
