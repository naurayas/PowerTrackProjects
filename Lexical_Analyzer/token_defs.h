#ifndef TOKEN_DEFS_H
#define TOKEN_DEFS_H

#define MAX_KEYWORDS   20
#define MAX_TOKEN_SIZE 100

/* All token categories the lexer can produce. */
typedef enum
{
    KEYWORD,           /* Reserved words: int, if, while, ... */
    OPERATOR,          /* Single- and multi-char operators: +, ==, &&, ... */
    SPECIAL_CHARACTER, /* Delimiters: ; { } ( ) [ ] , . */
    CONSTANT,          /* Numeric literals and string literals */
    IDENTIFIER,        /* Variable and function names */
    PREPROCESSOR,      /* Directives: #include, #define, ... */
    UNKNOWN,           /* Unrecognised characters */
    TOKEN_EOF          /* End-of-file sentinel */
} TokenType;

/* A single token: its raw text and its category. */
typedef struct
{
    char      lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

#endif /* TOKEN_DEFS_H */