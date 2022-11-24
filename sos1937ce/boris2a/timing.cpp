// $Header$

// $Log$

#include "BS2all.h"

timer::timer( float ticksPerSecond )
{
	setTimer( ticksPerSecond);
}

void  timer::setTimer( float ticksPerSecond )
{
//	tpsDIVcps =  ticksPerSecond * timing_oneOverFrequency; 
	started = false;
}

