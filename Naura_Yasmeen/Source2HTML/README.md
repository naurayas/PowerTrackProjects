# Source to HTML Converter
This project is a command-line based converter in C that converts C source code into a syntax-highlighted HTML document. The project parses the souce file using an event-driven state machine and generates an HTML file with CSS-based syntax highlighting.

## Features
- Converts C source code into HTML
- Highlights:
  - Preprocessor directives
  - Header files
  - Reserved keywords
  - Numeric constants
  - String literals
  - Character literals
  - Single-line comments
  - Multi-line comments
- Preserves the original source code formatting

## Working
The converter processes the input C source file in 3 steps:

1. Parsing: Reads the source file character by character using a finite state machine to identify different token types such as keywords, comments, strings, header files and numeric constants.
2. Event Generation: Generates events for each recognized token and classifies them.
3. HTML conversion: Produces a syntax-highlighted HTML file based on the token and CSS class.

## Technologies Used
- C programming
- File handling
- Finite State Machine
- HTML
- CSS

## Project Structure
```bash
.
├── makefile
├── s2html_conv.c
├── s2html_conv.h
├── s2html_event.c
├── s2html_event.h
├── s2html_main.c
├── styles.css
├── test.c
└── test.c.html
```

## Build Instructions
Compile the project using
```bash
make
```
Clean the object files and executable:
```bash
make clean
```

## Usage
Convert a source file:
```bash
./html test.c
```
This generates an output file. Open the generated HTML file in  any web browser to view the syntax-highlighted source code.
