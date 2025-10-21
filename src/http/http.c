#include "../../include/constants.h"
#include "../../include/http.h"



int isHeaderLimit(char *request) {
    return *request == '\r'
            && *(request + 1) == '\n'
            && *(request + 2) == '\r'
            && *(request + 3) == '\n';   
}

// http_header **parse_http_headers(char *http_request, int *header_count) {
//     http_header **parsed_header = (http_header **)malloc(sizeof(http_header));
//     int counter = 0;

//     strlen

//     for ()

// }


/**
 * Parses a raw HTTP request string into an http_request struct.
 * @param request The raw HTTP request string.
 * @return A pointer to the parsed http_request struct.
 * @attention The returned http_request must be freed using freeHttprequest.
 */
http_request *parse_http_request(char *request) {
    http_request *parsed_request = NULL;

    parsed_request = (http_request *)malloc(sizeof(http_request));

    if (parsed_request == NULL) {
        if (LOGGING) perror("error in malloc (parse_http_request)");
        return NULL;
    }

    parsed_request->body = NULL;

    char *line = strtok(request, "\r\n");
    
    sscanf(line, "%s %s %s", parsed_request->method, parsed_request->url, parsed_request->version);

    // TODO: extract function to parse headers and fix body position problem
    parsed_request->http_headers = (http_header **)malloc(sizeof(http_header *) * MAX_HEADERS);

    line = strtok(NULL, "\r\n");

    http_header **current_parsed_header = parsed_request->http_headers;

    while (line != NULL ) {
        http_header *current_header = NULL;
        
        current_header = (http_header *)malloc(sizeof(http_header));

        if (current_header == NULL) {
            if (LOGGING) perror("error in malloc (parse_http_request)");
            return NULL;
        }

        sscanf(line, "%s %s", current_header->name, current_header->value);

        *current_parsed_header = current_header;
        current_parsed_header++;

        line = strtok(NULL, "\r\n");
    }

    *current_parsed_header = (http_header *)malloc(sizeof(http_header));
    (*current_parsed_header)->name[0] = '\0';
    (*current_parsed_header)->value[0] = '\0';

    return parsed_request;
}


/**
 * Frees the memory allocated for an http_request struct.
 * @param request The http_request struct to free.
 * @return 0 on success.
 */
int freeHttprequest(http_request *request) {
    if (request == NULL) {
        return 0;
    }

    if (request->body != NULL) {
        free(request->body);
    }


    if (request->http_headers != NULL) {
        http_header **current_header = request->http_headers;

        while ((*current_header)->name[0] != '\0') {
            free(*current_header);

            current_header++;
        }

        free(*current_header);
        free(request->http_headers);
        free(request);
    }

    return 0;
}