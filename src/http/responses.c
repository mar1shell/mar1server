#include "../../include/files_utils.h"
#include "../../include/helpers.h"
#include "../../include/http_status.h"
#include "../../include/responses.h"
#include "../../include/constants.h"

const http_response_status http_status_map[] = {
    {HTTP_OK, "OK"},
    {HTTP_CREATED, "Created"},
    {HTTP_BAD_REQUEST, "Bad Request"},
    {HTTP_NOT_FOUND, "Not Found"},
    {HTTP_METHOD_NOT_ALLOWED, "Method Not Allowed"},
    {HTTP_REQUEST_TIMEOUT, "Request Timeout"},
    {HTTP_PAYLOAD_TOO_LARGE, "Payload Too Large"},
    {HTTP_INTERNAL_SERVER_ERROR, "Internal Server Error"},
    {HTTP_NOT_IMPLEMENTED, "Not Implemented"},
    {HTTP_SERVICE_UNAVAILABLE, "Service Unavailable"},
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
    NULL};

/**
 * Get the status message corresponding to a given status code.
 * @param status_code The HTTP status code.
 * @return The corresponding status message, or NULL if not found.
 */
const char *get_status_message(int status_code)
{
    const http_response_status *curr_status = http_status_map;

    while (curr_status != NULL && curr_status->code != 0)
    {
        if (curr_status->code == status_code)
        {
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
http_response_header **create_basic_response_headers(char *content_type, char *content_length)
{
    http_response_header **response_headers = (http_response_header **)malloc(sizeof(http_response_header *) * MAX_RESPONSE_HEADERS);

    if (response_headers == NULL)
    {
        if (LOGGING)
            perror("Failed to allocate memory for response headers");
        return NULL;
    }

    for (int i = 0; i < MAX_RESPONSE_HEADERS; i++)
    {
        response_headers[i] = NULL;
    }

    if (content_type == NULL)
    {
        content_type = "text/plain";
    }

    if (content_length == NULL)
    {
        content_length = "0";
    }

    x_bool is_valid_content_type = FALSE;

    for (int i = 0; valid_content_types[i] != NULL; i++)
    {
        if (strcmp(valid_content_types[i], content_type) == 0)
        {
            is_valid_content_type = TRUE;

            break;
        }
    }

    if (is_valid_content_type == FALSE)
    {
        if (LOGGING)
            perror("Invalid content type provided for response headers");

        response_headers = x_free(response_headers);

        return NULL;
    }

    char *names[] = {"Content-Type", "Content-Length", "Connection", "Server", NULL};
    char *values[] = {content_type, content_length, "close", "mar1server/0.1", NULL};

    for (int i = 0; i < 4; i++)
    {
        response_headers[i] = (http_response_header *)malloc(sizeof(http_response_header));

        if (response_headers[i] == NULL)
        {
            if (LOGGING)
                perror(RED "Failed to allocate memory for response header" RESET);

            for (int j = 0; j < i; j++)
            {
                response_headers[j] = x_free(response_headers[j]);
            }

            response_headers = x_free(response_headers);

            return NULL;
        }

        response_headers[i]->name = names[i];
        response_headers[i]->value = values[i];
    }

    response_headers[4] = NULL;

    return response_headers;
}

/**
 * Sends the HTTP response over the given socket.
 * @param socket The socket descriptor to send the response to.
 * @param response The HTTP response string to send.
 * @return The number of bytes sent, or -1 on error.
 */
ssize_t send_tcp_response(int socket, char *response)
{
    ssize_t bytes_sent = send(socket, response, strlen(response), 0);

    return bytes_sent;
}

/**
 * Creates the final HTTP response string from the given http_response structure.
 * @param response Pointer to the http_response structure.
 * @param final_response_ptr Pointer to the buffer where the final response will be stored.
 * @param final_response_size The size of the final response buffer.
 * @return Pointer to the final response string, or NULL on failure.
 */
char *create_final_response(http_response *response, char **final_response_ptr, size_t final_response_size)
{
    char *final_response = *final_response_ptr;

    if (response == NULL)
    {
        if (LOGGING)
            perror(RED "response is NULL in create_final_response_from_string" RESET);

        return NULL;
    }

    char status_code_string[STATUS_CODE_SIZE];

    // Start constructing the response line
    // HTTP version
    strncpy(final_response, "HTTP/1.1 ", strlen("HTTP/1.1 ") + 1);

    // Status code
    snprintf(status_code_string, sizeof(status_code_string), "%d ", response->status_code);
    strncat(final_response, status_code_string, final_response_size - strlen(final_response) - 1);

    // Status message
    strncat(final_response, response->status_message, final_response_size - strlen(final_response) - 1);
    strncat(final_response, CRLF, final_response_size - strlen(final_response) - 1);

    size_t body_length = (response->body != NULL) ? strlen(response->body) : 0;
    char body_length_string[SIZE_T_STRING_SIZE];

    snprintf(body_length_string, sizeof(body_length_string), "%lu", body_length);

    http_response_header **curr_header = response->http_response_headers;

    // Append headers to the final response
    while (*curr_header != NULL && (*curr_header)->name != NULL)
    {
        strncat(final_response, (*curr_header)->name, final_response_size - strlen(final_response) - 1);
        strncat(final_response, ": ", final_response_size - strlen(final_response) - 1);
        strncat(final_response, (*curr_header)->value, final_response_size - strlen(final_response) - 1);
        strncat(final_response, CRLF, final_response_size - strlen(final_response) - 1);

        curr_header++;
    }

    // End of headers
    strncat(final_response, CRLF, final_response_size - strlen(final_response) - 1);

    size_t response_length = (size_t)strlen(final_response) + body_length;

    if (response_length >= final_response_size)
    {
        char *new_final_response = (char *)realloc(final_response, (size_t)response_length + 1);

        if (new_final_response == NULL)
        {
            if (LOGGING)
                perror("error in realloc final_response (create_final_response_from_string)");

            return NULL;
        }

        final_response = new_final_response;
    }

    // Append body to the final response
    strncat(final_response, response->body, final_response_size - strlen(final_response) - 1);

    return final_response;
}

/**
 * Sends an error response to the client.
 * @param client_socket The client socket descriptor.
 * @param status_code The HTTP status code to send.
 * @param custom_message Optional custom message to include in the response body.
 * @return void
 */
void send_error_response(int client_socket, int status_code, char *custom_message)
{
    if (client_socket < 0 || status_code < 100 || status_code > 599)
    {
        if (LOGGING)
            perror(RED "Invalid client socket in send_error_response" RESET);

        return;
    }

    http_response *response = (http_response *)malloc(sizeof(http_response));
    response->status_code = status_code;
    response->status_message = get_status_message(status_code);
    response->http_response_headers = NULL;
    response->body = NULL;

    if (response->status_message == NULL)
    {
        if (LOGGING)
            perror(RED "Invalid status code in send_error_response" RESET);

        free_http_response(response);

        return;
    }

    if (custom_message != NULL)
    {
        char content_length[SIZE_T_STRING_SIZE];

        response->body = (char *)malloc((strlen(custom_message) + 1) * sizeof(char));

        strcpy(response->body, custom_message);

        snprintf(content_length, sizeof(content_length), "%lu", strlen(response->body));

        response->http_response_headers = create_basic_response_headers("text/plain", content_length);
    }
    else
    {
        response->body = (char *)malloc(INITIAL_BODY_SIZE * sizeof(char));

        response->body = serve_error_file(status_code, &response->body, INITIAL_BODY_SIZE);

        if (response->body == NULL)
        {
            if (LOGGING)
                perror(RED "Failed to load default error file in send_error_response" RESET);

            response->body = "<html><body><h1>Error</h1><p>An error occurred.</p></body></html>";
        }

        char content_length[SIZE_T_STRING_SIZE];

        snprintf(content_length, sizeof(content_length), "%lu", strlen(response->body));

        response->http_response_headers = create_basic_response_headers("text/html", content_length);
    }

    char *final_response = (char *)malloc(INITIAL_RESPONSE_SIZE);

    if (create_final_response(response, &final_response, INITIAL_RESPONSE_SIZE) == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to create final response in send_error_response" RESET);

        free_http_response(response);
        final_response = x_free(final_response);

        return;
    }

    ssize_t bytes_sent = send_tcp_response(client_socket, final_response);

    if (bytes_sent < 0)
    {
        if (LOGGING)
            perror(RED "Failed to send error response in send_error_response" RESET);
    }
    else
    {
        if (LOGGING)
            printf(GREEN "Sent %ld bytes in error response\n" RESET, bytes_sent);
    }

    free_http_response(response);
    final_response = x_free(final_response);
}

/**
 * Serves the error HTML file corresponding to the given status code.
 * @param status_code The HTTP status code.
 * @param buffer Pointer to the buffer where the file content will be stored.
 * @param buffer_size The size of the buffer.
 * @return Pointer to the buffer containing the file content, or NULL on failure.
 */
char *serve_error_file(int status_code, char **buffer, size_t buffer_size)
{
    if (status_code < 400 || status_code > 599 || buffer == NULL || buffer_size == 0)
    {
        if (LOGGING)
            perror(RED "Invalid arguments in serve_error_file" RESET);

        return NULL;
    }

    char file_path[PATH_MAX];

    snprintf(file_path, sizeof(file_path), "%s/%d.html", STATIC_FILES_PATH, status_code);

    if (read_text_file(file_path, buffer, buffer_size) == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to read error file in serve_error_file" RESET);

        return NULL;
    }

    return *buffer;
}

/**
 * Frees the memory allocated for an http_response structure.
 * @param response Pointer to the http_response structure to free.
 */
void *free_http_response(http_response *response)
{
    if (response == NULL)
    {
        return NULL;
    }

    http_response_header **curr_http_header = response->http_response_headers;

    while (curr_http_header != NULL && *curr_http_header != NULL)
    {
        *curr_http_header = x_free(*curr_http_header);
        curr_http_header++;
    }

    if (response->http_response_headers != NULL)
        response->http_response_headers = x_free(response->http_response_headers);

    response->body = x_free(response->body);
    response = x_free(response);

    return NULL;
}