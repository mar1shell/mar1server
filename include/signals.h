#ifndef SIGNALS_H
#define SIGNALS_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

extern volatile sig_atomic_t g_shutdown_flag;

void handle_sigint(int sig);
void handle_sigterm(int sig);
void setup_signal_handler(int sig, void (*handler)(int));

#endif // SIGNALS_H
