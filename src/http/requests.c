#include "../../include/constants.h"
#include "../../include/requests.h"
#include "../../include/http_status.h"
#include "../../include/files_utils.h"
#include "../../include/responses.h"
#include "../../include/helpers.h"

/**
 * Retrieves the value of a specific header from an array of HTTP headers.
 * @param http_headers The array of HTTP headers.
 * @param name The name of the header to retrieve.
 * @return A pointer to the value of the header, or NULL if not found.
 */
char *get_header_value(http_header **http_headers, char *name)
{
    http_header **curr_header = http_headers;

    while (*curr_header != NULL)
    {
        if (strcasecmp((*curr_header)->name, name) == 0)
        {
            return (*curr_header)->value;
        }

        curr_header++;
    }

    return NULL;
}

/**
 * TODO
 * Parses the body from the raw HTTP request.
 * @param raw_body The raw body string.
 * @param headers The array of HTTP headers.
 * @return A pointer to the parsed body string.
 * @attention NOT IMPLEMENTED YET
 */
char *parse_http_body(char *raw_body)
{
    if (raw_body == NULL)
    {
        return NULL;
    }

    return raw_body;
}

/**
 * Parses the header from the raw HTTP request.
 * @param raw_http_header The raw header string.
 * @return A pointer to the parsed headers array.
 * @attention The returned headers must be freed by the caller.
 */
http_header *parse_http_header(char *raw_http_header)
{
    if (raw_http_header == NULL || raw_http_header[0] == '\0')
    {
        return NULL;
    }

    http_header *parsed_http_header = NULL;

    parsed_http_header = (http_header *)malloc(sizeof(http_header));

    if (parsed_http_header == NULL)
    {
        if (LOGGING)
            perror(RED "error in malloc (parse_http_header)" RESET);

        return NULL;
    }

    char *colon_pos = strstr(raw_http_header, ": ");

    if (colon_pos == NULL)
    {
        if (LOGGING)
            perror(RED "Invalid HTTP header format" RESET);

        parsed_http_header = x_free(parsed_http_header);

        return NULL;
    }

    *colon_pos = '\0';

    parsed_http_header->name = raw_http_header;
    parsed_http_header->value = colon_pos + 2;

    return parsed_http_header;
}

/**
 * Parses a raw HTTP request string into an http_request struct.
 * @param request The raw HTTP request string.
 * @return A pointer to the parsed http_request struct.
 * @attention The returned http_request must be freed using freeHttprequest.
 */
