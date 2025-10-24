#include "../../include/constants.h"
#include "../../include/files_utils.h"

/**
 * Safely close a file pointer.
 * @param file The file pointer to close.
 */
void x_fclose(FILE *file) {
    if (file != NULL) {
        fclose(file);
    }
}

/**
 * Calculate the size of a file.
 * @param file The file pointer.
 * @return fie size in bytes.
 */
size_t file_size(FILE *file) {
    if (file == NULL) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

/**
 * Check if a file exists.
 * @param file_path The path to the file.
 * @return TRUE if the file exists, FALSE otherwise.
 */
int file_exists(char *file_path) {
    FILE *file = fopen(file_path, "r");

    if (file != NULL) {
        x_fclose(file);
        return TRUE;

    }

    return FALSE;
}

mime_type mime_types[] = {
    {".html", "text/html"},
    {".htm", "text/html"},
    {".txt", "text/plain"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".json", "application/json"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".svg", "image/svg+xml"},
    {".ico", "image/x-icon"},
    {".pdf", "application/pdf"},
    {NULL, NULL} // Sentinel value
};

/**
 * Extract the content type from a file name based on its extension.
 * @param file_name The name of the file.
 * @return The corresponding content type string.
 */
char *extract_content_type_from_file(char *file_name) {
    if (file_name == NULL || strcmp(file_name, "") == 0) {
        return "text/plain";
    }

    mime_type *curr_mime = mime_types;

    while (curr_mime != NULL || curr_mime->type != NULL) {
        if (strstr(file_name, curr_mime->extension) == 0) {
            return curr_mime->type;
        }

        curr_mime++;
    }

    curr_mime = mime_types;

    return "text/plain";
}