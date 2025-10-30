#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct
{
    char *name;
    char *value;
} http_response_header;

typedef struct
{
    http_response_header **http_response_headers;
    int status_code;
    const char *status_message;
    char *body;
} http_response;

typedef struct
{
    int code;
    const char *message;
} http_response_status;

char *create_final_response(http_response *response, char **final_response_ptr, size_t final_response_size);
http_response_header **create_basic_response_headers(char *content_type, char *content_length);
const char *get_status_message(int status_code);
ssize_t send_tcp_response(int socket, char *response);
void *free_http_response(http_response *response);
void send_error_response(int client_socket, int status_code, char *custom_message);
char *serve_error_file(int status_code, char **buffer, size_t buffer_size);

#endif // RESPONSES_H