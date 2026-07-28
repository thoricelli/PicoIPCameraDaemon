#include <turbojpeg.h>

#include "feed.hpp"
#include "log.hpp"

#define INDIVIDUAL_CAMERA_WIDTH 400
#define INDIVIDUAL_CAMERA_HEIGHT 400

FrameBuffer Feed::waitForLeftEyeBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->leftEyeMutex);
    this->leftEyeCv.wait(lock, [&]
                         { return this->frameNumber != *lastFrame; });
    *lastFrame = this->frameNumber;
    return this->leftEyeJpegBuffer;
}

FrameBuffer Feed::waitForRightEyeBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->rightEyeMutex);
    this->rightEyeCv.wait(lock, [&]
                          { return this->frameNumber != *lastFrame; });
    *lastFrame = this->frameNumber;
    return this->rightEyeJpegBuffer;
}

FrameBuffer Feed::waitForFaceBuffer(unsigned long *lastFrame)
{
    std::unique_lock<std::mutex> lock(this->faceMutex);
    this->faceCv.wait(lock, [&]
                      { return this->frameNumber != *lastFrame; });
    *lastFrame = this->frameNumber;

    return this->faceJpegBuffer;
}

void Feed::pushImageData(unsigned char *pixels)
{
    // Crop and convert to JPEG.
    FrameBuffer left = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(LEFT_EYE, pixels));
    FrameBuffer right = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(RIGHT_EYE, pixels));
    FrameBuffer face = std::make_shared<const std::vector<unsigned char>>(Feed::cropImage(FACE, pixels));

    this->leftEyeJpegBuffer = left;
    this->rightEyeJpegBuffer = right;
    this->faceJpegBuffer = face;

    this->frameNumber++;

    // Notify the different threads.
    this->leftEyeCv.notify_all();
    this->rightEyeCv.notify_all();
    this->faceCv.notify_all();
}

std::vector<unsigned char> Feed::cropImage(logicalCameraId cameraId, unsigned char *pixels)
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
        75,
        TJFLAG_FASTDCT);

    jpeg.assign(buffer, buffer + size);
    tjFree(buffer);

    return jpeg;
}