#include <iostream>
#include <camera/NdkCameraManager.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCaptureRequest.h>
#include <media/NdkImageReader.h>

#include "camera.hpp"
#include "feed.hpp"
#include "log.hpp"

// TODO.
static Feed *feed;

static AImageReader *mImageReader = nullptr;
static ACameraCaptureSession *mCaptureSession = nullptr;
static ACaptureRequest *mCaptureRequest = nullptr;
static AImageReader_ImageListener mImageListener = {
    .context = nullptr,
    .onImageAvailable = [](void *context, AImageReader *reader)
    {
        AImage *latestImage = nullptr;
        AImage *currentImage = nullptr;

        while (AImageReader_acquireLatestImage(reader, &currentImage) == AMEDIA_OK)
        {
            if (latestImage != nullptr)
            {
                AImage_delete(latestImage);
            }
            latestImage = currentImage;
        }

        if (latestImage != nullptr)
        {
            uint8_t *yData = nullptr;
            int yLen = 0;
            AImage_getPlaneData(latestImage, 0, &yData, &yLen);

            feed->pushImageData(yData);

            AImage_delete(latestImage);
        }
    }};
static ACameraCaptureSession_stateCallbacks mSessionStateCallbacks = {
    .context = nullptr,
    .onClosed = nullptr,
    .onReady = nullptr,
    .onActive = nullptr};
static ACameraDevice_StateCallbacks mCameraStateCallbacks = {
    .context = nullptr,
    .onDisconnected = nullptr,
    .onError = nullptr,
};

Camera::Camera(Feed *f)
{
    feed = f;
}

void Camera::open()
{
    ACameraManager *cameraManager = ACameraManager_create();

    printf("Opening the camera...\n");

    ACameraDevice *cameraDevice = nullptr;
    camera_status_t cameraStatus = ACameraManager_openCamera(cameraManager, "5", &mCameraStateCallbacks, &cameraDevice);

    if (cameraStatus != camera_status_t::ACAMERA_OK)
    {
        printf("Wasn't allowed to access cameraserver_service :(");
        return;
    }

    printf("Camera has opened! Huzzah!\n");

    media_status_t mstatus = AImageReader_new(400, 1600, AIMAGE_FORMAT_RAW_PRIVATE, 2, &mImageReader);

    mstatus = AImageReader_setImageListener(mImageReader, &mImageListener);

    ANativeWindow *nativeWindow = nullptr;
    mstatus = AImageReader_getWindow(mImageReader, &nativeWindow);

    ACaptureSessionOutputContainer *outputContainer = nullptr;
    ACaptureSessionOutputContainer_create(&outputContainer);

    ACaptureSessionOutput *sessionOutput = nullptr;
    ACaptureSessionOutput_create(nativeWindow, &sessionOutput);
    ACaptureSessionOutputContainer_add(outputContainer, sessionOutput);

    camera_status_t captureStatus = ACameraDevice_createCaptureRequest(cameraDevice, TEMPLATE_RECORD, &mCaptureRequest);

    int32_t sensitivity = 200;
    ACaptureRequest_setEntry_i32(mCaptureRequest, ACAMERA_SENSOR_SENSITIVITY, 1, &sensitivity);

    ACameraOutputTarget *imageReaderTarget;
    ACameraOutputTarget_create(nativeWindow, &imageReaderTarget);
    ACaptureRequest_addTarget(mCaptureRequest, imageReaderTarget);

    cameraStatus = ACameraDevice_createCaptureSession(cameraDevice, outputContainer, &mSessionStateCallbacks, &mCaptureSession);

    if (cameraStatus != ACAMERA_OK)
    {
        printf("Failed to create capture session. Error: %d\n", cameraStatus);
        return;
    }

    camera_status_t reqStatus = ACameraCaptureSession_setRepeatingRequest(mCaptureSession, nullptr, 1, &mCaptureRequest, nullptr);
}