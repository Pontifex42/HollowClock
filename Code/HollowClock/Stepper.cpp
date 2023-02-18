#include "Arduino.h"
#include "PinConfig.h"
#include "Stepper.h"
#include "Remote.h"
#include "BatteryProtect.h"

// #define DEBUG_STEPPER

#ifndef DEBUG_STEPPER
#ifdef DEBUG_PRINTLN
#undef DEBUG_PRINTLN
#define DEBUG_PRINTLN(a)
#undef DEBUG_PRINT
#define DEBUG_PRINT(a)
#endif
#endif


#define ROT_CW -1
#define ROT_CCW 1

#define MicroSecPerStep 1900 // usual limit is 2-3ms per step, regarding to datasheet. Mine work still at 1,5ms.

RTC_DATA_ATTR long absoluteCurrentStep = 0;
RTC_DATA_ATTR long absoluteTargetStep = 0;

unsigned long lastStepTime = 0;

void ExecuteStep(int direction)
{
	static int stepPhase_l = 0;

	// compute the next phase number
	stepPhase_l += direction;

	if (stepPhase_l <= -1)
		stepPhase_l = 3;

	if (stepPhase_l >= 4)
		stepPhase_l = 0;

	while ((micros() - lastStepTime) < MicroSecPerStep)
		;

	// set the coils for this phase
	switch (stepPhase_l)
	{
	case 0:
		digitalWrite(PIN_STEPPER_1, LOW);
		digitalWrite(PIN_STEPPER_2, LOW);
		digitalWrite(PIN_STEPPER_3, HIGH);
		digitalWrite(PIN_STEPPER_4, HIGH);
		break;
	case 1:
		digitalWrite(PIN_STEPPER_1, LOW);
		digitalWrite(PIN_STEPPER_2, HIGH);
		digitalWrite(PIN_STEPPER_3, HIGH);
		digitalWrite(PIN_STEPPER_4, LOW);
		break;
	case 2:
		digitalWrite(PIN_STEPPER_1, HIGH);
		digitalWrite(PIN_STEPPER_2, HIGH);
		digitalWrite(PIN_STEPPER_3, LOW);
		digitalWrite(PIN_STEPPER_4, LOW);
		break;
	case 3:
		digitalWrite(PIN_STEPPER_1, HIGH);
		digitalWrite(PIN_STEPPER_2, LOW);
		digitalWrite(PIN_STEPPER_3, LOW);
		digitalWrite(PIN_STEPPER_4, HIGH);
		break;
	}
	lastStepTime = micros();
}

void MoveStepper(int cnt)
{
	absoluteTargetStep += cnt;
	DEBUG_PRINTLN("Step to abs " + String(absoluteTargetStep));
}

bool isActivated = true;
void DeactivateStepper()
{
	if (!isActivated)
		return;
	DEBUG_PRINTLN("DeActivate stepper");
	while ((micros() - lastStepTime) < MicroSecPerStep)
		;// Let motor finish current step

	digitalWrite(PIN_STEPPER_1, LOW);
	digitalWrite(PIN_STEPPER_2, LOW);
	digitalWrite(PIN_STEPPER_3, LOW);
	digitalWrite(PIN_STEPPER_4, LOW);
	isActivated = false;
}

void ReactivateStepper()
{
	if (isActivated)
		return;
	DEBUG_PRINTLN("ReActivate stepper");
	isActivated = true;
	ExecuteStep(0);
}

void SetupStepper()
{
	DEBUG_PRINTLN("SetupStepper");

	pinMode(PIN_STEPPER_1, OUTPUT);
	pinMode(PIN_STEPPER_2, OUTPUT);
	pinMode(PIN_STEPPER_3, OUTPUT);
	pinMode(PIN_STEPPER_4, OUTPUT);
	
	lastStepTime = micros() + MicroSecPerStep;
	DeactivateStepper();
}

void LoopStepper()
{
	if ((micros() - lastStepTime) < MicroSecPerStep)
		return;

	long diff = absoluteTargetStep - absoluteCurrentStep;

	if (diff == 0)
	{
		// avoid overflow of step counters
		absoluteTargetStep %= 1000000;
		absoluteCurrentStep %= 1000000;
		DeactivateStepper();
		if(!isPowerupPhase)
			GoToSleep();
		return;
	}

	int dir = diff < 0 ? ROT_CCW : ROT_CW;
	ReactivateStepper();
	ExecuteStep(dir);
	absoluteCurrentStep -= dir;
}
