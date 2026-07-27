#pragma once
#include <cstdio>

// REGISTERS

/**
 * Global Control Register
 */
#define GCR_REG 0x20
/**
 * Brightness Control Register
 */
#define BCR_REG_START 0x21
/**
 * Update Register
 */
#define UPDATE_REG 0x45
/**
 * Scaling Register
 */
#define SR_START 0x46
/**
 * Global Current Control Register
 */
#define GCCR_REG 0x58
/**
 * Under Voltage Lockout Control Register
 */
#define UVCR_REG 0x60
/**
 * Global Control Register 2
 */
#define GCR2_REG 0x61
/**
 * Reset Register
 */
#define RESET_REG 0x70

// LED ID
#define FACE_LED_ID 0
#define R_EYE_LED_ID 1
#define L_EYE_LED_ID 2

struct led_settings
{
    /**
     * Sets the current of the face LED's in mA
     */
    char faceBrightness;
    /**
     * Sets the current of the right eye LED's in mA
     */
    char rightEyeBrightness;
    /**
     * Sets the current of the left eye LED's in mA
     */
    char leftEyeBrightness;
};

class Leds
{
public:
    int turnOn(led_settings *settings);
    // TODO custom scale / brightness SET for face and eyes.
    int turnOff();

private:
    FILE *regFd;

    int writeToRegister(char reg, char value);
};