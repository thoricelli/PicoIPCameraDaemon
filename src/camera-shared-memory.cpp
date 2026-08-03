#include "camera-shared-memory.hpp"
#include "pico/eye-tracking-service.hpp"

CameraSharedMemory::CameraSharedMemory(Feed *feed)
{
    this->feed = feed;
}

bool CameraSharedMemory::open()
{
    int fd = -2;
    int status = EyeTrackingService::GetCameraFrameSharedMemory(5, &fd);

    printf("Got status: %d, got fd: %d", status, fd);

    return true;
}

bool CameraSharedMemory::close()
{
    return true;
}

bool CameraSharedMemory::isOpen()
{
    return true;
}