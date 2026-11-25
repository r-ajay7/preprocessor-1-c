#include <stdio.h>
#include <string.h>
#include "preprocessor.h"

void create_preprocessed_file(const char *input_name);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./mypreprocessor inputfile.c\n");
        return 1;
    }

    create_preprocessed_file(argv[1]);
    return 0;
}
