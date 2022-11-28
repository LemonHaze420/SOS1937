//================================================================================================================================
// SineTable.h
// -----------
//
//================================================================================================================================

#ifndef _BS_SineTable
#define _BS_SineTable

__inline float mySine( float angle ) { return (float)sin((double)angle); }
__inline float myCosine( float angle ) { return (float)cos((double)angle); }

__inline float mySineAndCosine( float angle, float * s )
{
	*s = (float)sin(angle);
	return((float)cos(angle));
}


#endif	// _BS_SineTable

//================================================================================================================================
//END OF FILE
//================================================================================================================================
