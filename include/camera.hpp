#pragma once

#include <camera/NdkCameraCaptureSession.h>

class Camera
{
public:
    void open();

private:
    static void cameraOpened(void *context, ACameraDevice *device);
    static void cameraDisconnected(void *context, ACameraDevice *device);
    static void cameraError(void *context, ACameraDevice *device, int error);
};