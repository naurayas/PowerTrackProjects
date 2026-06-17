# Lexical Analyzer for C Programming Language

A production-ready lexical analyzer (tokenizer) for C source code, built with comprehensive documentation, robust error handling, and support for advanced language features.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Supported Token Types](#supported-token-types)
- [Examples](#examples)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## 🔍 Overview

This lexical analyzer is a fundamental component of a compiler toolchain. It reads C source code files and breaks them down into tokens, which are the basic building blocks for syntax analysis and compilation. The analyzer recognizes all standard C language constructs including keywords, operators, identifiers, constants, and preprocessor directives.

## ✨ Features

### Core Functionality
- **Keyword Recognition**: Identifies all standard C keywords (`int`, `if`, `while`, `return`, etc.)
- **Operator Support**: Handles both single-character (`+`, `-`, `*`) and multi-character operators (`==`, `!=`, `&&`, `||`, `<=`, `>=`, `++`, `--`)
- **Identifier Validation**: Recognizes valid C identifiers (variables, function names)
- **Constant Detection**: Supports integer literals, floating-point literals, and string literals
- **Preprocessor Directives**: Recognizes directives like `#include`, `#define`, `#ifdef`
- **Special Character Handling**: Identifies delimiters and punctuation (`;`, `{`, `}`, `[`, `]`, `,`, etc.)

### Advanced Features
- **Comment Removal**: Automatically skips single-line comments (`//`) and multi-line comments (`/* */`)
- **Multi-Character Operators**: Lookahead logic to correctly tokenize `==`, `!=`, `&&`, `||`
- **Line Number Tracking**: Maintains current line number for error reporting
- **String Literal Support**: Properly handles quoted strings
- **Robust Error Handling**: Graceful handling of invalid input

### Code Quality
- **Doxygen Documentation**: Comprehensive API documentation for all functions
- **Clean Code Style**: Allman brace style, 4-space indentation, descriptive naming
- **Production-Ready**: Proper memory management, buffer overflow protection
- **Modular Design**: Clear separation between lexer logic and driver program

## 📦 Requirements

- **Compiler**: GCC 4.8 or later (or any C99-compliant compiler)
- **Operating System**: Linux, macOS, or Windows (with MinGW/Cygwin)
- **Make**: GNU Make 3.8 or later

## 🛠️ Installation

### Quick Start

```bash
# Clone or download the repository
cd lexical-analyzer

# Build the project
make

# Run the analyzer on the test file
make run
```

### Manual Build

If you prefer not to use `make`:

```bash
gcc -Wall -Wextra -g -std=c99 -c main.c -o main.o
gcc -Wall -Wextra -g -std=c99 -c lexer.c -o lexer.o
gcc -o lexical_analyzer main.o lexer.o
```

## 🚀 Usage

### Basic Usage

```bash
./lexical_analyzer <source_file.c>
```

### Examples

Analyze the provided test file:
```bash
./lexical_analyzer test.c
```

Analyze your own C source file:
```bash
./lexical_analyzer my_program.c
```

### Using Make Targets

The makefile provides several convenient targets:

```bash
make              # Build the project
make run          # Build and run with test.c
make clean        # Remove build artifacts
make rebuild      # Clean and rebuild
make help         # Display help information
```

To run with a custom file:
```bash
make run-custom FILE=my_program.c
```

### Sample Output

```
=========================================
Lexical Analyzer - Token Report
=========================================
File: test.c
Status: Parsing completed successfully
=========================================

TOKEN TYPE           : LEXEME
----------------------------------------
Preprocessor         : #include
Operator             : <
Identifier           : stdio
Special Character    : .
Identifier           : h
Operator             : >
Keyword              : int
Identifier           : main
Special Character    : (
Keyword              : void
Special Character    : )
Special Character    : {
Keyword              : int
Identifier           : x
Operator             : =
Literal              : 42
Special Character    : ;
Keyword              : return
Literal              : 0
Special Character    : ;
Special Character    : }
=========================================
Total Tokens Processed: 23
=========================================
```

## 📁 File Structure

```
lexical-analyzer/
├── lexer.h              # Header file with token definitions and function prototypes
├── lexer.c              # Core lexical analyzer implementation
├── main.c               # Driver program for the analyzer
├── makefile             # Build automation script
├── test.c               # Comprehensive test file demonstrating all features
└── README.md            # This documentation file
```

### File Descriptions

- **lexer.h**: Defines the `Token` structure, `TokenType` enumeration, and function prototypes. Contains Doxygen documentation for the API.

- **lexer.c**: Implements the lexical analysis logic including:
  - `initializeLexer()`: Opens and prepares the source file
  - `getNextToken()`: Main tokenization function
  - `categorizeToken()`: Determines token types
  - Helper functions for validation (`isKeyword()`, `isOperator()`, etc.)

- **main.c**: Entry point that:
  - Validates command-line arguments
  - Initializes the lexer
  - Processes and displays all tokens

- **makefile**: Automates compilation with:
  - Compiler flags for warnings and debugging
  - Dependency tracking
  - Clean, build, and run targets

- **test.c**: Comprehensive test suite covering:
  - Preprocessor directives
  - Comments
  - All operator types
  - Keywords and identifiers
  - Various constant types
  - String literals

## 🎯 Supported Token Types

The analyzer categorizes tokens into the following types:

| Token Type | Description | Examples |
|------------|-------------|----------|
| **KEYWORD** | Reserved C keywords | `int`, `if`, `while`, `return`, `struct` |
| **OPERATOR** | Arithmetic and logical operators | `+`, `-`, `==`, `!=`, `&&`, `||` |
| **SPECIAL_CHARACTER** | Delimiters and punctuation | `;`, `{`, `}`, `(`, `)`, `,`, `.` |
| **CONSTANT** | Numeric and string literals | `42`, `3.14`, `"Hello"` |
| **IDENTIFIER** | Variable and function names | `counter`, `calculateSum`, `_temp` |
| **PREPROCESSOR** | Preprocessor directives | `#include`, `#define`, `#ifdef` |
| **UNKNOWN** | Unrecognized tokens | Invalid characters |
| **TOKEN_EOF** | End of file marker | (End of file) |

## 📚 Examples

### Example 1: Simple Program

**Input (simple.c):**
```c
#include <stdio.h>

int main(void)
{
    int x = 10;
    if (x == 10)
    {
        return 0;
    }
    return 1;
}
```

**Command:**
```bash
./lexical_analyzer simple.c
```

### Example 2: Operators Test

**Input (operators.c):**
```c
int test(int a, int b)
{
    if (a == b || a >= b)
        return 1;
    if (a != b && a <= b)
        return -1;
    return 0;
}
```

This will correctly identify multi-character operators like `==`, `||`, `>=`, `!=`, `&&`, and `<=`.

### Example 3: Preprocessor Directives

**Input (preprocessor.c):**
```c
#include <stdio.h>
#define MAX 100
#define MIN 0

#ifdef DEBUG
    #define LOG(x) printf(x)
#endif
```

The analyzer will recognize all `#include` and `#define` directives as PREPROCESSOR tokens.

## 🔧 Technical Details

### Tokenization Algorithm

The lexical analyzer uses a two-phase approach:

1. **Buffer Management Phase**:
   - Reads lines from the source file into a buffer
   - Skips whitespace and comments
   - Tracks line numbers for error reporting

2. **Token Extraction Phase**:
   - Identifies token type based on first character(s)
   - Uses lookahead for multi-character operators
   - Categorizes tokens appropriately

### Multi-Character Operator Handling

The analyzer implements lookahead logic to distinguish between:
- Single-character operators: `=`, `<`, `>`, `!`, `&`, `|`
- Two-character operators: `==`, `<=`, `>=`, `!=`, `&&`, `||`, `++`, `--`

### Comment Handling

The analyzer handles both types of C comments:
- **Single-line comments** (`//`): Detected and skipped during the buffer management phase. All characters from `//` to the end of the line are discarded.
- **Multi-line comments** (`/* */`): Detected and skipped across multiple lines. The analyzer reads through lines until it finds the closing `*/` sequence.

### Identifier Validation Rules

Valid identifiers must:
- Start with a letter (a-z, A-Z) or underscore (_)
- Contain only letters, digits, or underscores
- Not be a reserved keyword

## 🤝 Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Follow the existing code style (Allman braces, 4-space indentation)
4. Add Doxygen comments for new functions
5. Test thoroughly with various C files
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to the branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

### Code Style Guidelines

- Use 4 spaces for indentation (no tabs)
- Use Allman brace style (opening brace on new line)
- Add blank lines between logical code blocks
- Use descriptive variable names
- Document all functions with Doxygen comments

## 📄 License

This project is provided as-is for educational and commercial purposes. Feel free to modify and distribute as needed.

## 📞 Support

For questions, issues, or feature requests, please open an issue on the repository.

---

**Author**: Shahad K  
**Date**: February 2026  
**Version**: 1.0.0