//================================================================================================================================
// racingLine.cpp
// --------------
//
//================================================================================================================================

#include "BS2all.h"

extern char tempBuffer[];
	
#define MYfread( a, b, c, d ) \
			if(fread( a, b, c, d ) != 1) \
				fatalError("Error reading RacingLine file")
	

#define CONVTOKEY(k) \
				((long)((k)* 256.0f)) + (65536 << 8)


//debugger DBGR;

racingLine::racingLine( char * filename, float scale)
{
	ulong totalPoints, i, texhandle;
	sortlistentry * tempBuffer1;

	sortlistentry * tsbPtr, * firsttsbPtr;
	float xsqrsum, zsqrsum, meanx, meanz, dx, dz;
	ulong * sortLinePoint;

	texhandle = arcExtract_rlf( filename );
	
	if( texhandle ) 
	{
	
		arcRead( (char *) &totalLines, sizeof(totalLines), texhandle );
		arcRead( (char *) &pointsPerLine, sizeof(pointsPerLine), texhandle );

		totalPoints = totalLines * pointsPerLine;

		racingLinePoints = new vector3[totalPoints];
		sortedRacingLinePoints = new ulong[totalPoints];

		tempBuffer1 = new sortlistentry[ pointsPerLine ];

		arcRead( (char *) racingLinePoints,  sizeof(vector3) * totalPoints , texhandle );


				//
				// Scale up all the racing line points, also measure the x and z standard deviation from the
				// respective means. The dimension with maximum standard deviation will be the one used for ordering.
				//
				// In practice since we have the same number of points per dimension we only need to compare the
				// sums of squares of differences from the mean. (square roots cancel too).
				//

	
		meanx = meanz = 0;
		vector3 * firstLinePoint = racingLinePoints;

		for( i = totalPoints; i--; )
		{
			static vector3 * t;

			t = firstLinePoint;
			t->x *= scale;	
			t->y *= scale;			
			t->z *= scale;			

			meanx += t->x;
			meanz += t->z;
			firstLinePoint++;
		};


		float r = RECIPROCAL( ((float)(totalPoints)) );
		meanx *= r;
		meanz *= r;


	


		xsqrsum = zsqrsum = 0;
		firstLinePoint = racingLinePoints;
		for( i = totalPoints; i--; )
		{
		
			dx = firstLinePoint->x - meanx;
			dz = firstLinePoint->z - meanz;

			xsqrsum += dx * dx;
			zsqrsum += dz * dz;

			firstLinePoint++;
		};



				//
				// sorting on the appropriate dimension
				//


		ulong j, selectiveMask, pointIndex;

		firstLinePoint = racingLinePoints;
		sortLinePoint  = sortedRacingLinePoints;

		pointIndex = 0;
		if( sortedOnX = (xsqrsum >= zsqrsum))
		{
			for( i = totalLines; i--; )
			{
				selectiveMask = CONVTOKEY(firstLinePoint->x);
				tsbPtr   = tempBuffer1;
				for( j = pointsPerLine; j--; )
				{
					selectiveMask |=  (selectiveMask ^  (tsbPtr->key =  CONVTOKEY(firstLinePoint->x) ) );
					tsbPtr->idx =  pointIndex++; //(ulong)firstLinePoint;
					tsbPtr++;
					firstLinePoint++;
				};				
				
				sort ( pointsPerLine, selectiveMask, tempBuffer1); 
				tsbPtr   = tempBuffer1;

				for( j = pointsPerLine; j--; )
				{
					*sortLinePoint = tsbPtr->idx; //(vector3 *)(tsbPtr->idx);
					tsbPtr++;
					sortLinePoint++;
				};
   			
			}
		} else
		{
			for( i = totalLines; i--; )
			{
				selectiveMask = CONVTOKEY(firstLinePoint->z);
				firsttsbPtr = tsbPtr;
				for( j = pointsPerLine; j--; )
				{
					selectiveMask |=  (selectiveMask ^  (tsbPtr->key =  CONVTOKEY(firstLinePoint->z) ) );
					tsbPtr->idx = pointIndex++;//(ulong)firstLinePoint;
					tsbPtr++;
					firstLinePoint++;
				};							
				sort ( pointsPerLine, selectiveMask, firsttsbPtr); 
   				tsbPtr   = tempBuffer1;

				for( j = pointsPerLine; j--; )
				{
					*sortLinePoint = tsbPtr->idx; //(vector3 * )(tsbPtr->idx);
					tsbPtr++;
					sortLinePoint++;
				};
			}
		};

		delete[] tempBuffer1;



	/*
		vector3 ** tt = sortedRacingLinePoints;//racingLinePoints;
		
		for(j = totalLines; j--; )
		{
			DBGR.Dprintf("**********************************************************************************************");
			static vector3 ** t = tt;
			for(i = pointsPerLine; i--; )
			{
				

				sprintf(zzz,"%f %f %f",  (*t)->x, (*t)->y, (*t)->z);
				DBGR.Dprintf(zzz);
				t++;
			};
			tt += pointsPerLine;
		};
	*/

	/*
		vector3 * uu = racingLinePoints;
		
		for(j = totalLines; j--; )
		{
			DBGR.Dprintf("**********************************************************************************************");
			static vector3 * u = uu;
			for(i = pointsPerLine; i--; )
			{
				

				sprintf(zzz,"%f %f %f",  (u)->x, (u)->y, (u)->z);
				DBGR.Dprintf(zzz);
				u++;
			};
			uu += pointsPerLine;
		};
	*/


		arcDispose( texhandle );
	
	} else
	{
		
		fatalError("Failed to find RacingLine");
	}

};

