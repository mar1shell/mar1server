#include "../include/constants.h"
#include "../include/helpers.h"
#include "../include/helpers.h"
#include "../include/server.h"
#include "../include/http.h"

int main(int argc, char const *argv[]) {
    printWelcomeBanner();

    if (argc > 2) {
        printf(B_WHITE"Usage: %s [port]\n"RESET, argv[0]);
        return 1;
    }

    int port = PORT;

    if (argc == 2) {
        port = extractPort((char *)argv[1]);

        if (port < 0) {
            printf(RED"Invalid port number. Please provide a port between 1 and 65534.\n"RESET);
            return 1;
        }
    }

    // must be freed
    http_server *server = init_http_server(port);

    if (server == NULL) {
        printf(RED"Failed to initialize server.\n"RESET);
        return 1;
    }

    int client_socket = accept(server->socket, NULL, NULL);

    if (client_socket < 0) {
        if (LOGGING) perror("error in accept");
        close(server->socket);
        free(server);
        return 1;
    }

    char request[BUFSIZ];

    int bytes_received = recv(client_socket, request, sizeof(request), 0);

    if (bytes_received< 0) {
        if (LOGGING) perror("error in recv");
        close(client_socket);
        close(server->socket);
        free(server);
        return 1;
    }

    printf("______________________________________________________________\n");
    printf("Received %d bytes:\n%s\n", bytes_received, request);
    printf("______________________________________________________________\n");

    http_request *parsed_request = parse_http_request(request);

    printf("Method: %s\n", parsed_request->method);
    printf("URL: %s\n", parsed_request->url);
    printf("Version: %s\n", parsed_request->version);

    http_header **current_header = parsed_request->http_headers;

    while (current_header != NULL && (*current_header)->name[0] != '\0') {
        printf("Header %s => %s\n", (*current_header)->name, (*current_header)->value);
        current_header++;
    }

    char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, world!";

    int bytes_sent = send(client_socket, response, strlen(response), 0);

    if (bytes_sent < 0) {
        if (LOGGING) perror("error in send");
    } else {
        printf("Sent %d bytes:\n%s\n", bytes_sent, response);
    }

    close(client_socket);
    close(server->socket);
    freeHttprequest(parsed_request);
    free(server);

    return 0;
}


