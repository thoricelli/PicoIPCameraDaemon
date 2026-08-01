#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

#include "leds.hpp"

#define AW21009_DRIVER "/sys/bus/i2c/drivers/aw210xx_led/2-0020/leds/aw210xx_led/"
#define HWEN_DEVICE_ATTR AW21009_DRIVER "hwen"
#define REG_DEVICE_ATTR AW21009_DRIVER "reg"

int Leds::turnOn(LedSettings *settings)
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // We need to write to the AW21009 registers
    this->regFd = std::fopen(REG_DEVICE_ATTR, "w");

    if (regFd == nullptr)
    {
        std::perror("Failed to open AW21009 REG!");

        return EXIT_FAILURE;
    }

    this->writeToRegister(RESET_REG, 0x00);

    // Enable chip
    this->writeToRegister(GCR_REG, 0x01);

    this->writeToRegister(GCCR_REG, 0xFF);

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

int Leds::turnOff()
{
    FILE *hwenFd = std::fopen(HWEN_DEVICE_ATTR, "w");

    if (hwenFd == nullptr)
    {
        std::perror("Failed to open HWEN!");
        return EXIT_FAILURE;
    }

    std::fputs("3", hwenFd);

    std::fclose(hwenFd);

    return EXIT_SUCCESS;
}

void Leds::setLedBrightness(LedIndex led, unsigned int brightness)
{
    if ((led == LED_LEFT_EYE || led == LED_RIGHT_EYE) && brightness > MAXIMUM_EYE_BRIGHTNESS)
        brightness = MAXIMUM_EYE_BRIGHTNESS;

    if (led == FACE_LED_ID && brightness > MAXIMUM_FACE_BRIGHTNESS)
        brightness = MAXIMUM_FACE_BRIGHTNESS;

    writeToRegister(SR_START + (led * 3) + 2, brightness);
}

int Leds::writeToRegister(char reg, char value)
{
    char command[20];
    sprintf(command, "0x%x 0x%x", reg, value);

    int bytes_written = std::fputs(command, regFd);

    std::fflush(regFd);
    std::rewind(regFd);

    if (bytes_written < 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}