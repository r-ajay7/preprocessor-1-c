#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"

static void make_output_name(const char *input, char *output)
{
    const char *dot = strrchr(input, '.');
    size_t length = dot ? (size_t)(dot - input) : strlen(input);
    snprintf(output, 512, "%.*s.i", (int)length, input);
}

static int get_include_name(const char *line, char *name)
{
    const char *p = line;
    char end;
    int i = 0;

    while (isspace((unsigned char)*p)) p++;
    if (strncmp(p, "#include", 8) != 0)
        return 0;
    p += 8;
    while (isspace((unsigned char)*p)) p++;

    if (*p != '"' && *p != '<')
        return 0;

    end = (*p == '"') ? '"' : '>';
    p++;

    while (*p && *p != end && i < 500)
        name[i++] = *p++;
    name[i] = '\0';

    return (*p == end && i > 0);
}

void process_file(const char *input_name, FILE *output)
{
    FILE *input;
    char line[MAX_LINE], include_name[512], expanded[MAX_LINE];
    int inside_block_comment = 0;

    input = fopen(input_name, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Error: cannot open file '%s'\n", input_name);
        return;
    }

    while (fgets(line, sizeof(line), input) != NULL)
    {
        remove_comments(line, &inside_block_comment);

        if (get_include_name(line, include_name))
        {
            process_file(include_name, output);
            continue;
        }

        {
            char *p = line;
            while (isspace((unsigned char)*p)) p++;

            if (strncmp(p, "#define", 7) == 0)
            {
                define_macro(p);
                continue;
            }
        }

        expand_macros(line, expanded, 0);
        fputs(expanded, output);
    }

    fclose(input);
}

void create_preprocessed_file(const char *input_name)
{
    char output_name[512];
    FILE *output;

    make_output_name(input_name, output_name);
    output = fopen(output_name, "w");

    if (output == NULL)
    {
        fprintf(stderr, "Error: cannot create output file '%s'\n", output_name);
        return;
    }

    process_file(input_name, output);
    fclose(output);

    printf("Preprocessing completed: %s\n", output_name);
}
