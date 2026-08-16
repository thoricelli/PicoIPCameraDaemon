#pragma once

#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>

#include "feed.hpp"
#include "interfaces/ICamera.hpp"

class Camera : public ICamera
{
public:
    Camera(Feed *feed);
    bool openCamera();
    bool closeCamera();
    bool isOpen();

    void setISO(int ISO);

private:
    ACameraDevice *activeCamera = nullptr;
    AImageReader *imageReader = nullptr;

    ACameraCaptureSession *captureSession = nullptr;
    ACaptureRequest *captureRequest = nullptr;

    AImageReader_ImageListener *imageListener = nullptr;

    // Callbacks
    ACameraDevice_StateCallbacks cameraStateCallbacks;
    ACameraCaptureSession_stateCallbacks sessionStateCallbacks;

    std::atomic<bool> opening{false};
    std::atomic<bool> isOpened{false};

    void onDisconnected(ACameraDevice *device);
};