#include "../../include/signals.h"
#include "../../include/constants.h"

volatile sig_atomic_t g_shutdown_flag = 0;

/**
 * The parent's handler for SIGINT.
 * Sets a flag to indicate shutdown.
 * @param sig Signal number (unused).
 */
void handle_sigint(int sig)
{
    g_shutdown_flag = 1;
}

/**
 * The child's handler for SIGTERM.
 * Exits immediately.
 * @param sig Signal number (unused).
 */
void handle_sigterm(int sig)
{
    _exit(0);
}

/**
 * Sets up a signal handler for the specified signal.
 * @param sig Signal number to handle.
 * @param handler Function pointer to the handler function.
 */
void setup_signal_handler(int sig, void (*handler)(int))
{
    struct sigaction sa;

    g_shutdown_flag = 0;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(sig, &sa, NULL) == -1)
    {
        if (LOGGING)
            perror(RED "error in setting up signal handler" RESET);

        exit(1);
    }
}