#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "constants.h"

#define HTTP_VERSION "HTTP/1.1"
#define MAX_RESPONSE_HEADERS 10
#define STATUS_CODE_SIZE 6
#define INITIAL_RESPONSE_SIZE 1024

typedef struct {
    char *name;
    char *value;
} http_response_header;

typedef struct {
    http_response_header **http_response_headers;
    int status_code;
    const char *status_message;
    char *body;
} http_response;

typedef struct {
    int code;
    const char *message;
} http_response_status;

char *create_final_response_from_string(http_response **response, char *final_response, size_t final_response_size, const char* body);
char *create_final_response_from_file(http_response **response, char *final_response, size_t final_response_size, FILE *file, char *file_name);
http_response_header **create_basic_response_headers(char *content_type, char *content_length);
const char *get_status_message(int status_code);
ssize_t send_response(int socket, char *response);
void *free_http_response(http_response *response);
bool validate_http_method(char *method);
bool validate_http_version(char *version);

#endif // RESPONSES_H