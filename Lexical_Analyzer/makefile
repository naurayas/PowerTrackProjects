# ============================================================================
# Makefile for Lexical Analyzer
# ============================================================================
# This makefile provides targets for building, cleaning, and running the
# lexical analyzer. It includes proper dependency tracking and uses
# standard compiler flags for debugging and warnings.
#
# Author: Senior Embedded C Development Team
# Date: February 2026
# ============================================================================

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -pedantic
LDFLAGS =

# Target executable name
TARGET = lexical_analyzer

# Source files
SOURCES = main.c lexer_core.c token_classify.c
HEADERS = lexer.h token_defs.h lexer_core.h token_classify.h

# Object files (derived from source files)
OBJECTS = $(SOURCES:.c=.o)

# Default test file
TEST_FILE = test.c

# ============================================================================
# Targets
# ============================================================================

# Default target: build the executable
all: $(TARGET)
	@echo "========================================="
	@echo "Build completed successfully!"
	@echo "========================================="
	@echo "Run with: ./$(TARGET) <source_file.c>"
	@echo "Example:  ./$(TARGET) $(TEST_FILE)"
	@echo "========================================="

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program with the default test file
run: $(TARGET)
	@echo "========================================="
	@echo "Running $(TARGET) with $(TEST_FILE)"
	@echo "========================================="
	./$(TARGET) $(TEST_FILE)

# Run with a custom file (usage: make run-custom FILE=myfile.c)
run-custom: $(TARGET)
	@echo "========================================="
	@echo "Running $(TARGET) with $(FILE)"
	@echo "========================================="
	./$(TARGET) $(FILE)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete."

# Remove all generated files including test output
distclean: clean
	@echo "Performing deep clean..."
	rm -f *~ *.bak *.swp
	@echo "Deep clean complete."

# Rebuild from scratch
rebuild: clean all

# Display help information
help:
	@echo "========================================="
	@echo "Lexical Analyzer - Makefile Help"
	@echo "========================================="
	@echo "Available targets:"
	@echo "  all         - Build the lexical analyzer (default)"
	@echo "  run         - Build and run with test.c"
	@echo "  run-custom  - Build and run with custom file (usage: make run-custom FILE=myfile.c)"
	@echo "  clean       - Remove object files and executable"
	@echo "  distclean   - Remove all generated files"
	@echo "  rebuild     - Clean and rebuild from scratch"
	@echo "  help        - Display this help message"
	@echo "========================================="

# Phony targets (not actual files)
.PHONY: all run run-custom clean distclean rebuild help