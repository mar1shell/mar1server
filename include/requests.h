#ifndef REQUESTS_H
#define REQUESTS_H

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HEADERS 20
#define MAX_URL_LENGTH 200

typedef struct {
    char *name;
    char *value;
} http_header;

typedef struct {
    char *method;
    char *url;
    char *version;
    http_header **http_headers;
    char *body;
} http_request;

http_request *parse_http_request(char *request);
void print_request(http_request *parsed_request);
void free_http_request(http_request *request);

#endif // REQUESTS_H