// -----------------------------------------------------------------------------------
// axis servo TMC2209 stepper motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef SERVO_TMC2209_PRESENT

#include "../ServoDriver.h"

// default settings for any TMC UART drivers that may be present
#ifndef SERIAL_TMC
  #define SERIAL_TMC                  SoftSerial     // Use software serial w/ TX on M3 (CS) and RX on M4 (MISO) of each axis
#endif
#ifndef SERIAL_TMC_BAUD
  #define SERIAL_TMC_BAUD             115200         // Baud rate
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
  #define SERIAL_TMC_ADDRESS_MAP(x)   (0)            // driver addresses are 0 for all axes
#endif

#if SERIAL_TMC == SoftSerial
  #include <SoftwareSerial.h> // must be built into the board libraries
#endif

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

typedef struct ServoTmcPins {
  int16_t step;
  int16_t dir;
  int16_t enable;
  uint8_t enabledState;
  int16_t m0;
  int16_t m1;
  int16_t fault;
} ServoTmcPins;

typedef struct ServoTmcSettings {
  int16_t model;
  int8_t  status;
  int32_t velocityMax;   // maximum velocity in steps/second
  int32_t acceleration;  // acceleration steps/second/second
  int16_t microsteps;
  int16_t current;
} ServoTmcSettings;

class ServoTmc2209 : public ServoDriver {
  public:
    // constructor
    ServoTmc2209(uint8_t axisNumber, const ServoTmcPins *Pins, const ServoTmcSettings *TmcSettings);

    // decodes driver model and sets up the pin modes
    void init();

    // secondary way to power down, but using the enable pin in this case
    void enable(bool state);

    // power level to the motor
    float setMotorVelocity(float power);

    // update status info. for driver
    void updateStatus();

    // calibrate the motor if required
    void calibrate();

    const ServoTmcSettings *Settings;

  private:
    inline float mAToCs(float mA) { return 32.0F*(((mA/1000.0F)*(rSense+0.02F))/0.325F) - 1.0F; }
    float rSense = 0.11F;

    bool stealthChop() { 
      if ((axisNumber == 1 && AXIS1_DRIVER_DECAY == AXIS1_DRIVER_DECAY_GOTO && AXIS1_DRIVER_DECAY == STEALTHCHOP) ||
         (axisNumber == 2 && AXIS2_DRIVER_DECAY == AXIS2_DRIVER_DECAY_GOTO && AXIS2_DRIVER_DECAY == STEALTHCHOP)) return true; else return false;
    }

    #if SERIAL_TMC == SoftSerial
      SoftwareSerial SerialTMC;
    #endif

    TMC2209Stepper *driver;

    bool powered = false;
    float currentVelocity = 0.0F;
    float acceleration;
    float accelerationFs;
    const ServoTmcPins *Pins;
};

#endif
