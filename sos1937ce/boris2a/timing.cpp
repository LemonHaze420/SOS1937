//================================================================================================================================
// timing.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"

extern float   timing_oneOverFrequency;


//================================================================================================================================
//================================================================================================================================

timer::timer( float ticksPerSecond )
{
	setTimer( ticksPerSecond);
};

void  timer::setTimer( float ticksPerSecond )
{
	tpsDIVcps =  ticksPerSecond * timing_oneOverFrequency; 
	started = false;
}

timer::timer()
{
};

//================================================================================================================================
// getTicks
// ---------
//
// retrieve the number of whole ticks passed since the last time this routine was called.
// - first time returns 0 (zero) since no time has passed and initial information is valid
//================================================================================================================================

ulong timer::getTicks()
{
	slong oldtickspassed;
	if( started)
	{
		__int64 finalClocks;
  	  oldtickspassed = tickspassed;
	  QueryPerformanceCounter((LARGE_INTEGER *)&finalClocks);
	  FASTFTOL( &tickspassed,((float)(finalClocks - initialClocks)) * tpsDIVcps - 0.5f );
	  //initialClocks       = finalClocks;			// for next iteration
	  return( tickspassed - oldtickspassed);
	} else
	{
	  started = true;
	  QueryPerformanceCounter((LARGE_INTEGER *)&initialClocks);
	  tickspassed = 0;
	  return( 0 );
	}
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
