#pragma once

#include <vector>
#include <mutex>
#include <turbojpeg.h>

using FrameBuffer = std::shared_ptr<const std::vector<unsigned char>>;

enum LogicalCameraId
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

    void setLeftEyeBufferState(bool state);
    void setRightEyeBufferState(bool state);
    void setFaceBufferState(bool state);

    std::mutex leftEyeMutex;
    std::condition_variable leftEyeCv;

    std::mutex rightEyeMutex;
    std::condition_variable rightEyeCv;

    std::mutex faceMutex;
    std::condition_variable faceCv;

    unsigned long leftEyeFrameNumber = 0;
    unsigned long rightEyeFrameNumber = 0;
    unsigned long faceFrameNumber = 0;

private:
    // Feeds
    FrameBuffer leftEyeJpegBuffer;
    FrameBuffer rightEyeJpegBuffer;
    FrameBuffer faceJpegBuffer;

    bool leftEyeBufferState = false;
    bool rightEyeBufferState = false;
    bool faceBufferState = false;

    std::vector<unsigned char> cropImage(LogicalCameraId cameraId, unsigned char *pixels);
};