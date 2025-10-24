#ifndef FILES_UTILS
#define FILES_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *extension;
    char *type;
} mime_type;

char *extract_content_type_from_file(char *file_name);
int file_exists(char *file_path);
size_t file_size(FILE *file);
void x_fclose(FILE *file);


#endif // FILES_UTILS
