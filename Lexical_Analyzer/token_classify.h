#ifndef TOKEN_CLASSIFY_H
#define TOKEN_CLASSIFY_H

#include "token_defs.h"

/* Returns 1 if str is a reserved C keyword, 0 otherwise. */
int isKeyword(const char *str);

/* Returns 1 if str (≤2 chars) is a recognised operator, 0 otherwise. */
int isOperator(const char *str);

/* Returns 1 if ch is a special delimiter character, 0 otherwise. */
int isSpecialCharacter(char ch);

/* Returns 1 if str is a valid integer or floating-point constant, 0 otherwise. */
int isConstant(const char *str);

/* Returns 1 if str is a valid C identifier, 0 otherwise. */
int isIdentifier(const char *str);

/* Sets token->type by testing the lexeme against all categories in order. */
void categorizeToken(Token *token);

#endif /* TOKEN_CLASSIFY_H */