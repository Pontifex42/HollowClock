#include "PinConfig.h"
#include "Stepper.h"
#include "TimeMove.h"
#include "ClockTime.h"

// #define DEBUG_TIMEMOVE

#ifndef DEBUG_TIMEMOVE
#ifdef DEBUG_PRINTLN
#undef DEBUG_PRINTLN
#define DEBUG_PRINTLN(a)
#undef DEBUG_PRINT
#define DEBUG_PRINT(a)
#endif
#endif

// Motor and clock parameters
// 4096 * 90 / 12 = 30720
// My motor is slightly different: it seems that it has half that steps
#define STEPS_PER_HOUR (30720/2) // steps for a full turn of minute rotor
#define STEPS_PER_MINUTE (STEPS_PER_HOUR / 60)
#define STEPS_PER_HALFDAY (STEPS_PER_HOUR * 12)

RTC_DATA_ATTR long currentstep = 0;

int h, m, s;
void SetupTimeMove()
{
	DEBUG_PRINTLN("SetupTimeMove");
	GetTime(h, m, s);
	s = 0; //  delete line if smooth movement wanted
	MoveTimeAbsolute(h, m, s);
}

void LoopTimeMove()
{
	int tmph, tmpm, tmps;
	GetTime(tmph, tmpm, tmps);
	if ((tmph != h) || (tmpm != m) /* || (tmps != s)*/)
	{
		h = tmph;
		m = tmpm;
		// s = tmps; uncomment if smooth movement wanted
		MoveTimeAbsolute(h, m, s);
	}
}

void MoveTimeAbsolute(int hours, int minutes, int seconds)
{
	DEBUG_PRINTLN("Move absolute to " + String(hours) + ":" + String(minutes) + ":" + String(seconds));

	long targetstep = (hours * STEPS_PER_HOUR) + (minutes * STEPS_PER_MINUTE);
	targetstep %= STEPS_PER_HALFDAY;
	long delta = targetstep - currentstep;

	if (abs(delta) > (STEPS_PER_HALFDAY / 2))
	{
		if (delta < 0)
			delta += STEPS_PER_HALFDAY;
		else
			delta -= STEPS_PER_HALFDAY;
	}

	DEBUG_PRINTLN("CurrentStep:"  + String(currentstep) + " TargetStep:" + String(targetstep) + " Delta:" + String(delta));
	MoveStepper(delta); // forward or backward possible

	currentstep = targetstep;
	while (currentstep < 0)
		currentstep += (STEPS_PER_HOUR * 12);
	currentstep %= (STEPS_PER_HOUR * 12);
}
