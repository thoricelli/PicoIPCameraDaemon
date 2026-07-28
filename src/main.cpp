#include <iostream>
#include <chrono>
#include <thread>
#include <android/log.h>
#include <unistd.h>

#include "libbinder-ndk.hpp"
#include "mjpeg-server.hpp"

#define IS_A_DAEMON 1

/**
 * Turns this process into a daemon.
 * @return The PID of the daemon.
 */
int detach()
{
    int pid = fork();
    if (pid > 0)
    {
        return pid;
    }

    setsid();

    return EXIT_SUCCESS;
}

int main()
{
#if IS_A_DAEMON == 1
    // Make this process a daemon.
    int pid = detach();

    if (pid > 0)
    {
        printf("Goodbye su! Daemon started on PID: %d\n", pid);
        return EXIT_SUCCESS;
    }
#endif
    // Start a thread pool for listening to events, important!
    LibBinderNdk::ABinderProcess_startThreadPool();

    MJPEGServer *server = new MJPEGServer();
    server->startServerBlocking();

    return EXIT_SUCCESS;
}