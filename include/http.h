#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_HEADERS 20

#define HTTP_HEADER_NAME_SIZE 50
#define HTTP_HEADER_VALUE_SIZE 200

typedef struct {
    char name[HTTP_HEADER_NAME_SIZE];
    char value[HTTP_HEADER_VALUE_SIZE];
} http_header;

#define METHOD_SIZE 10
#define URL_SIZE 100
#define VERSION_SIZE 10
#define STATUS_MESSAGE_SIZE 100

typedef struct {
    char method[METHOD_SIZE];
    char url[URL_SIZE];
    char version[VERSION_SIZE];
    http_header **http_headers;
    char *body;
    int size;
} http_request;

typedef struct {
    char version[VERSION_SIZE];
    int status_code;
    char status_message[STATUS_MESSAGE_SIZE];
    http_header **http_headers;
    char *body;
    int size;
} http_response;

http_request *parse_http_request(char *request);
int freeHttprequest(http_request *request);

#endif // HTTP_H
