#include "Remote.h"
#include "ClockTime.h"
#include "PinConfig.h"
/*
   -- New project --

   This source code of graphical user interface
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.8 or later version
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/
     - for ANDROID 4.11.1 or later version;
     - for iOS 1.9.1 or later version;

   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/



//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_BLUETOOTH_NAME "HollowClock"


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 37 bytes
{ 255,5,0,0,0,30,0,16,31,1,1,0,25,78,12,12,2,31,0,7,
21,22,16,21,5,2,26,150,7,21,23,38,20,5,2,26,2 };

// this structure defines all the variables and events of your control interface 
struct {

    // input variables
    uint8_t button_1; // =1 if button pressed, else =0 
    int16_t Stunde;  // 32767.. +32767 
    int16_t Minute;  // 32767.. +32767 

    // other variable
    uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


// #define DEBUG_REMOTE

#ifndef DEBUG_REMOTE
#ifdef DEBUG_PRINTLN
#undef DEBUG_PRINTLN
#define DEBUG_PRINTLN(a)
#undef DEBUG_PRINT
#define DEBUG_PRINT(a)
#endif
#endif


#define MILLIS_OF_ACTIVE_BT (1000 * 60 * 3) // BT remains active for 3 minutes after boot, than shut down to save battery
#define IS_CONNECTED (RemoteXY.connect_flag == 1)
RTC_DATA_ATTR bool isPowerupPhase = true; // Give a chance to set clock by bluetooth only a minute after start
unsigned long TimeOfBoot;
// for correct sleep-wakeup behaviour: do not shut down before isPowerupPhase is false!

void SetupRemote()
{
    if (!isPowerupPhase)  // No more remote control after first minute
        return;

    DEBUG_PRINTLN("SetupRemote");

    RemoteXY_Init();
    TimeOfBoot = millis();
}

void LoopRemote()
{
    if (!isPowerupPhase)  // No more remote control after first minute
        return;

    RemoteXY_Handler();

    if (!IS_CONNECTED)
    {
        if ((millis() - TimeOfBoot) > MILLIS_OF_ACTIVE_BT)
        {
            isPowerupPhase = false;
            btStop();
        }
        return;
    }

    TimeOfBoot = millis(); // if connected, reset timer for a later reconnecting

    static int hasBeeenZero = false;
    if (!hasBeeenZero && (RemoteXY.button_1 == 0))
        hasBeeenZero = true;
    if (RemoteXY.button_1 == 1)
    {
        if (!hasBeeenZero)
            return;
        hasBeeenZero = false;

        int h = RemoteXY.Stunde;
        int m = RemoteXY.Minute;
        while (h < 0)
            h += 12;

        if (h >= 12)
            h %= 12;

        while (m < 0)
            m += 60;

        if (m >= 60)
            m %= 60;

        SetTime(h, m);
    }
}
