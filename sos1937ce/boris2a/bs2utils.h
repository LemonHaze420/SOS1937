// $Header$

// $Log$
// Revision 1.2  2000-01-18 14:48:08+00  jjs
// Changed to use intrinsics for speed.
//
// Revision 1.1  2000-01-13 17:27:45+00  jjs
// First version that supports the loading of objects.
//

#ifndef _BS_utils
#define _BS_utils

void minmax3( float a, float b, float c,  float * min, float * max);
void minmax4( float a, float b, float c, float d, float * min, float * max);

class vector3;

inline void FASTFTOL(long *result, float f)
{
#if defined(UNDER_CE)
	*result = (long)f;
#else
	__asm {
		fld f
		mov eax,result
		fistp dword ptr [eax]
}
#endif
}

#if !defined(UNDER_CE)
void BuildSqrtTable(void);
float fsqrt(float n);
float fisqrt(float n);
#endif

void	startEngine(long ttotal, long vtotal, long ntotal, float physicsTicks);

void stripfilesuffix( char * filename );
uchar normalOctantCode( vector3 * v );
#endif	// _BS_utils
