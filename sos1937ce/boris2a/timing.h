//================================================================================================================================
// timing.h
// --------
//
//================================================================================================================================

#ifndef _BS_timing
#define _BS_timing

#include <stdio.h>

class timer
{

	public:
			timer( float ticksPerSecond );
			timer();
			~timer(){};

			void  setTimer( float ticksPerSecond );
			ulong getTicks();
			
	private:
			slong tickspassed;
			float tpsDIVcps;
			bool  started;			// true iff we have called 'getTicks; at least once

			__int64 initialClocks;	// hold time at last iteration ... ready for next 
			
};

#endif	// _BS_timing

//================================================================================================================================
//END OF FILE
//================================================================================================================================
