#pragma once

#include <vector>
#include <mutex>
#include <turbojpeg.h>

using FrameBuffer = std::shared_ptr<const std::vector<unsigned char>>;

enum logicalCameraId
{
    LEFT_EYE,
    RIGHT_EYE,
    FACE
};

/**
 * Receives the raw Y8 image data and notifies listening threads.
 */
class Feed
{
public:
    /**
     * Receives planar data from the camera or any other source and converts it to JPEG then notifies listening threads.
     */
    void pushImageData(unsigned char *pixels);

    // Thread wait functions for buffer.
    FrameBuffer waitForLeftEyeBuffer(unsigned long *lastFrame);
    FrameBuffer waitForRightEyeBuffer(unsigned long *lastFrame);
    FrameBuffer waitForFaceBuffer(unsigned long *lastFrame);

    std::mutex leftEyeMutex;
    std::condition_variable leftEyeCv;

    std::mutex rightEyeMutex;
    std::condition_variable rightEyeCv;

    std::mutex faceMutex;
    std::condition_variable faceCv;

    unsigned long frameNumber = 0;

private:
    // Feeds
    FrameBuffer leftEyeJpegBuffer;
    FrameBuffer rightEyeJpegBuffer;
    FrameBuffer faceJpegBuffer;

    std::vector<unsigned char> cropImage(logicalCameraId cameraId, unsigned char *pixels);
};