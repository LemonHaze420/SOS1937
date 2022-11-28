//================================================================================================================================
// rollOverNumber.cpp
// ------------------
//
//================================================================================================================================

#include "BS2all.h"
#ifdef FEATURE_ROLLOVERNUMBER

rollOverNumber::rollOverNumber( char * rollOverFileName, 
						float x, float y,
						float digitW, float digitH, float digitG,
						ulong count,  ulong startvalue, bool leadingZeroes)
{

		// - read in the filename line by line, getting the textures needed for each digit	
	

		// - set up all internal settings from parameters


		// - adds it to a list of rollover counters held in the engine ready for rendering.
}

rollOverNumber::~rollOverNumber()
{
}

void rollOverNumber::increment()
{
}					// add one to counter.

void rollOverNumber::setCounter( ulong n )
{
}					// sets counter to a specific value

#endif



//================================================================================================================================
// END OF FILE
//================================================================================================================================
