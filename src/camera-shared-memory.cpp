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

    // I'd like to only open the camera, and not start the algorithm at all, but that's not really possible for a few reasons:
    // 1. I can *open* the camera via the StartPreview or OpenCamera, but when I close it the service will crash if an app is using the FT/ET.
    // 2. Using this, the camera will close automatically if no processes are using it.
    status_t algorithmStatus = PxrEyeTrackingService::StartAlgorithm(5, EYE_TRACKING_ON, 1000);

    if (algorithmStatus != OK)
        return false;

    if (this->dataBuffer != nullptr)
        this->dataBuffer->Close();

    int fd;
    status_t sharedMemoryStatus = PxrEyeTrackingService::GetCameraFrameSharedMemory(5, &fd, &cameraBuffer);

    if (sharedMemoryStatus != OK)
        return false;

    this->dataBuffer = new DataBuffer(cameraBuffer, fd);
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

    this->dataBuffer->Close();
    this->dataBuffer = nullptr;

    return PxrEyeTrackingService::StopAlgorithm(5, EYE_TRACKING_ON) == OK;
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