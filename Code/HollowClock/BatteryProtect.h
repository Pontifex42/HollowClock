#pragma once

void GoToSleep();
void GoShutDown();

void SetupBatteryProtect();
void LoopBatteryProtect();
bool GetBatteryAlarmState();
bool HandleBatteryAlarm();

// measured values with actual resistors
#define DIGITS_PER_VOLT 250.0

