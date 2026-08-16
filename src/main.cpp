#include "mjpeg-server.hpp"
#include "camera-shared-memory.hpp"

#include <binder/ProcessState.h>

#define IS_A_DAEMON 0

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
    android::ProcessState::self()->startThreadPool();

    Feed *feed = new Feed();

    MJPEGServer *server = new MJPEGServer(new CameraSharedMemory(feed), feed);
    server->startServerBlocking();

    return EXIT_SUCCESS;
}