#include "PinConfig.h"
#include "ClockTime.h"
#include <Wire.h>
#include "RTClib.h"

// #define DEBUG_CLOCKTIME

#ifndef DEBUG_CLOCKTIME
#ifdef DEBUG_PRINTLN
#undef DEBUG_PRINTLN
#define DEBUG_PRINTLN(a)
#undef DEBUG_PRINT
#define DEBUG_PRINT(a)
#endif
#endif

RTC_DS3231 rtc;
TwoWire myI2C = TwoWire(0);

void SetTime(int hour, int minute, int second)
{
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    DEBUG_PRINTLN("Adjust time to " + String(hour) + ":" + String(minute) + ":" + String(second));
    DateTime aTime(2023, 1, 1, hour, minute, second);
    rtc.adjust(aTime);
}

void GetTime(int &hour, int &minute, int &second)
{
    DateTime now = rtc.now();
    hour = now.hour();
    minute = now.minute();
    second = now.second();
}

void SetupClockTime()
{
    DEBUG_PRINTLN("SetupClockTime");
#define I2C_FREQ 400000
    myI2C.begin(PIN_SDA, PIN_SCL, I2C_FREQ);
    for (int i = 0; i < 100; ++i)
    {
        if (rtc.begin(&myI2C))
        {
            DEBUG_PRINTLN("RTC works after " + String(i * 10) + " milliseconds.");
            return;
        }
        delay(10);
    }
    if (!rtc.begin(&myI2C))
    {
        DEBUG_PRINTLN("Couldn't find RTC");
        DEBUG_SERIAL.flush();
        abort();
    }
}

void LoopClockTime()
{
#ifdef DEBUG_CLOCKTIME
#define DISPLAY_INTERVAL 2000
    static unsigned long lastTimeDisplayed = 0;

    unsigned long n = millis();
    if ((n - lastTimeDisplayed) < DISPLAY_INTERVAL)
        return;
    lastTimeDisplayed += DISPLAY_INTERVAL;
    DateTime now = rtc.now();
    DEBUG_PRINT(now.hour());
    DEBUG_PRINT(':');
    DEBUG_PRINT(now.minute());
    DEBUG_PRINT(':');
    DEBUG_PRINT(now.second());
    DEBUG_PRINTLN();
#endif
}