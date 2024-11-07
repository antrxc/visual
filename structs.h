#include"enum.h"
#include "def.h"
#ifndef STRUCT_H
#define STRUCT_H

// Struct for compiler configuration
typedef struct {
    char compiler_path[MAX_FILENAME_LEN];
    char source_file[MAX_FILENAME_LEN];
    bool verbose;
    char temp_dir[MAX_FILENAME_LEN];
} CompilerConfig;

// Struct for phase information
typedef struct {
    CompilationPhase phase;
    char description[MAX_CMD_LEN];
    char command_template[MAX_CMD_LEN];
} PhaseInfo;


// Struct for holding command output
typedef struct {
    char output[MAX_OUTPUT_LEN];
    CommandStatus status;
} CommandResult;

#endif