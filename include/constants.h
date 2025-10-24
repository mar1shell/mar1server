#ifndef CONSTANTS_H
#define CONSTANTS_H

typedef unsigned char bool;

#define TRUE 1
#define FALSE 0
#define PORT 9001
#define BACKLOG 10
#define LOGGING TRUE
#define CRLF "\r\n"
#define BODY_DELIM "\r\n\r\n"
#define BODY_DELIM_LENGTH 4

# define RESET   "\x1b[0m"

/* --- Regular Colors --- */
# define BLACK   "\x1b[30m"
# define RED     "\x1b[31m"
# define GREEN   "\x1b[32m"
# define YELLOW  "\x1b[33m"
# define BLUE    "\x1b[34m"
# define MAGENTA "\x1b[35m"
# define CYAN    "\x1b[36m"
# define WHITE   "\x1b[37m"

/* --- Bold Colors --- */
# define B_BLACK   "\x1b[1;30m"
# define B_RED     "\x1b[1;31m"
# define B_GREEN   "\x1b[1;32m"
# define B_YELLOW  "\x1b[1;33m"
# define B_BLUE    "\x1b[1;34m"
# define B_MAGENTA "\x1b[1;35m"
# define B_CYAN    "\x1b[1;36m"
# define B_WHITE   "\x1b[1;37m"

/* --- Background Colors --- */
# define BG_BLACK   "\x1b[40m"
# define BG_RED     "\x1b[41m"
# define BG_GREEN   "\x1b[42m"
# define BG_YELLOW  "\x1b[43m"
# define BG_BLUE    "\x1b[44m"
# define BG_MAGENTA "\x1b[45m"
# define BG_CYAN    "\x1b[46m"
# define BG_WHITE   "\x1b[47m"

#endif // CONSTANTS_H
