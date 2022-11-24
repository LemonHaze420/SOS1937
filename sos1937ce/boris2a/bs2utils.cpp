// $Header$

// $Log$

#include "BS2all.h"

extern engine * boris2;

//================================================================================================================================
//================================================================================================================================

void startEngine(long ttotal, long vtotal, long ntotal, float physicsTicks)
{
	boris2 = new engine( ttotal, vtotal, ntotal, physicsTicks  );

	initialiseModelLoading();
	startTextureStore();
	startModelStore();

}

void stripfilesuffix( char * filename )
{
	char * cptr = filename;
	char c;
	
	while( (c = *cptr) != 0)
	{
		if(c == '.')
		{
			*cptr = 0;
			break;
		}
		cptr++;
	}
}

//================================================================================================================================
// minmax3 .....  get the minimum and maximum of three floats
//				- requires either 2 or 3 compares
//================================================================================================================================

void minmax3( float a, float b, float c,  float * min, float * max)
{
	if ( a < b )
	{
		if ( b < c )
		{
			*min = a;
			*max = c;
		}
		else
		{
			*max = b;
			if ( a < c )
				*min = a;
			else
				*min = c;
		}

	}
	else
	{	
		if ( b < c )
		{
			*min = b;
			if ( a < c )
				*max = c;
			else
				*max = a;
		}
		else
		{
			*max = a;
			*min = c;
		}
	}
}
