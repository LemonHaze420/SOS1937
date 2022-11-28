//================================================================================================================================
// racingLine.h
// ------------
//
//================================================================================================================================

#ifndef _BS_racingLine
#define _BS_racingLine

#include "sortlistentry.h"

class racingLine
{
	public:
			racingLine( char * filename, float scale);
			~racingLine();

					//
					// 
					//

			double getNearestRacingLinePoint( ulong racingLineIdx, vector3 * position,
											  vector3 * racingLinePoint, ulong * idxOfPoint, ulong oldidx );

		
//	private:

			vector3 *   racingLinePoints;	// array of racing line points.
			ulong   *   sortedRacingLinePoints;

		
			ulong	pointsPerLine;
			ulong   totalLines;

			bool    sortedOnX;				// True iff sortedReacingLinePoints is keyed on 'X' else keyed on 'Z' -
											// this is decided on load of racing line.

};


#endif	// _BS_racingLine

//================================================================================================================================
//END OF FILE
//================================================================================================================================