racingLine::~racingLine()
{
	delete[] racingLinePoints;
	delete[] sortedRacingLinePoints;
};


//----------------------------------------------------------------------------------------------------
// Returns a pointer to the nearest racing line point for the given racing line. Returns the SQUARE
// of the distance.
//----------------------------------------------------------------------------------------------------

//extern debugger DBGR;
//static char zzz[200];

double racingLine::getNearestRacingLinePoint( ulong racingLineIdx, vector3 * position,
											  vector3 * racingLinePoint, ulong * idxOfPoint, ulong oldidx ) 
{

		ulong firstPoint, lastPoint;

		float searchValueX, searchValueY, searchValueZ, midvalue;
		double mindsquared, distance, radicalX, radicalZ;
		vector3 * v;
		ulong idx, minpointidx;

		//
		// (i) find the closest point on the SORTED AXIS the racing line points by binary chop
		//

		ulong lo, mid, hi, oldmid;

		firstPoint = racingLineIdx * pointsPerLine;
		lastPoint  = firstPoint + (pointsPerLine-1);

		lo  = firstPoint;
		hi  = lastPoint;

		mindsquared = 999999999.0f;

		searchValueX = position->x;
		searchValueY = position->y;
		searchValueZ = position->z;

		
		ulong * rlpPtr;
		//ulong pointidx;

		if (sortedOnX)	
		{

			mid = (lo + hi)>>1;
			do
			{
				midvalue = racingLinePoints[sortedRacingLinePoints[mid]].x;
				if( midvalue > searchValueX )
					hi = mid;
				else 
					if( midvalue < searchValueX )
						lo = mid;
					else
						break;

				oldmid = mid;
				mid = (lo + hi)>>1;
								
			} while ( mid != oldmid);

			//
			// (ii) Search DOWN from 'mid' position only as far as needed
			//
		

			rlpPtr = &sortedRacingLinePoints[idx = mid];
			do
			{
				v = &racingLinePoints[*rlpPtr ];
				radicalX = v->x - searchValueX;
				radicalX *= radicalX;

				if( radicalX >= mindsquared)			// stop scan in this direction since no more values can improve it!
					break;

				radicalZ = v->z - searchValueZ;
				distance = radicalZ*radicalZ + radicalX;			// distance squared
	
			
					if( distance < mindsquared )
					{
						mindsquared = distance;
						minpointidx = idx;
					};
					if ( idx == firstPoint ) 
						break;
			
				idx--;
				rlpPtr--;
			} while(1) ;


					// search UP

		
			
			if( mid != lastPoint)			// we need to skip the first one (already done by DOWN loop)
			{
			  
			  rlpPtr = &sortedRacingLinePoints[idx = mid+1];
			  while( idx != lastPoint )
			  {
				v = &racingLinePoints[*rlpPtr];;
				radicalX = v->x - searchValueX;
				radicalX *= radicalX;

				if( radicalX >= mindsquared)			// stop scan in this direction since no more values can improve it!
					break;

				radicalZ = v->z - searchValueZ;
				distance = radicalZ*radicalZ + radicalX;			// distance squared

			
					if( distance < mindsquared )
					{
						mindsquared = distance;
						minpointidx = idx;
					} 
				
				idx++;
				rlpPtr++;
			  }
			};

		

		} else			// Z sorted list
		{

			mid = (lo + hi)>>1;
			do
			{
				midvalue = racingLinePoints[sortedRacingLinePoints[mid]].z;
				if( midvalue > searchValueZ )
					hi = mid;
				else 
					if( midvalue < searchValueZ )
						lo = mid;
					else
						break;

				oldmid = mid;
				mid = (lo + hi)>>1;
								
			} while ( mid != oldmid);

			//
			// (ii) Search DOWN from 'mid' position only as far as needed
			//
		
			rlpPtr = &sortedRacingLinePoints[idx = mid];
			do
			{
				v = &racingLinePoints[ *rlpPtr ];
				radicalZ = v->z - searchValueZ;
				radicalZ *= radicalZ;

				if( radicalZ >= mindsquared)			// stop scan in this direction since no more values can improve it!
					break;

				radicalX = v->x - searchValueX;
				distance = radicalX*radicalX + radicalZ;			// distance squared
	
			
					if( distance < mindsquared )
					{
						mindsquared = distance;
						minpointidx = idx;
					}
				
				if ( idx == firstPoint ) 
					break;
				idx--;
				rlpPtr--;
			} while(1) ;


					// search UP

			if( mid != lastPoint)			// we need to skip the first one (already done by DOWN loop)
			{
 			  rlpPtr = &sortedRacingLinePoints[idx = mid+1];
			  while( idx != lastPoint )
			  {
				v = &racingLinePoints[  *rlpPtr ];;
				radicalZ = v->z - searchValueZ;
				radicalZ *= radicalZ;

				if( radicalZ >= mindsquared)			// stop scan in this direction since no more values can improve it!
					break;

				radicalX = v->x - searchValueX;
				distance = radicalX*radicalX + radicalZ;			// distance squared
	
				
					if( distance < mindsquared )
					{
						mindsquared = distance;
						minpointidx = idx;
					} 
				
				idx++;
				rlpPtr++;
			  }
			};


		};

		minpointidx = sortedRacingLinePoints[minpointidx];


		if( minpointidx == oldidx )
		{
			minpointidx++;
			if( minpointidx == pointsPerLine)
				minpointidx = 0;
		};


		*racingLinePoint = racingLinePoints[minpointidx];
		*idxOfPoint = minpointidx;		// idx 

		return(mindsquared);

};


/*

			vector3 ** lineStart;			// an array of pointers to start of line in 'racingLinePoints'
			vector3 *   racingLinePoints;	// array of racing line points.
			sortlistentry *   sortedRacingLinePoints;

		
			ulong	pointsPerLine;
			ulong   totalLines;

			bool    sortedOnX;				// True iff sortedReacingLinePoints is keyed on 'X' else keyed on 'Z' -
											// this is decided on load of racing line.
	

*/


//================================================================================================================================
// END OF FILE
//================================================================================================================================
