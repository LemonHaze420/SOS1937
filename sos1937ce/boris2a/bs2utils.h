//================================================================================================================================
// BS2utils.h
// ----------
//				- general utility routines for BORIS2
//================================================================================================================================

#ifndef _BS_utils
#define _BS_utils

void minmax3( float a, float b, float c,  float * min, float * max);
void minmax4( float a, float b, float c, float d, float * min, float * max);

class vector3;

inline void FASTFTOL(slong *result, float f)
{
__asm {
 		fld	f
 		mov	eax,result
 		fistp	dword ptr [eax]
	}
}

void BuildSqrtTable();
float fsqrt(float n);
float fisqrt(float n);

void	startEngine( 
				 bool sware,  slong outListCount, slong ttotal, slong vtotal, 
				 slong ntotal, float physicsTicks
				);

void stripfilesuffix( char * filename );
uchar normalOctantCode( vector3 * v );
#endif	// _BS_utils

//================================================================================================================================
//END OF FILE
//================================================================================================================================
