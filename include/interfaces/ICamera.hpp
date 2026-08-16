#pragma once

class ICamera
{
public:
    /**
     * Opens this camera.
     * @returns If the camera opened succesfully.
     */
    virtual bool openCamera() = 0;
    /**
     * Closes and cleans up this camera.
     * @returns If the camera has closed succesfully.
     */
    virtual bool closeCamera() = 0;

    /**
     * @returns If the camera is currently open.
     * @note Implementation note: Must be thread safe!
     */
    virtual bool isOpen() = 0;
};