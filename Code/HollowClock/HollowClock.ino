/*
 Name:		HollowClock.ino
 Created:	13.02.2023 07:31:52
 Author:	Thorsten
*/

#include "Remote.h"
#include "PinConfig.h"
#include "BatteryProtect.h"
#include "Stepper.h"
#include "ClockTime.h"
#include "TimeMove.h"

// #define DEBUG_INO

#ifndef DEBUG_INO
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

#ifdef DEBUG_INO

//Increment boot number and print it every reboot
RTC_DATA_ATTR int bootCount = 0;

// Method to print the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason(esp_sleep_wakeup_cause_t wakeup_reason)
{
    ++bootCount;
    DEBUG_PRINTLN("Boot number: " + String(bootCount));

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0: DEBUG_PRINTLN("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: DEBUG_PRINTLN("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: DEBUG_PRINTLN("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: DEBUG_PRINTLN("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: DEBUG_PRINTLN("Wakeup caused by ULP program"); break;
    default: DEBUG_PRINTLN("Wakeup was not caused by deep sleep: %d\n" + String(wakeup_reason)); break;
    }
}
#endif

void setup()
{
    Serial.begin(115200);
#ifdef DEBUG_INO
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    print_wakeup_reason(wakeup_reason);
#endif
    SetupBatteryProtect();
    SetupRemote();
    SetupStepper();
    SetupClockTime();
    SetupTimeMove();

    DEBUG_PRINTLN("Setup done");
}


void loop()
{
#ifndef DEBUG_INO
    LoopBatteryProtect();
#endif
    LoopStepper(); // may go to deep sleep mode
    LoopRemote();
    LoopTimeMove();
    LoopClockTime(); // just for debug output
}
