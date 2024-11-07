#ifndef ENUM_H
#define ENUM_H 

// Enum for compilation phases
typedef enum {
    PREPROCESSING,
    COMPILATION,
    ASSEMBLY,
    LINKING
} CompilationPhase;

// Enum for command execution status
typedef enum {
    CMD_SUCCESS,
    CMD_FAILURE,
    CMD_FILE_ERROR
} CommandStatus;

#endif 