#include "../../include/server.h"
#include "../../include/constants.h"

/**
 * Initializes the HTTP server by creating a socket and binding it to the specified port.
 *  and start listening on that port;
 * @param port The port number to bind the server to.
 * @return A pointer to the initialized http_server structure, or NULL on failure.
 * @attention Remember to free the allocated memory for http_server when done.
 */
http_server *init_http_server(int port) {
    http_server *server = NULL;

    server = (http_server *)malloc(sizeof(http_server));

    if (server == NULL) {
        if (LOGGING) perror(RED"problem in malloc (init_http_server)"RESET);
        return NULL;
    }

    server->port = port;
    server->socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server->socket < 0) {
        if (LOGGING) perror("problem in socket creation (init_http_server)");
        free(server);
        return NULL;
    }

    // Allow reuse of the address
    int opt = 1;
    int result = setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (result < 0) {
        if (LOGGING) perror("problem in setsockopt (init_http_server)");
        free(server);
        return NULL;
    }

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server->socket, (struct sockaddr *)&server_address, sizeof(server_address));

    listen(server->socket, BACKLOG);

    printf("Server listening on port " B_BLUE"%d\n"RESET, port);

    return server;
}

/**
 * Accepts a client connection on the given socket.
 * @param socket The server socket to accept connections on.
 * @param socket_address Pointer to a sockaddr_in structure to store the client's address.
 * @return The client socket descriptor, or -1 on failure.
 */
int accept_connection(int socket, struct sockaddr_in *socket_address) {
    socklen_t client_address_len = sizeof(socket_address);

    return accept(socket, (struct sockaddr *)socket_address, &client_address_len);
}