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
    S_LT         = 12,  // <
    S_BANG       = 13,  // !
    S_NE         = 14,  // !=
    S_AMP        = 15,  // &
    S_AND        = 16,  // &&
    S_PIPE       = 17,  // |
    S_OR         = 18,  // ||
    S_SEMIC      = 19,  // ;
    S_COMMA      = 20,  // ,
    S_LPAREN     = 21,  // (
    S_RPAREN     = 22,  // )
    S_LBRACE     = 23,  // {
    S_RBRACE     = 24,  // }
    S_COMMENT    = 25,  // коммент //
    S_COMMENT_ML = 26,  // коммент /*

    S_W          = 27,
    S_WH         = 28,
    S_WHI        = 29,
    S_WHIL       = 30,
    S_WHILE      = 31,

    S_F          = 32,
    S_FO         = 33,
    S_FOR        = 34,

    S_D          = 35,
    S_DO         = 36,

    S_I          = 37,
    S_IF         = 38,

    S_E          = 39,
    S_EL         = 40,
    S_ELS        = 41,
    S_ELSE       = 42,
    S_ELSE_SPACE = 43,
    S_ELSE_I     = 44,
    S_ELSE_IF    = 45,

    S_ERROR      = 46, // ошибка
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
