#pragma once

#include <camera/NdkCameraCaptureSession.h>

#include "feed.hpp"

class Camera
{
public:
    Camera(Feed *feed);
    void open();

private:
    static void cameraOpened(void *context, ACameraDevice *device);
    static void cameraDisconnected(void *context, ACameraDevice *device);
    static void cameraError(void *context, ACameraDevice *device, int error);
};