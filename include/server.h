#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    int socket;
    int port;
} http_server;

http_server *init_http_server(int port);
int accept_connection(int socket, struct sockaddr_in *socket_address);

#endif // SERVER_H
