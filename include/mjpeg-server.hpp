#pragma once

#include <thread>

#include "httplib.h"
#include "feed.hpp"
#include "camera.hpp"
#include "leds.hpp"

class MJPEGServer
{
public:
    MJPEGServer();
    void startServerBlocking();

private:
    // Endpoints
    void leftEyeEndpoint(const httplib::Request &req, httplib::Response &res);
    void rightEyeEndpoint(const httplib::Request &req, httplib::Response &res);
    void faceEndpoint(const httplib::Request &req, httplib::Response &res);

    bool writeToSink(httplib::DataSink &sink, FrameBuffer jpeg);
    void ensureCameraOpen();
    void checkCameraShouldStayOpen();

    // TODO seperate this into its own class
    void closeTimerLoop();

    void startCameraCloseTimer();
    void cancelCameraCloseTimer();

    bool timerActive;
    std::chrono::steady_clock::time_point triggerStart;

    std::thread timerThread;

    Feed *feed;
    ICamera *camera;
    Leds *leds;

    int leftEyeListeners = 0;
    int rightEyeListeners = 0;
    int faceListeners = 0;
};