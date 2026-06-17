#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer_core.h"
#include "token_classify.h"

#define BUFFER_SIZE  2048
#define MAX_DEPTH    256   /* maximum nesting depth for delimiter stack */

static FILE *sourceFile        = NULL;
static char  lineBuffer[BUFFER_SIZE];
static char *currentPosition   = NULL;
static int   currentLineNumber = 1;

/* -------------------------------------------------------------------------
 * Initialisation / cleanup
 * ------------------------------------------------------------------------- */

int initializeLexer(const char *filename)
{
    sourceFile = fopen(filename, "r");
    if (sourceFile == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return 0;
    }
    currentLineNumber = 1;
    currentPosition   = NULL;
    return 1;
}

void closeLexer(void)
{
    if (sourceFile != NULL)
    {
        fclose(sourceFile);
        sourceFile = NULL;
    }
}

/* -------------------------------------------------------------------------
 * Internal helpers for getNextToken
 * ------------------------------------------------------------------------- */

/* Reads the next source line into lineBuffer. Returns 0 at EOF. */
static int refillBuffer(Token *eofToken)
{
    if (fgets(lineBuffer, BUFFER_SIZE, sourceFile) == NULL)
    {
        eofToken->type      = TOKEN_EOF;
        eofToken->lexeme[0] = '\0';
        return 0;
    }
    currentPosition = lineBuffer;
    currentLineNumber++;
    return 1;
}

/* Skips over a block comment. Returns 0 if EOF is reached inside it. */
static int skipBlockComment(Token *eofToken)
{
    currentPosition += 2; /* skip the opening / and * */
    while (1)
    {
        if (*currentPosition == '\0' || *currentPosition == '\n')
        {
            if (!refillBuffer(eofToken)) return 0;
            continue;
        }
        if (*currentPosition == '*' && *(currentPosition + 1) == '/')
        {
            currentPosition += 2; /* skip the closing * and / */
            return 1;
        }
        currentPosition++;
    }
}

/* -------------------------------------------------------------------------
 * Token extractors (each handles exactly one token shape)
 * ------------------------------------------------------------------------- */

static Token extractPreprocessor(void)
{
    Token token;
    int i = 0;
    while (*currentPosition != ' '  && *currentPosition != '\t' &&
           *currentPosition != '\n' && *currentPosition != '\0' &&
           i < MAX_TOKEN_SIZE - 1)
    {
        token.lexeme[i++] = *currentPosition++;
    }
    token.lexeme[i] = '\0';
    token.type = PREPROCESSOR;
    return token;
}

static Token extractStringLiteral(void)
{
    Token token;
    int i = 0;
    token.lexeme[i++] = *currentPosition++; /* opening quote */
    while (*currentPosition != '"'  && *currentPosition != '\n' &&
           *currentPosition != '\0' && i < MAX_TOKEN_SIZE - 1)
    {
        token.lexeme[i++] = *currentPosition++;
    }
    if (*currentPosition == '"')
        token.lexeme[i++] = *currentPosition++;
    token.lexeme[i] = '\0';
    token.type = CONSTANT;
    return token;
}

static Token extractWord(void)
{
    Token token;
    int i = 0;
    while ((isalnum(*currentPosition) || *currentPosition == '_') &&
           i < MAX_TOKEN_SIZE - 1)
    {
        token.lexeme[i++] = *currentPosition++;
    }
    token.lexeme[i] = '\0';
    categorizeToken(&token); /* resolves KEYWORD vs IDENTIFIER */
    return token;
}

static Token extractNumber(void)
{
    Token token;
    int i = 0;
    while (isdigit(*currentPosition) && i < MAX_TOKEN_SIZE - 1)
        token.lexeme[i++] = *currentPosition++;
    if (*currentPosition == '.') /* optional fractional part */
    {
        token.lexeme[i++] = *currentPosition++;
        while (isdigit(*currentPosition) && i < MAX_TOKEN_SIZE - 1)
            token.lexeme[i++] = *currentPosition++;
    }
    token.lexeme[i] = '\0';
    categorizeToken(&token);
    return token;
}

/* Uses one character of lookahead to detect two-character operators. */
static Token extractOperator(void)
{
    Token token;
    char  next = *(currentPosition + 1);
    token.lexeme[0] = *currentPosition;

    int twoChar = (next != '\0' && next != '\n') &&
        ((*currentPosition == '=' && next == '=') ||
         (*currentPosition == '!' && next == '=') ||
         (*currentPosition == '<' && next == '=') ||
         (*currentPosition == '>' && next == '=') ||
         (*currentPosition == '&' && next == '&') ||
         (*currentPosition == '|' && next == '|') ||
         (*currentPosition == '+' && next == '+') ||
         (*currentPosition == '-' && next == '-'));

    if (twoChar)
    {
        token.lexeme[1] = next;
        token.lexeme[2] = '\0';
        currentPosition += 2;
    }
    else
    {
        token.lexeme[1] = '\0';
        currentPosition++;
    }
    token.type = OPERATOR;
    return token;
}

