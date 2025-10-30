#ifndef FILES_UTILS
#define FILES_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "constants.h"

typedef struct
{
    char *extension;
    char *type;
} mime_type;

char *read_text_file(const char *file_path, char **buffer_ptr, size_t buffer_size);
char *extract_content_type_from_file(char *file_name);
size_t file_size(FILE *file);
x_bool is_path_safe(char *path);
int x_fclose(FILE *file);

#endif // FILES_UTILS
