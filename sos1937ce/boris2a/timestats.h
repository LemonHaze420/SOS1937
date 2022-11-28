//================================================================================================================================
// timeStats.h
// -----------
//
//================================================================================================================================

#ifndef _BS_timeStats
#define _BS_timeStats

#include <stdio.h>

typedef struct 
{
			__int64 startTime;
			__int64 totalTicks;
			ulong r,g,b;
} timeStatsElement;


void endTimingStats( void );
void startStat( ulong idx, ulong r, ulong g, ulong b );
void endStat( ulong idx );

void initialiseFrameRateCounter( ulong sampleframes );
void acknowledgeFrame( void );
void displayFrameRate( ulong gNum, ulong rNum, float vNum);


	//
	// define MACROS so that we dont need loads of 'ifdef' tests in the main code
	//

#ifdef CONFIG_RELATIVE_TIMING
	#define STARTSTAT(a, r,g,b) startStat(a, r,g,b)
	#define ENDSTAT(a) endStat(a)
	#define ENDTIMINGSTATS() endTimingStats()
#else
	#define STARTSTAT(a ,r,g,b) 
	#define ENDSTAT(a) 
	#define ENDTIMINGSTATS() 
#endif

#ifdef CONFIG_FRAMERATE
	#define INITIALISEFRAMERATECOUNTER(a) initialiseFrameRateCounter(a)
	#define ACKNOWLEDGEFRAME() acknowledgeFrame()
	#define DISPLAYFRAMERATE() displayFrameRate( testcar->engine->currentGearNumber ,\
												 testcar->getEngineRevs(), \
												 testcar->engine->localVelocity.z )
#else
	#define INITIALISEFRAMERATECOUNTER(a)
	#define ACKNOWLEDGEFRAME() 
	#define DISPLAYFRAMERATE()
#endif


#endif	// _BS_timeStats

//================================================================================================================================
//END OF FILE
//================================================================================================================================
