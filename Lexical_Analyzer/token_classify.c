#include <string.h>
#include <ctype.h>
#include "token_classify.h"

static const char *keywords[MAX_KEYWORDS] =
{
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char *operators      = "+-*/%=!<>|&";
static const char *specialChars   = ",;{}()[].";

int isKeyword(const char *str)
{
    for (int i = 0; i < MAX_KEYWORDS; i++)
    {
        if (keywords[i] && strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

/* Operators are at most 2 characters; only the first char is checked. */
int isOperator(const char *str)
{
    return strlen(str) <= 2 && strchr(operators, str[0]) != NULL;
}

int isSpecialCharacter(char ch)
{
    return strchr(specialChars, ch) != NULL;
}

int isIdentifier(const char *str)
{
    if (!str || str[0] == '\0') return 0;
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    for (int i = 1; str[i]; i++)
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    return 1;
}

int isConstant(const char *str)
{
    if (!str || str[0] == '\0') return 0;

    int dots = 0, digits = 0;
    for (int i = 0; str[i]; i++)
    {
        if (isdigit(str[i]))      { digits++; }
        else if (str[i] == '.')   { if (++dots > 1) return 0; }
        else                       return 0;
    }
    return digits > 0;
}

void categorizeToken(Token *token)
{
    if      (isKeyword(token->lexeme))              token->type = KEYWORD;
    else if (isConstant(token->lexeme))             token->type = CONSTANT;
    else if (isIdentifier(token->lexeme))           token->type = IDENTIFIER;
    else if (isOperator(token->lexeme))             token->type = OPERATOR;
    else if (isSpecialCharacter(token->lexeme[0]))  token->type = SPECIAL_CHARACTER;
    else                                             token->type = UNKNOWN;
}