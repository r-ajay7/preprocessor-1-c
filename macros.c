#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "preprocessor.h"

Macro macros[MAX_MACROS];
int macro_count = 0;

static int is_identifier_start(char c)
{
    return isalpha((unsigned char)c) || c == '_';
}

static int is_identifier_char(char c)
{
    return isalnum((unsigned char)c) || c == '_';
}

static void trim(char *s)
{
    int start = 0;
    size_t len;

    while (isspace((unsigned char)s[start]))
        start++;

    if (start > 0)
        memmove(s, s + start, strlen(s + start) + 1);

    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

static int find_macro(const char *name)
{
    int i;
    for (i = 0; i < macro_count; i++)
        if (strcmp(macros[i].name, name) == 0)
            return i;
    return -1;
}

void define_macro(const char *line)
{
    char copy[MAX_LINE], *p, *name_start, *value_start;
    Macro m;
    int index;

    if (macro_count >= MAX_MACROS)
        return;

    strcpy(copy, line);
    p = copy;

    while (isspace((unsigned char)*p)) p++;
    if (strncmp(p, "#define", 7) != 0)
        return;
    p += 7;
    while (isspace((unsigned char)*p)) p++;

    name_start = p;
    while (is_identifier_char(*p)) p++;

    if (p == name_start)
        return;

    memset(&m, 0, sizeof(m));
    snprintf(m.name, sizeof(m.name), "%.*s", (int)(p - name_start), name_start);

    if (*p == '(')
    {
        char *close;
        m.is_function = 1;
        p++;
        close = strchr(p, ')');
        if (close == NULL)
            return;

        while (p < close && m.param_count < MAX_PARAMS)
        {
            char *comma = strchr(p, ',');
            char *end = (comma && comma < close) ? comma : close;
            int n = (int)(end - p);
            while (n > 0 && isspace((unsigned char)*p)) { p++; n--; }
            while (n > 0 && isspace((unsigned char)p[n - 1])) n--;
            if (n > 0)
                snprintf(m.params[m.param_count++], MAX_NAME, "%.*s", n, p);
            p = (end == close) ? close : end + 1;
        }
        p = close + 1;
    }

    while (isspace((unsigned char)*p)) p++;
    value_start = p;
    snprintf(m.value, sizeof(m.value), "%s", value_start);
    trim(m.value);

    index = find_macro(m.name);
    if (index >= 0)
        macros[index] = m;
    else
        macros[macro_count++] = m;
}

static void replace_parameters(const Macro *m, char args[MAX_PARAMS][MAX_VALUE],
                               char *result)
{
    int i = 0, j = 0;

    while (m->value[i] != '\0' && j < MAX_VALUE - 1)
    {
        if (is_identifier_start(m->value[i]))
        {
            char token[MAX_NAME];
            int k = 0, p, found = -1;

            while (is_identifier_char(m->value[i]) && k < MAX_NAME - 1)
                token[k++] = m->value[i++];
            token[k] = '\0';

            for (p = 0; p < m->param_count; p++)
                if (strcmp(token, m->params[p]) == 0)
                    found = p;

            if (found >= 0)
            {
                int a;
                for (a = 0; args[found][a] && j < MAX_VALUE - 1; a++)
                    result[j++] = args[found][a];
            }
            else
            {
                int a;
                for (a = 0; token[a] && j < MAX_VALUE - 1; a++)
                    result[j++] = token[a];
            }
        }
        else
            result[j++] = m->value[i++];
    }
    result[j] = '\0';
}

void expand_macros(const char *input, char *output, int depth)
{
    int i = 0, j = 0;
    int in_string = 0, in_char = 0, escaped = 0;

    if (depth > 10)
    {
        snprintf(output, MAX_LINE, "%s", input);
        return;
    }

    while (input[i] != '\0' && j < MAX_LINE - 1)
    {
        if (escaped)
        {
            output[j++] = input[i++];
            escaped = 0;
            continue;
        }

        if ((in_string || in_char) && input[i] == '\\')
        {
            output[j++] = input[i++];
            escaped = 1;
            continue;
        }

        if (!in_char && input[i] == '"')
        {
            in_string = !in_string;
            output[j++] = input[i++];
            continue;
        }

        if (!in_string && input[i] == '\'')
        {
            in_char = !in_char;
            output[j++] = input[i++];
            continue;
        }

        if (!in_string && !in_char && is_identifier_start(input[i]))
        {
            char token[MAX_NAME];
            int k = 0, index;

            while (is_identifier_char(input[i]) && k < MAX_NAME - 1)
                token[k++] = input[i++];
            token[k] = '\0';

            index = find_macro(token);
            if (index >= 0 && !macros[index].is_function)
            {
                char expanded[MAX_LINE];
                expand_macros(macros[index].value, expanded, depth + 1);
                k = 0;
                while (expanded[k] && j < MAX_LINE - 1)
                    output[j++] = expanded[k++];
            }
            else if (index >= 0 && macros[index].is_function)
            {
                int pos = i, arg_count = 0, level = 0, a = 0;
                char args[MAX_PARAMS][MAX_VALUE] = {{0}};
                char replacement[MAX_VALUE];

                while (isspace((unsigned char)input[pos])) pos++;

                if (input[pos] != '(')
                {
                    k = 0;
                    while (token[k] && j < MAX_LINE - 1) output[j++] = token[k++];
                    continue;
                }

                pos++;
                while (input[pos] && arg_count < MAX_PARAMS)
                {
                    char c = input[pos];

                    if (c == '(') level++;
                    if (c == ')')
                    {
                        if (level == 0)
                        {
                            args[arg_count][a] = '\0';
                            if (a > 0 || arg_count > 0)
                                arg_count++;
                            pos++;
                            break;
                        }
                        level--;
                    }

                    if (c == ',' && level == 0)
                    {
                        args[arg_count][a] = '\0';
                        arg_count++;
                        a = 0;
                        pos++;
                        continue;
                    }

                    if (a < MAX_VALUE - 1)
                        args[arg_count][a++] = c;
                    pos++;
                }

                if (pos <= i || input[pos - 1] != ')')
                {
                    k = 0;
                    while (token[k] && j < MAX_LINE - 1) output[j++] = token[k++];
                    continue;
                }

                if (macros[index].param_count == 0)
                    arg_count = 0;

                if (arg_count == macros[index].param_count)
                {
                    int x;
                    for (x = 0; x < arg_count; x++)
                    {
                        char trimmed[MAX_VALUE];
                        snprintf(trimmed, sizeof(trimmed), "%s", args[x]);
                        trim(trimmed);
                        snprintf(args[x], MAX_VALUE, "%s", trimmed);
                    }
                    replace_parameters(&macros[index], args, replacement);
                    {
                        char expanded[MAX_LINE];
                        expand_macros(replacement, expanded, depth + 1);
                        k = 0;
                        while (expanded[k] && j < MAX_LINE - 1)
                            output[j++] = expanded[k++];
                    }
                    i = pos;
                }
                else
                {
                    k = 0;
                    while (token[k] && j < MAX_LINE - 1) output[j++] = token[k++];
                }
            }
            else
            {
                k = 0;
                while (token[k] && j < MAX_LINE - 1) output[j++] = token[k++];
            }
            continue;
        }

        output[j++] = input[i++];
    }

    output[j] = '\0';
}
