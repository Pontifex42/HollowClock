#pragma once
#include "RTClib.h"

void SetupClockTime();
void LoopClockTime();
void SetTime(int hour, int minute, int second = 0);
void GetTime(int &hour, int &minute, int &second);