#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "def.h"
#include"enum.h"
#include"structs.h"

#define MAX_CMD_LEN 1024
#define MAX_OUTPUT_LEN 4096
#define MAX_FILENAME_LEN 256




// Function to initialize compiler configuration
CompilerConfig init_compiler_config(const char* source_file) {
    CompilerConfig config = {
        .verbose = false,
        .temp_dir = "/tmp"
    };
    strncpy(config.compiler_path, "gcc", MAX_FILENAME_LEN - 1);
    strncpy(config.source_file, source_file, MAX_FILENAME_LEN - 1);
    return config;
}

// Function to execute a command and capture its output
CommandResult execute_command(const char* command) {
    CommandResult result = {0};
    FILE* pipe = popen(command, "r");
    
    if (!pipe) {
        result.status = CMD_FAILURE;
        return result;
    }
    
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        if (strlen(result.output) + strlen(buffer) < MAX_OUTPUT_LEN) {
            strcat(result.output, buffer);
        }
    }
    
    int status = pclose(pipe);
    result.status = (status == 0) ? CMD_SUCCESS : CMD_FAILURE;
    return result;
}

// Function to get phase information
PhaseInfo get_phase_info(CompilationPhase phase) {
    PhaseInfo info = {0};
    info.phase = phase;
    
    switch (phase) {
        case PREPROCESSING:
            strcpy(info.description, "Preprocessing Phase");
            strcpy(info.command_template, "%s -E %s");
            break;
        case COMPILATION:
            strcpy(info.description, "Compilation to Assembly Phase");
            strcpy(info.command_template, "%s -S -fno-asynchronous-unwind-tables %s -o -");
            break;
        case ASSEMBLY:
            strcpy(info.description, "Assembly to Object Code Phase");
            strcpy(info.command_template, "%s -c -o %s/temp.o %s && objdump -d %s/temp.o");
            break;
        case LINKING:
            strcpy(info.description, "Linking Phase");
            strcpy(info.command_template, "%s -v %s");
            break;
    }
    
    return info;
}

// Function to process a compilation phase
void process_phase(const CompilerConfig* config, CompilationPhase phase) {
    PhaseInfo info = get_phase_info(phase);
    char command[MAX_CMD_LEN];
    
    printf("\n=== %s ===\n", info.description);
    
    switch (phase) {
        case PREPROCESSING:
        case COMPILATION:
        case LINKING:
            snprintf(command, sizeof(command), info.command_template, 
                    config->compiler_path, config->source_file);
            break;
        case ASSEMBLY:
            snprintf(command, sizeof(command), info.command_template,
                    config->compiler_path, config->temp_dir, 
                    config->source_file, config->temp_dir);
            break;
    }
    
    CommandResult result = execute_command(command);
    
    if (result.status == CMD_SUCCESS) {
        printf("Output:\n%s\n", result.output);
        if (phase == ASSEMBLY) {
            char rm_command[MAX_CMD_LEN];
            snprintf(rm_command, sizeof(rm_command), "rm -f %s/temp.o", config->temp_dir);
            system(rm_command);
        }
    } else {
        printf("Error during %s\n", info.description);
    }
    
    if (config->verbose) {
        printf("Command executed: %s\n", command);
    }
}

// Function to validate source file
bool validate_source_file(const char* filename) {
    if (access(filename, F_OK) == -1) {
        printf("Error: Source file '%s' does not exist\n", filename);
        return false;
    }
    
    const char* extension = strrchr(filename, '.');
    if (!extension || strcmp(extension, ".c") != 0) {
        printf("Error: File must have .c extension\n");
        return false;
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file.c>\n", argv[0]);
        return 1;
    }
    
    if (!validate_source_file(argv[1])) {
        return 1;
    }
    
    CompilerConfig config = init_compiler_config(argv[1]);
    printf("Analyzing compilation phases for: %s\n", config.source_file);
    
    // Process all phases
    CompilationPhase phases[] = {PREPROCESSING, COMPILATION, ASSEMBLY, LINKING};
    for (size_t i = 0; i < sizeof(phases)/sizeof(phases[0]); i++) {
        process_phase(&config, phases[i]);
    }
    
    return 0;
}