#include "../../include/constants.h"
#include "../../include/helpers.h"

/**
 * Prints the welcome banner to the console.
 */
void printWelcomeBanner()
{
    printf(GREEN "███╗   ███╗ █████╗ ██████╗  ██╗███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n"
                 "████╗ ████║██╔══██╗██╔══██╗███║██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n"
                 "██╔████╔██║███████║██████╔╝╚██║███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n"
                 "██║╚██╔╝██║██╔══██║██╔══██╗ ██║╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n"
                 "██║ ╚═╝ ██║██║  ██║██║  ██║ ██║███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n"
                 "╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n"
                 "________________________________________________________________________________\n\n\n" RESET);
    printf(B_WHITE "Welcome to the mar1server Server!\n"
                   "Server is starting up...\n" RESET);
}

void printGoodbyeBanner()
{
    printf(GREEN "███╗   ███╗ █████╗ ██████╗  ██╗███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n"
                 "████╗ ████║██╔══██╗██╔══██╗███║██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n"
                 "██╔████╔██║███████║██████╔╝╚██║███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n"
                 "██║╚██╔╝██║██╔══██║██╔══██╗ ██║╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n"
                 "██║ ╚═╝ ██║██║  ██║██║  ██║ ██║███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n"
                 "╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n"
                 "________________________________________________________________________________\n\n\n" RESET);
    printf(B_WHITE "Goodbye from the mar1server!\n" RESET);
}

/**
 * Extracts and validates the port number from a string.
 * @param port_string The string containing the port number.
 * @return The extracted port number or -1 if invalid.
 */
int extractPort(char *port_string)
{
    int port = atoi(port_string);

    if (port <= 0 || port >= 65535)
    {
        return -1;
    }

    return port;
}

// function wrappers

/**
 * Wrapper for free function that sets the pointer to NULL after freeing.
 * @param ptr Pointer to pointer of the memory to be freed.
 * @return NULL after freeing the memory.
 */
void *x_free(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }

    return NULL;
}
