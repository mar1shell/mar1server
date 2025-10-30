#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <requests.h>

x_bool handle_request(int client_socket, http_request *request);

#endif // CONTROLLER_H