/* -------------------------------------------------------------------------
 * Public tokeniser
 * ------------------------------------------------------------------------- */

Token getNextToken(void)
{
    Token token;
    token.type = UNKNOWN;

    while (1)
    {
        /* Reload buffer when current line is used up. */
        if (currentPosition == NULL || *currentPosition == '\0' || *currentPosition == '\n')
        {
            if (!refillBuffer(&token)) return token; /* EOF */
        }

        while (*currentPosition == ' ' || *currentPosition == '\t')
            currentPosition++;

        if (*currentPosition == '/')
        {
            if (*(currentPosition + 1) == '/')
            {
                /* Single-line comment: skip to end of line. */
                while (*currentPosition != '\n' && *currentPosition != '\0')
                    currentPosition++;
                continue;
            }
            else if (*(currentPosition + 1) == '*')
            {
                if (!skipBlockComment(&token)) return token;
                continue;
            }
        }

        if (*currentPosition == '\n' || *currentPosition == '\0') continue;
        break;
    }

    if (*currentPosition == '#')                              return extractPreprocessor();
    if (*currentPosition == '"')                              return extractStringLiteral();
    if (isalpha(*currentPosition) || *currentPosition == '_') return extractWord();
    if (isdigit(*currentPosition))                            return extractNumber();
    if (strchr("+-*/%=!<>|&", *currentPosition))             return extractOperator();

    /* Single special character or unknown. */
    token.lexeme[0] = *currentPosition++;
    token.lexeme[1] = '\0';
    categorizeToken(&token);
    return token;
}

/* -------------------------------------------------------------------------
 * Delimiter balance checker
 *
 * Uses a small stack. Each entry records the opening character and the
 * line on which it appeared. On a closing delimiter we verify it matches
 * the top of the stack; on EOF we report anything left open.
 * Comments and string literals are skipped so their brackets are ignored.
 * ------------------------------------------------------------------------- */

/* Returns the expected closing delimiter for an opener, or 0 if not an opener. */
static char matchingClose(char ch)
{
    if (ch == '(') return ')';
    if (ch == '[') return ']';
    if (ch == '{') return '}';
    return 0;
}

/* Returns 1 if ch is a closing delimiter. */
static int isCloser(char ch)
{
    return ch == ')' || ch == ']' || ch == '}';
}

int checkBalancedDelimiters(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: Could not open file '%s'\n", filename);
        return 1;
    }

    /* Stack entries: the opening character and its source line. */
    char openChars[MAX_DEPTH];
    int  openLines[MAX_DEPTH];
    int  top    = 0;   /* stack pointer */
    int  errors = 0;
    int  line   = 1;
    int  ch;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
        {
            line++;
            continue;
        }

        /* Skip single-line comments. */
        if (ch == '/')
        {
            int next = fgetc(fp);
            if (next == '/')
            {
                while ((ch = fgetc(fp)) != EOF && ch != '\n') {}
                if (ch == '\n') line++;
                continue;
            }
            /* Skip block comments. */
            if (next == '*')
            {
                int prev = 0;
                while ((ch = fgetc(fp)) != EOF)
                {
                    if (ch == '\n') line++;
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                continue;
            }
            /* Not a comment: put next back by re-processing it. */
            ungetc(next, fp);
            continue;
        }

        /* Skip string literals so their brackets don't confuse the checker. */
        if (ch == '"')
        {
            while ((ch = fgetc(fp)) != EOF && ch != '"')
            {
                if (ch == '\\') fgetc(fp); /* skip escaped character */
                if (ch == '\n') line++;
            }
            continue;
        }

        /* Opening delimiter: push onto stack. */
        if (matchingClose((char)ch))
        {
            if (top < MAX_DEPTH)
            {
                openChars[top] = (char)ch;
                openLines[top] = line;
                top++;
            }
            continue;
        }

        /* Closing delimiter: check against top of stack. */
        if (isCloser((char)ch))
        {
            if (top == 0)
            {
                printf("Error (line %d): unexpected '%c' with no matching opener.\n",
                       line, ch);
                errors++;
            }
            else if (matchingClose(openChars[top - 1]) != (char)ch)
            {
                printf("Error (line %d): '%c' does not match '%c' opened on line %d.\n",
                       line, ch, openChars[top - 1], openLines[top - 1]);
                errors++;
                top--; /* consume the mismatched opener and keep going */
            }
            else
            {
                top--; /* matched: pop */
            }
        }
    }

    /* Anything left on the stack was never closed. */
    while (top > 0)
    {
        top--;
        printf("Error (line %d): '%c' was never closed.\n",
               openLines[top], openChars[top]);
        errors++;
    }

    fclose(fp);
    return errors > 0 ? 1 : 0;
}