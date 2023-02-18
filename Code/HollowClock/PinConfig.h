#pragma once
#include "Arduino.h"

#define PIN_M1 0	// Servo 1. Not a bootstrap pin.
#define PIN_M2 1	// Servo 2
#define PIN_M3 3	// Servo 3
#define PIN_M4 2	// Servo 4
#define PIN_M5 9	// Servo 5. Bootstrap for firmware download
#define PIN_M6 8	// Servo 6. Bootstrap for booting from SD card
#define PIN_MOT_A	6 // Wired wrong on first revision of board.
#define PIN_MOT_B	7 // Wired wrong on first revision of board.
#define PIN_T1 19
#define PIN_T2 18
#define PIN_T3 4
#define PIN_T4 10

// Overdischarge protection
#define PIN_CHECK_BATT	5

// just to remember that these pins are occupied
// Usable if serial communication is no longer used
#define PIN_SERIAL0_TX 22
#define PIN_SERIAL0_RX 21

//------------------------------------------------------------

// I2C for DS3231 real time clock
#define PIN_SDA PIN_M3
#define PIN_SCL PIN_M4

// Stepper driver ULN2003N attached to these pins:
#define PIN_STEPPER_1 PIN_T1
#define PIN_STEPPER_2 PIN_T2
#define PIN_STEPPER_3 PIN_T3
#define PIN_STEPPER_4 PIN_T4


#define DEBUG_SERIAL Serial
#define DEBUG_PRINTLN(a) DEBUG_SERIAL.println(a)
#define DEBUG_PRINT(a) DEBUG_SERIAL.print(a)
