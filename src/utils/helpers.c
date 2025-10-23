#include "../../include/constants.h"
#include "../../include/helpers.h"


/**
 * Prints the welcome banner to the console.
 */
void printWelcomeBanner() {
    printf(GREEN"███╗   ███╗ █████╗ ██████╗  ██╗███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n"
                "████╗ ████║██╔══██╗██╔══██╗███║██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n"
                "██╔████╔██║███████║██████╔╝╚██║███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n"
                "██║╚██╔╝██║██╔══██║██╔══██╗ ██║╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n"
                "██║ ╚═╝ ██║██║  ██║██║  ██║ ██║███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n"
                "╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n"
                "________________________________________________________________________________\n\n\n"RESET);
    printf("Welcome to the mar1server Server!\n"
            "Server is starting up...\n");
}


/**
 * Extracts and validates the port number from a string.
 * @param port_string The string containing the port number.
 * @return The extracted port number or -1 if invalid.
 */
int extractPort(char *port_string) {
    int port = atoi(port_string);

    if (port <= 0 || port >= 65535) {
        return -1;
    }

    return port;
}