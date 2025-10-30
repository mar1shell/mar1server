#include "../../include/signals.h"

volatile sig_atomic_t g_shutdown_flag = 0;

/*
 * The PARENT'S handler for SIGINT (Ctrl+C).
 * It just sets the flag and does nothing else.
 */
void handle_sigint(int sig)
{
    g_shutdown_flag = 1;
}

/*
 * The CHILD'S handler for SIGTERM.
 * (The parent will also get this, which is fine).
 * We use _exit() because it is async-signal-safe. exit() is not.
 */
void handle_sigterm(int sig)
{
    _exit(0); // Exit immediately
}

// Helper to set up handlers
void setup_signal_handler(int sig, void (*handler)(int))
{
    g_shutdown_flag = 0;
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(sig, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
}