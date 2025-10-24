#include "../../include/constants.h"
#include "../../include/requests.h"
#include "../../include/helpers.h"

/**
 * Retrieves the value of a specific header from an array of HTTP headers.
 * @param http_headers The array of HTTP headers.
 * @param name The name of the header to retrieve.
 * @return A pointer to the value of the header, or NULL if not found.
 */
char *get_header_value(http_header **http_headers, char *name) {
    http_header **curr_header = http_headers;

    while (*curr_header != NULL) {
        if (strcasecmp((*curr_header)->name, name) == 0) {
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
char *parse_http_body(char *raw_body) {
    if (raw_body == NULL) {
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
http_header *parse_http_header(char *raw_http_header) {
    if (raw_http_header == NULL ||raw_http_header[0] == '\0' ) {
        return NULL;
    }

    http_header *parsed_http_header = NULL;

    parsed_http_header = (http_header *)malloc(sizeof(http_header));

    if (parsed_http_header == NULL) {
        if (LOGGING) perror(RED"error in malloc (parse_http_header)"RESET);
        
        return NULL;
    }

    char *colon_pos = strstr(raw_http_header, ": ");

    if (colon_pos == NULL ) {
        if (LOGGING) perror(RED"Invalid HTTP header format"RESET);

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
http_request *parse_http_request(char *request) {
    if (request == NULL || request[0] == '\0') {
        return NULL;
    }

    http_request *parsed_request = NULL;

    parsed_request = (http_request *)malloc(sizeof(http_request));

    if (parsed_request == NULL) {
        if (LOGGING) perror(RED"error in malloc (parse_http_request)"RESET);

        return NULL;
    }

    parsed_request->method = NULL;
    parsed_request->url = NULL;
    parsed_request->version = NULL;
    parsed_request->http_headers = NULL;
    parsed_request->body = NULL;

    char *body_delimiter = strstr(request, BODY_DELIM);

    if (body_delimiter == NULL) {
        if (LOGGING) perror(RED"No body delimiter found in request"RESET);

        parsed_request = x_free(parsed_request);

        return NULL;
    }

    *body_delimiter = '\0';

    parsed_request->body = body_delimiter + BODY_DELIM_LENGTH;

    char *state = NULL;
    char *line = NULL;
    line = __strtok_r(request, CRLF, &state);

    parsed_request->method = strtok(line, " ");
    parsed_request->url = strtok(NULL, " ");
    parsed_request->version = strtok(NULL, " ");

    line = __strtok_r(NULL, CRLF, &state);

    parsed_request->http_headers = (http_header **)malloc(sizeof(http_header *) * MAX_HEADERS);

    if (parsed_request->http_headers == NULL) {
        if (LOGGING) perror(RED"error in malloc (parse_http_request)"RESET);

        parsed_request = x_free(parsed_request);
        
        return NULL;
    }

    http_header **current_header = NULL;

    current_header = parsed_request->http_headers;
    
    while (line != NULL) {
        // TODO: handle max headers case
        *current_header = parse_http_header(line);
        
        // TODO: handle this
        if (*current_header == NULL) {
            if (LOGGING) perror(RED"error in parse_http_header"RESET);

            free_http_request(parsed_request);

            return NULL;
        }

        line = __strtok_r(NULL, CRLF, &state);
        
        current_header++;
    }

    *current_header = NULL;

    // TODO
    parsed_request->body = parse_http_body(body_delimiter + BODY_DELIM_LENGTH);

    return parsed_request;
}

/**
 * Prints the contents of an http_request struct.
 * @param parsed_request The http_request struct to print.
 */
void print_request(http_request *parsed_request) {
        printf("______________________________________________________________\n");
        printf("Method: %s\n", parsed_request->method);
        printf("URL: %s\n", parsed_request->url);
        printf("Version: %s\n", parsed_request->version);

        http_header **current_header = parsed_request->http_headers;

        while (current_header != NULL && *current_header != NULL && (*current_header)->name != NULL) {
            printf("Header %s => %s\n", (*current_header)->name, (*current_header)->value);

            current_header++;
        }

        if (parsed_request->body != NULL) {
            printf("Body: %s\n", parsed_request->body);
        }

        printf("______________________________________________________________\n");
}

/**
 * frees the memory allocated for an http_request struct.
 * @param request The http_request struct to free.
 * @return 0 on success.
 */
void free_http_request(http_request *request) {
    if (request == NULL) {
        return;
    }

    if (request->http_headers != NULL) {
        http_header **current_header = request->http_headers;

        while (*current_header != NULL) {
            *current_header = x_free(*current_header);

            current_header++;
        }

        if (*current_header != NULL) current_header = x_free(*current_header);

        request->http_headers = x_free(request->http_headers);
    }

    request = x_free(request);
}