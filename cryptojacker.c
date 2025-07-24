#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/prctl.h>

// Global thread control
volatile int keep_running = 1;
int num_threads = 1;
pthread_t *threads = NULL;

void* burn_cpu(void* arg) {
    while (keep_running) {
        asm volatile("" ::: "memory"); // Prevent compiler optimizations
    }
    return NULL;
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent exits

    // Child continues
    if (setsid() < 0) exit(EXIT_FAILURE); // Create new session
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // First child exits

    umask(0);
    chdir("/");

    // Close std file descriptors
    for (int i = 0; i < 3; i++) close(i);
}

void handle_signal(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        keep_running = 0;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    num_threads = atoi(argv[1]);
    if (num_threads < 1 || num_threads > 256) {
        fprintf(stderr, "Invalid number of threads.\n");
        return 1;
    }

    prctl(PR_SET_NAME, "crypto_daemon", 0, 0, 0);


    // Setup signal handler
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // Daemonize the process
    daemonize();

    // write PID to a hidden file
    FILE *pidfile = fopen("/tmp/.cryptojacker.pid", "w");
    if (pidfile) {
        fprintf(pidfile, "%d\n", getpid());
        fclose(pidfile);
    }

    threads = malloc(sizeof(pthread_t) * num_threads);
    for (int i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], NULL, burn_cpu, NULL);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    return 0;
}
