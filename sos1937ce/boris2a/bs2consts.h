//================================================================================================================================
// BS2consts.h
// -----------
//		Constants used throughout the engine
//================================================================================================================================

#ifndef _BS_BS2consts
#define _BS_BS2consts


	//
	// base resolution ... this is the resolution of a 'virtual 2D screen' and thereby standardises all projection into#
	//					   this 2D space, resulting in the same view being displayed no matter what the actual physical size
	//					   of the 2D display  (For instance we want the same view no matter what resolution we are
	//					   displaying into).
	//

#define BASERES_WIDTH	800.0f
#define BASERES_HEIGHT  600.0f

	//
	// MAXIMUM Z-DISTANCE ALLOWED. WHEN PROJECTING WE NORMALISE TO THE RANGE 0..1 TO REPRESENT THIS DISTANCE. IF
	// THE RESULTING 'Z' HAS INSUFFICIENT GRANULARITY FOR THE Z-BUFFER WE WOULD NEED TO DECREASE THIS!
	//
	//  if NOT defined then we do not clip highz to scale


//#define MAXIMUMREARZ    99990.0f
#define MAXIMUMREARZ    30000.0f

	//
	// MAX2DENTRIES .....   The maximum number of 2D rextangular images overlaid on the 3D image (for HUD etc.)
	//

#define MAX2DENTRIES  512
	//
	// The maximum value for 'z' if software depth buffering is used (only used when software
	// rendering is used).  This value  should be a power of 2, minus 1 and a FLOATING POINT 
	// constant, since it is used at the point of projecting a 3d coord to a 2d coord. (this
	// is the most efficient place).
	// If you change this value bear in mind:
	//
	//		(i)  The bigger the value the more iterations (one per power of 2) of the
	//			 radix2 bin-sort is needed to sort on 'z'. 
	//			 smaller values are faster, bigger values more accurate on close together 'z's
	//			 ( it is not worth using 'w' (1/z) sorting on triangle centroids ).
	//
	//			- The value actually held by this constant must be one third of the maximum value
	//			  (so that when we add three values we dont need to multiply by 1/3 to get the average)
	//

#define SOFTWARE_ZMAX	( 65535.0f * 0.333333f )

	//
	// MAXIMUM_SORT_ENTRIES  ... This static array must be big enough for any sort. This implies
	//							 that it is at least as many as the number of triangles in a 
	//							 frame (since they will be sorted, either by material or by
	//							 mean-z depending on whether hardware or local software rendering
	//							 is being used.
	//

#define MAXIMUM_SORT_ENTRIES  32768

	//
	//
	//

#define UNDERWATER_UDIFFUSE    0x7F10107F

	//
	// MATH CONSTANTS
	//

#define PI				( 3.14159265359f )
#define TWOPI			( 2.0f * PI )
#define HALFPI          ( 0.5f * PI )
#define QUARTERPI       ( 0.25f * PI )

#endif	// _BS_BS2consts

//================================================================================================================================
//END OF FILE
//================================================================================================================================
