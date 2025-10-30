#include "../include/constants.h"
#include "../include/controller.h"
#include "../include/helpers.h"
#include "../include/main.h"
#include "../include/requests.h"
#include "../include/server.h"
#include "../include/signals.h"
#include "../include/http_status.h"

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

    setup_signal_handler(SIGINT, handle_sigint);
    setup_signal_handler(SIGTERM, handle_sigterm);

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

    struct sockaddr_in client_address;
    int client_socket;

    while (!g_shutdown_flag)
    {
        // accept a client connection and receive data
        client_socket = accept_connection(server->socket, &client_address);

        printf(B_BLUE "##############################################################\n" RESET);

        if (client_socket < 0)
        {
            if (errno == EINTR && g_shutdown_flag)
            {

                printf(BLUE "##############################################################\n" RESET);

                break;
            }

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
            close(server->socket);
            server = x_free(server);

            signal(SIGINT, SIG_IGN);

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

            struct timeval timeout;
            timeout.tv_sec = 30; // 30 second timeout
            timeout.tv_usec = 0;

            if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO,
                           &timeout, sizeof(timeout)) < 0)
            {
                perror(RED "setsockopt failed" RESET);
            }

            ssize_t bytes_received;

            bytes_received = recv(client_socket, request, MAX_REQUEST_SIZE - 1, 0);

            if (bytes_received < 0)
            {

                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    if (LOGGING)
                        fprintf(stderr, YELLOW "recv timeout\n" RESET);

                    send_error_response(client_socket, HTTP_REQUEST_TIMEOUT, "Request Timeout");
                }
                else if (LOGGING)
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
                    perror(YELLOW "error in handling request" RESET);
            }
            else
            {
                if (LOGGING)
                    printf(B_WHITE "request handled successfully" RESET "\n");
            }

            // cleanup
            free_http_request(parsed_request);
            close(client_socket);

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

    printf(B_WHITE "Shutting down the mar1server...\n\n" RESET);

    signal(SIGTERM, SIG_IGN);
    killpg(0, SIGTERM);
    signal(SIGTERM, handle_sigterm);

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ; // wait for all child processes

    printGoodbyeBanner();

    // cleanup
    close(server->socket);
    server = x_free(server);

    return 0;
}