http_request *parse_http_request(int client_socket, char *request)
{
    if (request == NULL || request[0] == '\0')
    {
        if (LOGGING)
            perror(RED "request is NULL or empty in parse_http_request" RESET);

        return NULL;
    }

    http_request *parsed_request = NULL;

    parsed_request = (http_request *)malloc(sizeof(http_request));

    if (parsed_request == NULL)
    {
        if (LOGGING)
            perror(RED "error in malloc (parse_http_request)" RESET);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

        return NULL;
    }

    parsed_request->method = NULL;
    parsed_request->url = NULL;
    parsed_request->version = NULL;
    parsed_request->http_headers = NULL;
    parsed_request->body = NULL;

    char *body_delimiter = strstr(request, BODY_DELIM);

    if (body_delimiter == NULL)
    {
        if (LOGGING)
            perror(RED "No body delimiter found in request" RESET);

        send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: No body delimiter found");

        free_http_request(parsed_request);

        return NULL;
    }

    *body_delimiter = '\0';

    parsed_request->body = body_delimiter + BODY_DELIM_LENGTH;

    char *state = NULL;
    char *line = NULL;

    line = strtok_r(request, CRLF, &state);

    parsed_request->method = strtok(line, " ");

    if (validate_http_method(parsed_request->method) == FALSE)
    {
        if (LOGGING)
            perror(RED "Invalid HTTP method in request" RESET);

        send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: Invalid HTTP method");

        free_http_request(parsed_request);

        return NULL;
    }

    parsed_request->url = strtok(NULL, " ");

    if (validate_http_url(parsed_request->url) == FALSE)
    {
        if (LOGGING)
            perror(RED "Invalid HTTP URL in request" RESET);

        send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: Invalid HTTP URL");

        free_http_request(parsed_request);

        return NULL;
    }

    if (is_path_safe(parsed_request->url) == FALSE)
    {
        if (LOGGING)
            perror(RED "Invalid file path in request" RESET);

        send_error_response(client_socket, HTTP_STATUS_FORBIDDEN, NULL);

        free_http_request(parsed_request);

        return NULL;
    }

    parsed_request->version = strtok(NULL, " ");

    if (validate_http_version(parsed_request->version) == FALSE)
    {
        if (LOGGING)
            perror(RED "Invalid HTTP version in request" RESET);

        send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: Invalid HTTP version");

        free_http_request(parsed_request);

        return NULL;
    }

    line = strtok_r(NULL, CRLF, &state);

    parsed_request->http_headers = (http_header **)malloc(sizeof(http_header *) * MAX_HEADERS);

    if (parsed_request->http_headers == NULL)
    {
        if (LOGGING)
            perror(RED "error in malloc (parse_http_request)" RESET);

        send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

        parsed_request = x_free(parsed_request);

        return NULL;
    }

    http_header **current_header = NULL;

    char *host_header = get_header_value(parsed_request->http_headers, "Host");

    // Check if Host header is present, as required by HTTP/1.1
    if (host_header == NULL)
    {
        if (LOGGING)
            perror(RED "Missing Host header in request" RESET);

        send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: Missing Host header");

        free_http_request(parsed_request);

        return NULL;
    }

    int header_count = 0;

    current_header = parsed_request->http_headers;

    while (line != NULL)
    {
        if (header_count >= MAX_HEADERS - 1)
        {
            *current_header = NULL;

            if (LOGGING)
                perror(YELLOW "Maximum number of headers exceeded" RESET);

            send_error_response(client_socket, HTTP_BAD_REQUEST, "Bad Request: Too many headers");

            free_http_request(parsed_request);

            return NULL;
        }

        *current_header = parse_http_header(line);

        if (*current_header == NULL)
        {
            if (LOGGING)
                perror(RED "error in parse_http_header" RESET);

            send_error_response(client_socket, HTTP_INTERNAL_SERVER_ERROR, NULL);

            free_http_request(parsed_request);

            return NULL;
        }

        line = strtok_r(NULL, CRLF, &state);

        current_header++;
        header_count++;
    }

    *current_header = NULL;

    parsed_request->body = parse_http_body(body_delimiter + BODY_DELIM_LENGTH);

    return parsed_request;
}

/**
 * Prints the contents of an http_request struct.
 * @param parsed_request The http_request struct to print.
 */
void print_request(http_request *parsed_request)
{
    printf("______________________________________________________________\n");
    printf("Method: %s\n", parsed_request->method);
    printf("URL: %s\n", parsed_request->url);
    printf("Version: %s\n", parsed_request->version);

    http_header **current_header = parsed_request->http_headers;

    while (current_header != NULL && *current_header != NULL && (*current_header)->name != NULL)
    {
        printf("Header %s => %s\n", (*current_header)->name, (*current_header)->value);

        current_header++;
    }

    if (parsed_request->body != NULL)
    {
        printf("Body: %s\n", parsed_request->body);
    }

    printf("______________________________________________________________\n");
}

/**
 * frees the memory allocated for an http_request struct.
 * @param request The http_request struct to free.
 * @return 0 on success.
 */
void *free_http_response(http_response *response)
{
    if (response == NULL)
    {
        return NULL;
    }

    if (response->http_response_headers != NULL)
    {
        http_response_header **curr_http_header = response->http_response_headers;

        while (*curr_http_header != NULL)
        {
            x_free(*curr_http_header);
            curr_http_header++;
        }

        x_free(response->http_response_headers);
    }

    if (response->body != NULL)
    {
        x_free(response->body);
    }

    x_free(response);

    return NULL;
}

/**
 * Validates an HTTP method.
 * @param method The HTTP method to validate.
 * @return 0 if valid, -1 otherwise.
 */
x_bool validate_http_method(char *method)
{
    char *valid_methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "TRACE", "CONNECT", NULL};

    for (int i = 0; valid_methods[i] != NULL; i++)
    {
        if (strcmp(method, valid_methods[i]) == 0)
        {
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
x_bool validate_http_version(char *version)
{
    if (strcmp(version, "HTTP/1.1") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

/**
 * Validates an HTTP URL.
 * @param url The HTTP URL to validate.
 * @return TRUE (1) if valid, FALSE (0) otherwise.
 */
x_bool validate_http_url(char *url)
{
    if (url == NULL || url[0] != '/' || strlen(url) > MAX_URL_LENGTH)
    {
        return FALSE;
    }

    return TRUE;
}
