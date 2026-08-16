#pragma once

#include <thread>
#include <atomic>

#include "data-buffer.hpp"
#include "interfaces/ICamera.hpp"
#include "feed.hpp"

class CameraSharedMemory : public ICamera
{
public:
    CameraSharedMemory(Feed *feed);
    bool openCamera();
    bool closeCamera();
    bool isOpen();

private:
    void PollSharedMemory();

    Feed *feed;
    DataBuffer *dataBuffer = nullptr;

    std::atomic<bool> cameraOpen = {false};
    std::thread worker;
};