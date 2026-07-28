#pragma once

#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>

#include "feed.hpp"

class Camera
{
public:
    Camera(Feed *feed);
    void open();
    void close();

    bool isOpened = false;

private:
    ACameraDevice *activeCamera = nullptr;
    AImageReader *imageReader = nullptr;

    ACameraCaptureSession *captureSession = nullptr;
    ACaptureRequest *captureRequest = nullptr;

    AImageReader_ImageListener *imageListener = nullptr;

    // Callbacks
    ACameraDevice_StateCallbacks cameraStateCallbacks;
    ACameraCaptureSession_stateCallbacks sessionStateCallbacks;

    void onDisconnected(ACameraDevice *device);
};