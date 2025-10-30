#include "../include/constants.h"
#include "../include/controller.h"
#include "../include/helpers.h"
#include "../include/main.h"
#include "../include/requests.h"
#include "../include/server.h"

/**
 * Main function to start the HTTP server.
 * @param argc Argument count from command line.
 * @param argv Argument vector from command line.
 * @return Exit status code.
 */
int main(int argc, char const *argv[])
{
    // TODO: add config files, and also validate files like 404 and index.html existence

    printWelcomeBanner();

    if (argc > 2)
    {
        printf(B_WHITE "Usage: %s [port]\n" RESET, argv[0]);

        return 1;
    }

    int port = PORT;

    if (argc == 2)
    {
        port = extractPort((char *)argv[1]);

        if (port < 0)
        {
            perror(RED "Invalid port number. Please provide a port between 1 and 65534.\n" RESET);

            return 1;
        }
    }

    // initialize the HTTP server
    http_server *server = init_http_server(port);

    if (server == NULL)
    {
        if (LOGGING)
            perror(RED "Failed to initialize server.\n" RESET);

        return 1;
    }

    int client_socket;
    struct sockaddr_in client_address;

    while (TRUE)
    {
        // accept a client connection and receive data
        client_socket = accept_connection(server->socket, &client_address);

        printf(B_BLUE "##############################################################\n" RESET);

        if (client_socket < 0)
        {
            if (LOGGING)
                perror(RED "error in accept" RESET);

            break;
        }

        // fork a new process to handle the client request
        pid_t pid = fork();

        if (pid < 0)
        {
            if (LOGGING)
                perror(RED "error in fork" RESET);

            close(client_socket);
            close(server->socket);
            server = x_free(server);

            break;
        }

        // child process
        if (pid == 0)
        {
            char *request = (char *)malloc(MAX_REQUEST_SIZE);

            if (request == NULL)
            {
                if (LOGGING)
                    perror(RED "error in malloc for request" RESET);

                close(client_socket);
                close(server->socket);
                server = x_free(server);

                printf(B_BLUE "##############################################################\n" RESET);

                exit(1);
            }

            ssize_t bytes_received;

            bytes_received = recv(client_socket, request, MAX_REQUEST_SIZE, 0);

            if (bytes_received < 0)
            {
                if (LOGGING)
                    perror(RED "error in recv" RESET);

                close(client_socket);
                close(server->socket);
                server = x_free(server);

                printf(B_BLUE "##############################################################\n" RESET);

                exit(1);
            }

            request[bytes_received] = '\0'; // null-terminate the received string

            printf("______________________________________________________________\n");
            printf("Received %ld bytes:\n%s\n", bytes_received, request);
            printf("______________________________________________________________\n");

            http_request *parsed_request = parse_http_request(client_socket, request);

            print_request(parsed_request);

            int status = handle_request(client_socket, parsed_request);

            if (status < 0)
            {
                if (LOGGING)
                    perror(RED "error in handling request" RESET);
            }
            else
            {
                if (LOGGING)
                    printf(B_WHITE "request handled successfully" RESET "\n");
            }

            // cleanup
            free_http_request(parsed_request);
            close(client_socket);
            close(server->socket);
            server = x_free(server);

            printf(B_BLUE "##############################################################\n" RESET);

            exit(0);
        }
        else
        { // parent process
            close(client_socket);

            // wait for child processes to prevent zombies
            waitpid(pid, NULL, WNOHANG);
        }
    }

    // cleanup
    close(server->socket);
    server = x_free(server);

    return 0;
}
