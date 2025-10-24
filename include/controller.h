#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <requests.h>

#define STATIC_FILES_PATH "/home/mar1shell/Github/mar1server/www"

bool handle_request(int client_socket, http_request *request);

#endif // CONTROLLER_H
