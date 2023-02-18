#include "Arduino.h"
#include "PinConfig.h"
#include "BatteryProtect.h"
#include "Stepper.h"
#include "ClockTime.h"

// #define DEBUG_BATTERYPROTECT

#ifndef DEBUG_BATTERYPROTECT
#undef DEBUG_PRINTLN
#undef DEBUG_PRINT
#define DEBUG_PRINTLN(a)
#define DEBUG_PRINT(a)
#endif

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */

void GoShutDown()
{
	DEBUG_PRINTLN("Shutting down for good.");
	DEBUG_SERIAL.flush();

	esp_deep_sleep_start(); // never wake up
}

void GoToSleep()
{
	int tmph, tmpm, tmps;
	GetTime(tmph, tmpm, tmps);
	unsigned long SleepSeconds = 60 - tmps;
	
	DEBUG_PRINTLN("Sleep for: " + String(SleepSeconds) + "seconds");
	esp_sleep_enable_timer_wakeup(SleepSeconds * uS_TO_S_FACTOR);

	//esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
	//DEBUG_SERIAL.println("Configured all RTC Peripherals to be powered down in sleep");
	DEBUG_PRINTLN("Going to sleep now");
	DEBUG_SERIAL.flush();
	esp_deep_sleep_start();
}

#define BATTERY_CHECK_INTERVALL 500000  // 2 times a second will do nicely
#define THRESHOLD_VALUE (3.3*DIGITS_PER_VOLT) // Battery alarm runs if battery goes below 3.3V
#define HYSTERESIS_VALUE (3.5*DIGITS_PER_VOLT) // Battery alarm ends as battery goes above 3.5V

RTC_DATA_ATTR bool BatteryAlarmState = false;
RTC_DATA_ATTR int batteryLowCounter = 0;
RTC_DATA_ATTR unsigned long nextTimeToCheckBattery;

bool GetBatteryAlarmState()
{
	return BatteryAlarmState;
}

void SetBatteryAlarmState(bool state)
{
	BatteryAlarmState = state;
}

bool HandleBatteryAlarm()
{
	bool AlarmIsActive = GetBatteryAlarmState();
	if (!AlarmIsActive)
		return false;

	RTC_DATA_ATTR static bool AlarmWasActive = false;

	if (AlarmWasActive && !AlarmIsActive)
	{	// return to normal state
		AlarmWasActive = false;
		DEBUG_PRINTLN("Battery alarm ends.");
		
		ReactivateStepper();
	}
	else if (!AlarmWasActive && AlarmIsActive)
	{	// Go to power save mode.
		AlarmWasActive = true;
		DEBUG_PRINTLN("Battery alarm!");
		DeactivateStepper();
		btStop();
		GoShutDown();
	}

	return AlarmIsActive;
}

void LoopBatteryProtect()
{
	unsigned long now = micros();
	if (now < nextTimeToCheckBattery)
		return;
		
	nextTimeToCheckBattery = now + BATTERY_CHECK_INTERVALL;

	uint16_t measuredBattery = analogRead(PIN_CHECK_BATT);

	DEBUG_PRINT(measuredBattery);
	DEBUG_PRINT("   ");
	if (GetBatteryAlarmState())
	{	// needs to be a certain time above a higher voltage to return to non-alarm-state
		if (measuredBattery > HYSTERESIS_VALUE)
			batteryLowCounter--;
		else
			batteryLowCounter = 10;

		if (batteryLowCounter <= 0)
		{
			batteryLowCounter = 0;
			SetBatteryAlarmState(false);
		}
	}
	else
	{
		if (measuredBattery < THRESHOLD_VALUE)
			batteryLowCounter++;
		else
			batteryLowCounter = 0;

		if (batteryLowCounter >= 10) // one second with undervoltage
		{
			batteryLowCounter = 10;
			SetBatteryAlarmState(true);
		}
	}
	DEBUG_PRINTLN(batteryLowCounter);
}

void SetupBatteryProtect()
{
	DEBUG_PRINTLN("SetupBatteryProtect");
	pinMode(PIN_CHECK_BATT, INPUT);
	nextTimeToCheckBattery = micros() + BATTERY_CHECK_INTERVALL;
}
