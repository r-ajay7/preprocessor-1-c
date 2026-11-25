#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>

#define MAX_MACROS 100
#define MAX_NAME  100
#define MAX_VALUE 1024
#define MAX_PARAMS 10
#define MAX_LINE  4096

typedef struct
{
    char name[MAX_NAME];
    char value[MAX_VALUE];
    int is_function;
    int param_count;
    char params[MAX_PARAMS][MAX_NAME];
} Macro;

extern Macro macros[MAX_MACROS];
extern int macro_count;

void remove_comments(char *line, int *inside_block_comment);
void define_macro(const char *line);
void expand_macros(const char *input, char *output, int depth);
void process_file(const char *input_name, FILE *output);

#endif
