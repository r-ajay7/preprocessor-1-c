#include <string.h>
#include "preprocessor.h"


void remove_comments(char *line, int *inside_block_comment)
{
    int i = 0, j = 0;
    int in_string = 0, in_char = 0, escaped = 0;

    while (line[i] != '\0')
    {
        if (*inside_block_comment)
        {
            if (line[i] == '*' && line[i + 1] == '/')
            {
                *inside_block_comment = 0;
                i += 2;
            }
            else
            {
                if (line[i] == '\n')
                    line[j++] = '\n';
                i++;
            }
            continue;
        }

        if (!in_string && !in_char && line[i] == '/' && line[i + 1] == '*')
        {
            *inside_block_comment = 1;
            i += 2;
            continue;
        }

        if (!in_string && !in_char && line[i] == '/' && line[i + 1] == '/')
            break;

        line[j++] = line[i];

        if (escaped)
            escaped = 0;
        else if ((in_string || in_char) && line[i] == '\\')
            escaped = 1;
        else if (!in_char && line[i] == '"')
            in_string = !in_string;
        else if (!in_string && line[i] == '\'')
            in_char = !in_char;

        i++;
    }

    line[j] = '\0';
}
