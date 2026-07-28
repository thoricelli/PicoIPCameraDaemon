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
    static void cameraOpened(void *context, ACameraDevice *device);
    static void cameraDisconnected(void *context, ACameraDevice *device);
    static void cameraError(void *context, ACameraDevice *device, int error);

    ACameraDevice *activeCamera = nullptr;
    AImageReader *imageReader = nullptr;

    ACameraCaptureSession *captureSession = nullptr;
    ACaptureRequest *captureRequest = nullptr;

    AImageReader_ImageListener *imageListener = nullptr;
};