#pragma once

#include <thread>

#include "httplib.h"
#include "feed.hpp"
#include "interfaces/ICamera.hpp"
#include "leds.hpp"

class MJPEGServer
{
public:
    MJPEGServer(ICamera *camera, Feed *feed);
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
    void openCameraLoop();
    void stopCameraOpen();

    void startCameraCloseTimer();
    void cancelCameraCloseTimer();

    bool timerActive;
    std::chrono::steady_clock::time_point triggerStart;

    std::thread timerThread;
    std::thread openCameraThread;

    Feed *feed;
    ICamera *camera;

    std::atomic<bool> isOpening{false};

    std::mutex waitCameraOpenMtx;
    std::condition_variable waitCameraOpenCv;
    bool waitCameraOpen{false};

    int leftEyeListeners = 0;
    int rightEyeListeners = 0;
    int faceListeners = 0;
};