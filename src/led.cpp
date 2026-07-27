#include <cstdio>
#include <cstdlib>

#include "led.hpp"

// I'm not responsible for eye damage if you set this higher!
#define MAX_BRIGHTNESS 0x40 // in mA.

#define AW21009_DRIVER "/sys/bus/i2c/drivers/aw210xx_led/2-0020/leds/aw210xx_led/"
#define HWEN_DEVICE_ATTR AW21009_DRIVER "hwen"
#define REG_DEVICE_ATTR AW21009_DRIVER "reg"

int Led::turnOn(led_settings *settings)
{
    // The GPIO pins control the chip and LED power.
    FILE *hwenFd = std::fopen(HWEN_DEVICE_ATTR, "w");

    if (hwenFd == nullptr)
    {
        std::perror("Failed to open HWEN!");
        return EXIT_FAILURE;
    }

    int bytesWritten = std::fputs("1", hwenFd);
    if (bytesWritten < 0)
    {
        std::perror("Failed to write to HWEN!");

        return EXIT_FAILURE;
    }

    std::fclose(hwenFd);

    // We need to write to the AW21009 registers
    this->regFd = std::fopen(REG_DEVICE_ATTR, "w");

    if (regFd == nullptr)
    {
        std::perror("Failed to open AW21009 REG!");

        return EXIT_FAILURE;
    }

    this->writeToRegister(RESET_REG, 0x01);

    // Enable chip
    this->writeToRegister(GCCR_REG, 0x01);

    // Safety: maximum current for all leds. Face has max current of 0x40.
    this->writeToRegister(GCCR_REG, MAX_BRIGHTNESS);

    // Disable UVLO detect and UVLO protect.
    this->writeToRegister(UVCR_REG, 0x03);

    // Set Single byte mode ON, and RGB mode ON.
    // This way we need less writes for our current.
    this->writeToRegister(GCR2_REG, 0x02);

    for (int i = 0; i < 9; i++)
    {
        // Set the PWM of all LED's to 0xFF, we will change the brightness using good ol' current.
        this->writeToRegister(BCR_REG_START + i, 0xFF);
    }

    // Face
    this->writeToRegister(SR_START + (FACE_LED_ID * 3) + 2, settings->faceBrightness);

    // Eyes
    this->writeToRegister(SR_START + (R_EYE_LED_ID * 3) + 2, settings->rightEyeBrightness);
    this->writeToRegister(SR_START + (L_EYE_LED_ID * 3) + 2, settings->leftEyeBrightness);

    // Write 0 to UPDATE register to put our values into the registers.
    this->writeToRegister(UPDATE_REG, 0x00);

    return EXIT_SUCCESS;
}

int Led::turnOff()
{
    FILE *hwenFd = std::fopen(HWEN_DEVICE_ATTR, "w");

    if (hwenFd == nullptr)
    {
        std::perror("Failed to open HWEN!");
        return EXIT_FAILURE;
    }

    int bytesWritten = std::fputs("0", hwenFd);
    if (bytesWritten < 0)
    {
        std::perror("Failed to write to HWEN!");

        return EXIT_FAILURE;
    }

    std::fclose(hwenFd);

    return EXIT_SUCCESS;
}

int Led::writeToRegister(char reg, char value)
{
    char *command = nullptr;
    sprintf(command, "%x %x", reg, value);

    int bytes_written = std::fputs(command, regFd);

    if (bytes_written < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}