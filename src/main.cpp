#include <iostream>
#include <chrono>
#include <thread>
#include <android/log.h>
#include <unistd.h>

#include "camera.hpp"
#include "mjpeg-server.hpp"
#include "libbinder-ndk.hpp"
#include "leds.hpp"

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

    // Set ourselves to a system UUID to keep cameraserver content.
    setuid(1000);

    Feed *feed = new Feed();

    led_settings ledSettings = {
        .leftEyeBrightness = 0x10,
        .rightEyeBrightness = 0x10,
        .faceBrightness = 0x40,
    };

    Camera *camera = new Camera(feed);
    camera->open();

    Leds *leds = new Leds();
    leds->turnOn(&ledSettings);

    MJPEGServer *server = new MJPEGServer(feed);
    server->startServer();

    while (true)
    {
        // Keep the main thread busy.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return EXIT_SUCCESS;
}