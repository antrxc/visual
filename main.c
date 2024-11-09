#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 10000

// Structure to store tokens
struct Token {
    char type[20];
    char value[100];
};

// Function to remove comments and whitespace
void preprocessor(char *input, char *output) {
    int i = 0, j = 0;
    int in_comment = 0;
    
    while (input[i] != '\0') {
        // Handle single line comments
        if (input[i] == '/' && input[i + 1] == '/') {
            while (input[i] != '\n' && input[i] != '\0')
                i++;
        }
        // Handle multi-line comments
        else if (input[i] == '/' && input[i + 1] == '*') {
            in_comment = 1;
            i += 2;
            continue;
        }
        else if (input[i] == '*' && input[i + 1] == '/') {
            in_comment = 0;
            i += 2;
            continue;
        }
        // Copy non-comment characters
        else if (!in_comment) {
            // Reduce multiple spaces to single space
            if (isspace(input[i])) {
                if (j > 0 && !isspace(output[j-1])) {
                    output[j++] = ' ';
                }
            } else {
                output[j++] = input[i];
            }
        }
        i++;
    }
    output[j] = '\0';
}

// Function to perform lexical analysis
void lexical_analyzer(char *input, struct Token *tokens, int *token_count) {
    char *token = strtok(input, " \n\t");
    *token_count = 0;
    
    while (token != NULL) {
        // Keywords
        if (strcmp(token, "int") == 0 || strcmp(token, "float") == 0 || 
            strcmp(token, "char") == 0 || strcmp(token, "void") == 0 ||
            strcmp(token, "return") == 0 || strcmp(token, "if") == 0 ||
            strcmp(token, "else") == 0 || strcmp(token, "while") == 0 ||
            strcmp(token, "for") == 0) {
            strcpy(tokens[*token_count].type, "KEYWORD");
        }
        // Operators
        else if (strchr("+-*/=<>!", token[0]) != NULL) {
            strcpy(tokens[*token_count].type, "OPERATOR");
        }
        // Numbers
        else if (isdigit(token[0])) {
            strcpy(tokens[*token_count].type, "NUMBER");
        }
        // Identifiers
        else if (isalpha(token[0])) {
            strcpy(tokens[*token_count].type, "IDENTIFIER");
        }
        // Delimiters
        else if (strchr("(){};,", token[0]) != NULL) {
            strcpy(tokens[*token_count].type, "DELIMITER");
        }
        else {
            strcpy(tokens[*token_count].type, "UNKNOWN");
        }
        
        strcpy(tokens[*token_count].value, token);
        (*token_count)++;
        token = strtok(NULL, " \n\t");
    }
}

// Function to perform basic syntax analysis
void syntax_analyzer(struct Token *tokens, int token_count) {
    printf("\nSyntax Analysis Phase:\n");
    printf("Basic syntax checking...\n");
    
    int brace_count = 0;
    int paren_count = 0;
    
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i].value, "{") == 0) brace_count++;
        if (strcmp(tokens[i].value, "}") == 0) brace_count--;
        if (strcmp(tokens[i].value, "(") == 0) paren_count++;
        if (strcmp(tokens[i].value, ")") == 0) paren_count--;
    }
    
    if (brace_count != 0) printf("Error: Mismatched braces\n");
    if (paren_count != 0) printf("Error: Mismatched parentheses\n");
    if (brace_count == 0 && paren_count == 0) printf("Basic syntax check passed\n");
}

// Function to read file content
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        exit(1);
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory for file content
    char* content = (char*)malloc(file_size + 1);
    if (content == NULL) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    // Read file content
    size_t read_size = fread(content, 1, file_size, file);
    content[read_size] = '\0';

    fclose(file);
    return content;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }

    char *input = read_file(argv[1]);
    char preprocessed[MAX_SIZE];
    struct Token tokens[MAX_SIZE];
    int token_count;
    
    // Display original source code
    printf("Original Source Code:\n");
    printf("--------------------\n");
    printf("%s\n", input);
    
    // Preprocessing Phase
    printf("\nPreprocessing Phase:\n");
    printf("-------------------\n");
    preprocessor(input, preprocessed);
    printf("After preprocessing:\n%s\n", preprocessed);
    
    // Lexical Analysis Phase
    printf("\nLexical Analysis Phase:\n");
    printf("----------------------\n");
    lexical_analyzer(preprocessed, tokens, &token_count);
    printf("Tokens identified:\n");
    for (int i = 0; i < token_count; i++) {
        printf("Token %d: Type=%-12s Value=%s\n", i+1, tokens[i].type, tokens[i].value);
    }
    
    // Syntax Analysis Phase
    syntax_analyzer(tokens, token_count);
    
    // Free allocated memory
    free(input);
    return 0;
}