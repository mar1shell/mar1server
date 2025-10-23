#ifndef REQUESTS_H
#define REQUESTS_H

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HEADERS 20

typedef struct {
    char *name;
    char *value;
} http_header;

#define METHOD_SIZE 10
#define URL_SIZE 100
#define VERSION_SIZE 10
#define STATUS_MESSAGE_SIZE 100

typedef struct {
    char *method;
    char *url;
    char *version;
    http_header **http_headers;
    char *body;
} http_request;

http_request *parse_http_request(char *request);
void print_request(http_request *parsed_request);
int free_http_request(http_request *request);

#endif // REQUESTS_H