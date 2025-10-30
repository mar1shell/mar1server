#ifndef REQUESTS_H
#define REQUESTS_H

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

typedef struct
{
    char *name;
    char *value;
} http_header;

typedef struct
{
    char *method;
    char *url;
    char *version;
    http_header **http_headers;
    char *body;
} http_request;

http_request *parse_http_request(int client_socket, char *request);
void print_request(http_request *parsed_request);
void free_http_request(http_request *request);
x_bool validate_http_method(char *method);
x_bool validate_http_version(char *version);
x_bool validate_http_url(char *version);

#endif // REQUESTS_H