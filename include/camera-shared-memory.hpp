#pragma once

#include "interfaces/ICamera.hpp"
#include "feed.hpp"

class CameraSharedMemory : public ICamera
{
public:
    CameraSharedMemory(Feed *feed);
    bool open();
    bool close();
    bool isOpen();

private:
    Feed *feed;
};