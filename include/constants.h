#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef unsigned char x_bool;

#include <limits.h>

#define TRUE 1
#define FALSE 0
#define PORT 9001
#define BACKLOG 10
#define LOGGING TRUE
#define CRLF "\r\n"
#define BODY_DELIM "\r\n\r\n"
#define BODY_DELIM_LENGTH 4

#define MAX_HEADERS 20
#define MAX_URL_LENGTH 200
#define MAX_REQUEST_SIZE 16384 // 16 KB
#define INITIAL_REQUEST_SIZE 2048

#define HTTP_VERSION "HTTP/1.1"
#define MAX_RESPONSE_HEADERS 10
#define STATUS_CODE_SIZE 6
#define INITIAL_RESPONSE_SIZE 16384 // 16 KB
#define INITIAL_BODY_SIZE 8192      // 8 KB
#define MAX_RESPONSE_SIZE 1146880   // 1.1 MB
#define MAX_BODY_SIZE 1048576       // 1 MB

#define STATIC_FILES_PATH "/home/mar1shell/Github/mar1server/www"

#define SIZE_T_STRING_SIZE 21 // Maximum length for size_t string representation

#define RESET "\x1b[0m"

/* --- Regular Colors --- */
#define BLACK "\x1b[30m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"

/* --- Bold Colors --- */
#define B_BLACK "\x1b[1;30m"
#define B_RED "\x1b[1;31m"
#define B_GREEN "\x1b[1;32m"
#define B_YELLOW "\x1b[1;33m"
#define B_BLUE "\x1b[1;34m"
#define B_MAGENTA "\x1b[1;35m"
#define B_CYAN "\x1b[1;36m"
#define B_WHITE "\x1b[1;37m"

/* --- Background Colors --- */
#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"

#endif // CONSTANTS_H
