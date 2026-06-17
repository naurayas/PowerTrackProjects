#include <stdio.h>
#include "lexer.h"

/* Maps TokenType enum values to display strings (order must match the enum). */
static const char *tokenTypeNames[] =
{
    "Keyword",
    "Operator",
    "Special Character",
    "Literal",
    "Identifier",
    "Preprocessor",
    "Unknown",
    "End of File"
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    /* Check delimiter balance before tokenising.
       Errors are printed inside checkBalancedDelimiters. */
    if (checkBalancedDelimiters(filename) != 0)
    {
        printf("Fix the errors above before continuing.\n");
        return 1;
    }

    if (!initializeLexer(filename))
    {
        printf("Error: Failed to initialize lexer with file '%s'\n", filename);
        return 1;
    }

    printf("========================================\n");
    printf("Lexical Analyzer - Token Report\n");
    printf("========================================\n");
    printf("File: %s\n", filename);
    printf("========================================\n\n");
    printf("%-20s : %s\n", "TOKEN TYPE", "LEXEME");
    printf("----------------------------------------\n");

    Token token;
    int tokenCount = 0;

    while ((token = getNextToken()).type != TOKEN_EOF)
    {
        printf("%-20s : %s\n", tokenTypeNames[token.type], token.lexeme);
        tokenCount++;
    }

    printf("========================================\n");
    printf("Total Tokens Processed: %d\n", tokenCount);
    printf("========================================\n");

    closeLexer();
    return 0;
}