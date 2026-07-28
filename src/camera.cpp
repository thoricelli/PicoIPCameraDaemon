#include <iostream>

#include "camera.hpp"
#include "feed.hpp"
#include "log.hpp"

// TODO.
static Feed *feed;

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

Camera::Camera(Feed *f)
{
    feed = f;
}

void Camera::open()
{
    ACameraManager *cameraManager = ACameraManager_create();

    this->cameraStateCallbacks = {
        .context = this,
        .onDisconnected = [](void *context, ACameraDevice *device)
        {
            auto *self = static_cast<Camera *>(context);
            self->onDisconnected(device);
        },
        .onError = nullptr,
    };

    camera_status_t cameraStatus = ACameraManager_openCamera(cameraManager, "5", &cameraStateCallbacks, &this->activeCamera);

    media_status_t mstatus = AImageReader_new(400, 1600, AIMAGE_FORMAT_RAW_PRIVATE, 2, &this->imageReader);

    mstatus = AImageReader_setImageListener(this->imageReader, &mImageListener);

    ANativeWindow *nativeWindow = nullptr;
    mstatus = AImageReader_getWindow(this->imageReader, &nativeWindow);

    ACaptureSessionOutputContainer *outputContainer = nullptr;
    ACaptureSessionOutputContainer_create(&outputContainer);

    ACaptureSessionOutput *sessionOutput = nullptr;
    ACaptureSessionOutput_create(nativeWindow, &sessionOutput);
    ACaptureSessionOutputContainer_add(outputContainer, sessionOutput);

    camera_status_t captureStatus = ACameraDevice_createCaptureRequest(this->activeCamera, TEMPLATE_RECORD, &this->captureRequest);

    int32_t sensitivity = 300;
    ACaptureRequest_setEntry_i32(this->captureRequest, ACAMERA_SENSOR_SENSITIVITY, 1, &sensitivity);

    ACameraOutputTarget *imageReaderTarget;
    ACameraOutputTarget_create(nativeWindow, &imageReaderTarget);
    ACaptureRequest_addTarget(this->captureRequest, imageReaderTarget);

    this->sessionStateCallbacks = {
        .context = nullptr,
        .onClosed = nullptr,
        .onReady = nullptr,
        .onActive = nullptr};

    cameraStatus = ACameraDevice_createCaptureSession(this->activeCamera, outputContainer, &this->sessionStateCallbacks, &this->captureSession);

    camera_status_t reqStatus = ACameraCaptureSession_setRepeatingRequest(this->captureSession, nullptr, 1, &this->captureRequest, nullptr);

    this->isOpened = true;
}

void Camera::onDisconnected(ACameraDevice *device)
{
    this->isOpened = false;
}

void Camera::close()
{
    ACameraDevice_close(this->activeCamera);
    this->isOpened = false;
}