#include <sys/stat.h>
#include <unistd.h>

#include "camera-shared-memory.hpp"
#include "pvr/pxr-eye-tracking-service.hpp"

CameraSharedMemory::CameraSharedMemory(Feed *feed)
{
    this->feed = feed;
}

bool CameraSharedMemory::openCamera()
{
    void *cameraBuffer = nullptr;

    status_t algorithmStatus = PxrEyeTrackingService::StartAlgorithm(5, EYE_TRACKING_ON | FACE_TRACKING_ON, 1000);

    if (algorithmStatus != OK)
        return false;

    if (this->dataBuffer == nullptr)
    {
        status_t sharedMemoryStatus = PxrEyeTrackingService::GetCameraFrameSharedMemory(5, &cameraBuffer);

        if (sharedMemoryStatus != OK)
            return false;

        this->dataBuffer = new DataBuffer(cameraBuffer);
    }
    this->cameraOpen.store(true);

    // Spin up a new thread that will start sending data, and dies after it receives a signal.
    this->worker = std::thread(&CameraSharedMemory::PollSharedMemory, this);

    return true;
}

bool CameraSharedMemory::closeCamera()
{
    this->cameraOpen = false;

    if (this->worker.joinable())
        this->worker.join();

    return PxrEyeTrackingService::StopAlgorithm(5, EYE_TRACKING_ON | FACE_TRACKING_ON) == OK;
}

bool CameraSharedMemory::isOpen()
{
    return this->cameraOpen;
}

void CameraSharedMemory::PollSharedMemory()
{
    while (this->cameraOpen.load())
    {
        void *cameraFrameBuffer = this->dataBuffer->GetLatest();

        if (cameraFrameBuffer != nullptr)
        {
            unsigned char *cameraFrame = static_cast<unsigned char *>(cameraFrameBuffer) + 151; // 151 bytes are data, which I have yet to figure out what it is.

            this->feed->pushImageData(cameraFrame);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}