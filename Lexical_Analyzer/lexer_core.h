#ifndef LEXER_CORE_H
#define LEXER_CORE_H

#include "token_defs.h"

/* Opens filename for tokenisation. Must be called before getNextToken().
   Returns 1 on success, 0 on failure. */
int initializeLexer(const char *filename);

/* Closes the source file and resets internal state. */
void closeLexer(void);

/* Returns the next token from the source file.
   Returns a TOKEN_EOF token when the file is exhausted. */
Token getNextToken(void);

/* Scans filename and checks that every ( ) [ ] { } is matched.
   Prints an error for each unclosed or unexpected delimiter, including
   the line number where the opening delimiter was seen.
   Returns 0 if balanced, 1 if any mismatch is found. */
int checkBalancedDelimiters(const char *filename);

#endif /* LEXER_CORE_H */