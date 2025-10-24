#include "../include/constants.h"
#include "../include/helpers.h"
#include "../include/helpers.h"
#include "../include/server.h"
#include "../include/requests.h"
#include "../include/main.h"


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
            perror(RED"Invalid port number. Please provide a port between 1 and 65534.\n"RESET);
            return 1;
        }
    }

    // must be freed
    http_server *server = init_http_server(port);

    if (server == NULL) {
        perror(RED"Failed to initialize server.\n"RESET);
        return 1;
    }

    int client_socket;
    struct sockaddr_in client_address;

    while (TRUE) {
        // accept a client connection and receive data
        client_socket = accept_connection(server->socket, &client_address);

        if (client_socket < 0) {
            if (LOGGING) perror("error in accept");
            break;
        }

        pid_t pid = fork();


        if (pid < 0) {
            if (LOGGING) perror("error in fork");
            close(client_socket);
            continue;
        }

        if (pid == 0) {
            char request[BUFSIZ];

            int bytes_received = recv(client_socket, request, sizeof(request), 0);

            if (bytes_received< 0) {
                if (LOGGING) perror("error in recv");
                close(client_socket);
                close(server->socket);
                x_free(&server);
                exit(1);
            }

            printf("______________________________________________________________\n");
            printf("Received %d bytes:\n%s\n", bytes_received, request);
            printf("______________________________________________________________\n");

            http_request *parsed_request = parse_http_request(request);

            print_request(parsed_request);

            char *response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, world!";

            int bytes_sent = send(client_socket, response, strlen(response), 0);

            if (bytes_sent < 0) {
                if (LOGGING) perror("error in send");
                close(client_socket);
                free_http_request(parsed_request);
                x_free(&server);
                continue;
            }

            printf("Sent %d bytes:\n%s\n", bytes_sent, response);

            free_http_request(parsed_request);
            close(client_socket);
            close(server->socket);
            x_free(&server);
            exit(0);
        } else {
            close(client_socket);

            // wait for child processes to prevent zombies
            waitpid(pid, NULL, WNOHANG);    
        }
    }
    
    close(server->socket);
    x_free(&server);

    return 0;
}


