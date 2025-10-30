#include "../../include/files_utils.h"

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
 * Safely close a file pointer.
 * @param file The file pointer to close.
 */
void x_fclose(FILE *file)
{
    if (file != NULL)
    {
        fclose(file);
    }
}

/**
 * Calculate the size of a file.
 * @param file The file pointer.
 * @return fie size in bytes.
 */
size_t file_size(FILE *file)
{
    if (file == NULL)
    {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}

/**
 * Extract the content type from a file name based on its extension.
 * @param file_name The name of the file.
 * @return The corresponding content type string.
 * @attention returns "text/plain" if the file name is NULL or empty, or if no matching extension is found.
 */
char *extract_content_type_from_file(char *file_name)
{
    if (file_name == NULL || strcmp(file_name, "") == 0)
    {
        return "text/plain";
    }

    mime_type *curr_mime = mime_types;

    while (curr_mime != NULL || curr_mime->type != NULL)
    {
        if (strstr(file_name, curr_mime->extension) == 0)
        {
            return curr_mime->type;
        }

        curr_mime++;
    }

    curr_mime = mime_types;

    return "text/plain";
}

/**
 * Check if a given path is safe (i.e., does not lead outside the static files directory).
 * @param path The file path to check.
 * @return TRUE if the path is safe, FALSE otherwise.
 */
x_bool is_path_safe(char *path)
{
    char resolved_path[PATH_MAX];
    char base_path[PATH_MAX];
    char static_files_path[PATH_MAX];

    strcpy(static_files_path, STATIC_FILES_PATH);
    strcpy(base_path, static_files_path);
    strcat(base_path, path);

    printf("Base Path: %s\n", base_path);

    realpath(base_path, resolved_path);

    if (strncmp(resolved_path, static_files_path, strlen(static_files_path)) != 0)
    {
        if (LOGGING)
            perror(RED "Unsafe file path detected" RESET);
        printf("Resolved Path: %s\n", resolved_path);
        return FALSE;
    }

    return TRUE;
}

/**
 * Read the contents of a text file into a dynamically allocated buffer.
 * @param file_path The path to the text file.
 * @param buffer_ptr Pointer to the buffer where the file contents will be stored.
 * @param buffer_size The initial size of the buffer.
 * @return Pointer to the buffer containing the file contents, or NULL on failure.
 */
char *read_text_file(const char *file_path, char **buffer_ptr, size_t buffer_size)
{
    char *buffer = *buffer_ptr;
    FILE *file = fopen(file_path, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return NULL;
    }

    size_t total_size = file_size(file);

    if (total_size >= MAX_BODY_SIZE)
    {
        if (LOGGING)
            fprintf(stderr, RED "Error: File size exceeds MAX_BODY_SIZE (%d bytes). Aborting.\n" RESET, MAX_BODY_SIZE);

        x_fclose(file);
        return NULL;
    }

    size_t total_read = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer + total_read, 1, buffer_size - total_read, file)) > 0)
    {
        total_read += bytes_read;

        if (total_read + 1 >= buffer_size)
        {
            size_t new_size = buffer_size * 2;
            char *new_buffer = realloc(buffer, new_size);

            if (new_buffer == NULL)
            {
                if (LOGGING)
                    fprintf(stderr, RED "Error: Memory allocation failed while reading file '%s'.\n" RESET, file_path);

                x_fclose(file);

                return NULL;
            }

            buffer = new_buffer;
            buffer_size = new_size;
        }
    }

    if (ferror(file))
    {
        if (LOGGING)
            fprintf(stderr, RED "Error: Failed to read file '%s'.\n" RESET, file_path);

        return NULL;
    }

    buffer[total_read] = '\0';

    return buffer;
}