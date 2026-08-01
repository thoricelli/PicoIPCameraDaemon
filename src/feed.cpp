#include <turbojpeg.h>

#include "feed.hpp"
#include "log.hpp"

#define INDIVIDUAL_CAMERA_WIDTH 400
#define INDIVIDUAL_CAMERA_HEIGHT 400

FrameBuffer Feed::waitForLeftEyeBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->leftEyeMutex);
    this->leftEyeCv.wait(lock, [&]
                         { return this->leftEyeFrameNumber != *lastFrame && this->leftEyeJpegBuffer != nullptr; });
    *lastFrame = this->leftEyeFrameNumber;
    return this->leftEyeJpegBuffer;
}

FrameBuffer Feed::waitForRightEyeBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->rightEyeMutex);
    this->rightEyeCv.wait(lock, [&]
                          { return this->rightEyeFrameNumber != *lastFrame && this->rightEyeJpegBuffer != nullptr; });
    *lastFrame = this->rightEyeFrameNumber;
    return this->rightEyeJpegBuffer;
}

FrameBuffer Feed::waitForFaceBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->faceMutex);
    this->faceCv.wait(lock, [&]
                      { return this->faceFrameNumber != *lastFrame && this->faceJpegBuffer != nullptr;; });
    *lastFrame = this->faceFrameNumber;

    return this->faceJpegBuffer;
}

void Feed::pushImageData(unsigned char *pixels)
{
    // Crop and convert to JPEG.
    FrameBuffer left = nullptr;

    if (this->leftEyeBufferState)
    {
        left = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(LEFT_EYE, pixels));
    }

    FrameBuffer right = nullptr;
    if (this->rightEyeBufferState)
        right = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(RIGHT_EYE, pixels));

    FrameBuffer face = nullptr;
    if (this->faceBufferState)
        face = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(FACE, pixels));

    if (left != nullptr)
    {
        this->leftEyeFrameNumber++;
        this->leftEyeJpegBuffer = left;
        this->leftEyeCv.notify_all();
    }

    if (right != nullptr)
    {
        this->rightEyeFrameNumber++;
        this->rightEyeJpegBuffer = right;
        this->rightEyeCv.notify_all();
    }

    if (face != nullptr)
    {
        this->faceFrameNumber++;
        this->faceJpegBuffer = face;
        this->faceCv.notify_all();
    }
}

std::vector<unsigned char> Feed::cropImage(LogicalCameraId cameraId, unsigned char *pixels)
{
    std::vector<uint8_t> jpeg;

    unsigned char *buffer = nullptr;
    unsigned long size = 0;

    tjhandle tjHandle = tjInitCompress();

    tjCompress2(
        tjHandle,
        pixels + (cameraId * INDIVIDUAL_CAMERA_WIDTH),
        INDIVIDUAL_CAMERA_WIDTH,
        1600,
        INDIVIDUAL_CAMERA_HEIGHT,
        TJPF_GRAY,
        &buffer,
        &size,
        TJSAMP_GRAY,
        80,
        TJFLAG_FASTDCT);

    jpeg.assign(buffer, buffer + size);
    tjFree(buffer);

    return jpeg;
}

void Feed::setLeftEyeBufferState(bool state)
{
    this->leftEyeBufferState = state;
}

void Feed::setRightEyeBufferState(bool state)
{
    this->rightEyeBufferState = state;
}

void Feed::setFaceBufferState(bool state)
{
    this->faceBufferState = state;
}