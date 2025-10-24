#include "../../include/files_utils.h"
#include "../../include/helpers.h"
#include "../../include/http_status.h"
#include "../../include/responses.h"

const http_response_status http_status_map[] = {
    {HTTP_OK, "OK"},
    {HTTP_CREATED, "Created"},
    {HTTP_BAD_REQUEST, "Bad Request"},
    {HTTP_NOT_FOUND, "Not Found"},
    {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HTTP_PAYLOAD_TOO_LARGE, "Payload Too Large"},
    {HTTP_INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {0, NULL} // Terminator (end of the list)
};


const char *valid_content_types[] = {
    "text/plain",
    "text/html",
    "application/json",
    "image/png",
    "image/jpeg",
    "application/javascript",
    "text/css",
    NULL
};

/**
 * Get the status message corresponding to a given status code.
 * @param status_code The HTTP status code.
 * @return The corresponding status message, or NULL if not found.
 */
const char *get_status_message(int status_code) {
    const http_response_status *curr_status = http_status_map;

    while (curr_status != NULL && curr_status->code != 0) {
        if (curr_status->code == status_code) {
            return curr_status->message;
        }

        curr_status++;
    }

    return NULL;
}

/**
 * Create basic HTTP response headers.
 * @param content_type The Content-Type header value.
 * @param content_length The Content-Length header value.
 * @return An array of http_response_header pointers.
 */
http_response_header **create_basic_response_headers(char *content_type, char *content_length) {
    http_response_header **response_headers = (http_response_header **)malloc(sizeof(http_response_header *) * MAX_RESPONSE_HEADERS);

    if (response_headers == NULL) {
        if (LOGGING) perror("Failed to allocate memory for response headers");
        return NULL;
    }

    for (int i = 0; i < MAX_RESPONSE_HEADERS; i++) {
        response_headers[i] = NULL;
    }

    if (content_type == NULL) {
        content_type = "text/plain";
    }

    if (content_length == NULL) {
        content_length = "0";
    }

    bool is_valid_content_type = FALSE;

    for (int i = 0; valid_content_types[i] != NULL; i++) {
        if (strcmp(valid_content_types[i], content_type) == 0) {
            is_valid_content_type = TRUE;

            break;
        }
    }

    if (is_valid_content_type == FALSE) {
        if (LOGGING) perror("Invalid content type provided for response headers");

        response_headers = x_free(response_headers);
        
        return NULL;
    }

    char *names[] = {"Content-Type", "Content-Length", "Connection", NULL};
    char *values[] = {content_type, content_length, "close", NULL};

    for (int i = 0; i < 3; i++) {
        response_headers[i] = (http_response_header *)malloc(sizeof(http_response_header));
        
        // TODO
        if (response_headers[i] == NULL) {
            if (LOGGING) perror(RED"Failed to allocate memory for response header"RESET); 
            
            for (int j = 0; j < i; j++) {
                response_headers[j] = x_free(response_headers[j]);
            }

            response_headers = x_free(response_headers);

            return NULL;
        }

        response_headers[i]->name = names[i];
        response_headers[i]->value = values[i];
    }

    response_headers[3] = NULL;

    return response_headers;
}


/**
 * Sends the HTTP response over the given socket.
 * @param socket The socket descriptor to send the response to.
 * @param response The HTTP response string to send.
 * @return The number of bytes sent, or -1 on error.
 */
ssize_t send_response(int socket, char *response) {
    ssize_t bytes_sent = send(socket, response, strlen(response), 0);

    return bytes_sent;
}

/**
 * Creates the final HTTP response string from the given http_response structure and body string.
 * @param response_ptr Pointer to the http_response structure.
 * @param final_response The buffer to store the final response string.
 * @param final_response_size The size of the final_response buffer.
 * @param body The body string to include in the response.
 * @return The final response string, or NULL on error.
 */
char *create_final_response_from_string(http_response **response_ptr, char *final_response, size_t final_response_size, const char *body) {
    if (response_ptr == NULL || *response_ptr == NULL) {
        if (LOGGING) perror(RED"response is NULL in create_final_response_from_string"RESET);

        return NULL;
    }

    http_response *response = *response_ptr;

    char status_code_string[STATUS_CODE_SIZE];

    // Start constructing the response line
    // HTTP version
    strcpy(final_response, "HTTP/1.1 ");

    // Status code
    sprintf(status_code_string, "%d ", response->status_code);
    strcat(final_response, status_code_string);

    // Status message
    strcat(final_response, response->status_message);
    strcat(final_response, CRLF);

    size_t body_length = (body != NULL) ? strlen(body) : 0;
    char body_length_string[20];

    sprintf(body_length_string, "%lu", body_length);

    // Create basic response headers (Content-Type and Content-Length and Connection)
    response->http_response_headers = create_basic_response_headers("text/plain", body_length_string);

    http_response_header **curr_header = response->http_response_headers;

    // Append headers to the final response
    while (*curr_header != NULL && (*curr_header)->name != NULL) {
        strcat(final_response, (*curr_header)->name);
        strcat(final_response, ": ");
        strcat(final_response, (*curr_header)->value);
        strcat(final_response, CRLF);

        curr_header++;
    }

    // End of headers
    strcat(final_response, CRLF);

    size_t response_length = (size_t)strlen(final_response) + body_length;

    if (response_length > final_response_size + 1) {
        final_response = (char *)realloc(final_response, (size_t)response_length + 1);

        if (final_response == NULL) {
            if (LOGGING) perror("error in realloc final_response (create_final_response_from_string)");

            return NULL;
        }
    }

    // Append body to the final response
    strcat(final_response, body);

    return final_response;
}

/**
 * Creates the final HTTP response string from the given http_response structure and file content.
 * @param response_ptr Pointer to the http_response structure.
 * @param final_response The buffer to store the final response string.
 * @param final_response_size The size of the final_response buffer.
 * @param file The file pointer to read the body content from.
 * @return The final response string, or NULL on error.
 * @attention Make sure the file is opened in read mode before calling this function and free response after use.
 */
char *create_final_response_from_file(http_response **response_ptr, 
                                    char *final_response, 
                                    size_t final_response_size, 
                                    FILE *file, 
                                    char *file_name) {
    if (response_ptr == NULL || *response_ptr == NULL) {
        if (LOGGING) perror(RED"response is NULL in create_final_response_from_string"RESET);

        return NULL;
    }
    
    http_response *response = *response_ptr;

    char status_code_string[STATUS_CODE_SIZE];

    // Start constructing the response line
    // HTTP version
    strcpy(final_response, "HTTP/1.1 ");

    // Status code
    sprintf(status_code_string, "%d ", response->status_code);
    strcat(final_response, status_code_string);

    // Status message
    strcat(final_response, response->status_message);
    strcat(final_response, CRLF);

    size_t body_length = file_size(file);
    char body_length_string[20];

    // convert Body length to string
    sprintf(body_length_string, "%lu", body_length);

    char *content_type = extract_content_type_from_file(file_name);

    response->http_response_headers = create_basic_response_headers(content_type, body_length_string);

    char *body = (char *)malloc(body_length + 1);

    if (body == NULL) {
        if (LOGGING) perror(RED"error in malloc body (create_final_response_from_file)"RESET);
        
        return NULL;
    }

    body[0] = '\0';

    http_response_header **curr_header = response->http_response_headers;

    // Append headers to the final response
    while (*curr_header != NULL && (*curr_header)->name != NULL) {
        strcat(final_response, (*curr_header)->name);
        strcat(final_response, ": ");
        strcat(final_response, (*curr_header)->value);
        strcat(final_response, CRLF);

        curr_header++;
    }

    // End of headers
    strcat(final_response, CRLF);

    size_t response_length = (size_t)strlen(final_response) + body_length;

    if (response_length > final_response_size + 1) {
        final_response = (char *)realloc(final_response, (size_t)response_length + 1);

        if (final_response == NULL) {
            if (LOGGING) perror(RED"error in realloc final_response (create_final_response_from_string)"RESET);

            return NULL;
        }
    }

    // Read file content into body
    size_t read_size = fread(body, 1, body_length, file);

    if (read_size != body_length) {
        if (LOGGING) perror(RED"error in fread body (create_final_response_from_file)"RESET);

        x_free(body);

        return NULL;
    }

    body[body_length] = '\0';

    // finally, append body to the final response
    strcat(final_response, body);

    x_free(body);

    return final_response;
}

/**
 * Frees the memory allocated for an http_response structure.
 * @param response Pointer to the http_response structure to free.
 */

void *free_http_response(http_response *response) {
    if (response ==NULL) {
        return NULL;
    }
    
    http_response_header **curr_http_header = response->http_response_headers;

    while (curr_http_header != NULL && *curr_http_header != NULL) {
        *curr_http_header = x_free(*curr_http_header);
        curr_http_header++;
    }

    if (*curr_http_header != NULL) {
        *curr_http_header = x_free(*curr_http_header);  // If sentinel is allocated, free it
    }
    
    response->http_response_headers = x_free(response->http_response_headers);
    response->body = x_free(response->body);
    response = x_free(response);

    return NULL;
}

/**
 * Validates an HTTP method.
 * @param method The HTTP method to validate.
 * @return 0 if valid, -1 otherwise.
 */
bool validate_http_method(char *method) {
    char *valid_methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "TRACE", "CONNECT", NULL};

    for (int i = 0; valid_methods[i] != NULL; i++) {
        if (strcmp(method, valid_methods[i]) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 * Validates an HTTP version.
 * @param version The HTTP version to validate.
 * @return TRUE (1) if valid, FALSE (0) otherwise.
 */
bool validate_http_version(char *version) {
    if (strcmp(version, "HTTP/1.1") == 0) {
        return TRUE;
    }

    return FALSE;
}