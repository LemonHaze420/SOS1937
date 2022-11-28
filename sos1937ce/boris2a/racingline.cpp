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
	

__inline long CONVTOKEY(float k) 
{
	long retval;

	FASTFTOL(&retval,k* 256.0f);
	return retval + (65536 << 8);
}

//debugger DBGR;

RLSet racinglineset;	// this is the only RLSet object
	
void rdprintf(char *f,...)
{
	char buf[256];

	va_list argptr;

	va_start(argptr,f);
	vsprintf(buf,f,argptr);
	OutputDebugString(buf);OutputDebugString("\n");
	va_end(argptr);
}


void loadRacingLineSet( char * filename, float scale)
{
	// SDS - Quick plug and pray - can't be bothered with files and stuff
	// SDS - so current splines are const arrays
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



	rlvector * flp;
	int totalPoints, i, texhandle;
	sortlistentry * tempBuffer1;
	sortlistentry * tsbPtr;
	float xsqrsum, zsqrsum, meanx, meanz, dx, dz;
	int * sortLinePoint;

	rdprintf("RacingLine:Entry");
	texhandle = arcExtract_rlf( filename );
	if(!texhandle)
		fatalError("failed to find racing line");
	// archive is open, so read the RLset header having cleared any old lines
	racinglineset.DeleteAll();




	racinglineset.h.numlines=1;

	rdprintf("RacingLine:about to decide local or file");

	// SDS - create a new set of lines e.t.c...
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	rdprintf("RacingLine:loading %d lines",racinglineset.h.numlines);

	for(int li=0;li<racinglineset.h.numlines;li++)
	{


		racingLine *line = new racingLine;
		racinglineset.lines[li]=line;

		bool gotone = false;
		extern char *jcfhack1;



		// SDS - load in the racingline & stuff direclty from the archive
		// Note archive file name is raceline.rline
		//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
		rdprintf("RacingLine: loading from archive");
		rdprintf("RacingLine: doing line %d...",li);

		// racingLine *line = new racingLine;
		// racinglineset.lines[li]=line;

		arcRead( (char *) &(line->pointsPerLine), sizeof(line->pointsPerLine), texhandle );
		line->totalLines=1;
		rdprintf("RacingLine: 1");
		//arcRead( (char *) &(line->pointsPerLine), sizeof(line->pointsPerLine), texhandle );
		//rdprintf("line has %d lines, %d points",line->totalLines,line->pointsPerLine);
		totalPoints = line->pointsPerLine;
		rdprintf("RacingLine: 1a %d",totalPoints);
		line->driveinfo = new drivinginfo[totalPoints];
		rdprintf("RacingLine: 2");
		line->zinfo = new zbackinfo[totalPoints];
		rdprintf("RacingLine: 3");
		line->racingLinePoints = new rlvector[totalPoints];
		rdprintf("RacingLine: 4");
		line->sortedRacingLinePoints = new int[totalPoints];
		rdprintf("RacingLine: 5");
		line->haswalls = racinglineset.h.haswalls[li]?true:false;
		rdprintf("RacingLine: 6");
		tempBuffer1 = new sortlistentry[ line->pointsPerLine ];
		rdprintf("RacingLine: 7");
		rdprintf("reading %ld points",totalPoints);

		arcRead( (char *) (line->racingLinePoints), sizeof(rlvector) * totalPoints , texhandle );
		arcRead( (char *) &(line->numBrakes),4,texhandle);
		line->brakePoints = new rlbrakes[line->numBrakes+1];
		line->brakePoints[line->numBrakes].point=99999;
		line->brakePoints[line->numBrakes].speed=80;
		arcRead( (char *) (line->brakePoints), sizeof(rlbrakes)*line->numBrakes, texhandle);

		char buffer[170];
		arcRead( (char *)buffer, 152, texhandle);
		// New Zback Values
		arcRead( (char *) (line->zinfo), (sizeof(zbackinfo)*totalPoints)-4, texhandle);


		//
		// Scale up all the racing line points, also measure the x and z standard deviation from the
		// respective means. The dimension with maximum standard deviation will be the one used for ordering.
		//
		// In practice since we have the same number of points per dimension we only need to compare the
		// sums of squares of differences from the mean. (square roots cancel too).
		//
	
		meanx = meanz = 0;
		rlvector * firstLinePoint = line->racingLinePoints;
		flp=firstLinePoint;
		for( i = totalPoints; i--; )
		{
			static rlvector * t;
			t = firstLinePoint;
			t->x *= scale;	
			t->y *= scale;			
			t->z *= scale;			
			meanx += t->x;
			meanz += t->z;
			firstLinePoint++;
		}

		rdprintf("scaled ok");
		float r = RECIPROCAL( ((float)(totalPoints)) );
		meanx *= r;
		meanz *= r;
		xsqrsum = zsqrsum = 0;
		firstLinePoint = line->racingLinePoints;
		for( i = totalPoints; i--; )
		{
			dx = firstLinePoint->x - meanx;
			dz = firstLinePoint->z - meanz;
			xsqrsum += dx * dx;
			zsqrsum += dz * dz;
			firstLinePoint++;
		}
		rdprintf("sqrsums ok");
		//
		// sorting on the appropriate dimension
		//
		int j, selectiveMask, pointIndex;
		firstLinePoint = line->racingLinePoints;
		sortLinePoint  = line->sortedRacingLinePoints;
		pointIndex = 0;
		if( line->sortedOnX = (xsqrsum >= zsqrsum))
		{
			for( i = line->totalLines; i--; )
			{
				rdprintf("xsorting line %d",i);
				selectiveMask = CONVTOKEY(firstLinePoint->x);
				tsbPtr   = tempBuffer1;
				for( j = line->pointsPerLine; j--; )
				{
					selectiveMask |=  (selectiveMask ^  (tsbPtr->key =  CONVTOKEY(firstLinePoint->x) ) );
					tsbPtr->idx =  pointIndex++; //(int)firstLinePoint;
					tsbPtr++;
					firstLinePoint++;
				}
				
				sort ( line->pointsPerLine, selectiveMask, tempBuffer1); 
				tsbPtr   = tempBuffer1;
				for( j = line->pointsPerLine; j--; )
				{
					*sortLinePoint = tsbPtr->idx; //(vector3 *)(tsbPtr->idx);
					tsbPtr++;
					sortLinePoint++;
				}
			}
		} 
		else
		{
			for( i = line->totalLines; i--; )
			{
				rdprintf("zsorting line %d",i);

				selectiveMask = CONVTOKEY(firstLinePoint->z);
				tsbPtr   = tempBuffer1;
				for( j = line->pointsPerLine; j--; )
				{
					selectiveMask |=  (selectiveMask ^  (tsbPtr->key =  CONVTOKEY(firstLinePoint->z) ) );
					tsbPtr->idx =  pointIndex++; //(int)firstLinePoint;
					tsbPtr++;
					firstLinePoint++;
				}
				
				sort ( line->pointsPerLine, selectiveMask, tempBuffer1); 
				tsbPtr   = tempBuffer1;

				for( j = line->pointsPerLine; j--; )
				{
					*sortLinePoint = tsbPtr->idx; //(vector3 *)(tsbPtr->idx);
					tsbPtr++;
					sortLinePoint++;
				}
			}
		}

		rdprintf("sorted OK");
		delete[] tempBuffer1;
		rdprintf("all loaded OK");
		if (totalPoints==475) {dprintf("Brooklands Hack"); racinglineset.h.numlines=2;}
	}





		



	arcDispose( texhandle );
	rdprintf("RacingLine:Exit");
}

racingLine::~racingLine()
{
	delete[] zinfo;
	delete[] driveinfo;
	delete[] racingLinePoints;
	racingLinePoints=NULL;
	delete[] sortedRacingLinePoints;
	sortedRacingLinePoints=NULL;
}

//----------------------------------------------------------------------------------------------------
// Returns a pointer to the nearest racing line point for the given racing line. Returns the SQUARE
// of the distance.
//----------------------------------------------------------------------------------------------------

// this uses a binary chop, and so is fastest when we need to search all the points.

double racingLine::getNearestRacingLinePointOfAll( int racingLineIdx, vector3 * position,
											  rlvector * racingLinePoint, int * idxOfPoint) 
{
		int firstPoint, lastPoint;

		float searchValueX, searchValueY, searchValueZ, midvalue;
		double mindsquared, distance, radicalX, radicalZ;
		vector3 * v;
		int idx, minpointidx;

		//
		// (i) find the closest point on the SORTED AXIS the racing line points by binary chop
		//

		int lo, mid, hi, oldmid;

		firstPoint = racingLineIdx * pointsPerLine;
		lastPoint  = firstPoint + (pointsPerLine-1);

		lo  = firstPoint;
		hi  = lastPoint;

		mindsquared = 999999999.0f;

		searchValueX = position->x;
		searchValueY = position->y;
		searchValueZ = position->z;
		
		int * rlpPtr;

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
			}
		} 
		else			// Z sorted list
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
			}
		}

		minpointidx = sortedRacingLinePoints[minpointidx];

		*racingLinePoint = racingLinePoints[minpointidx];
		*idxOfPoint = minpointidx;		// idx 

		return(mindsquared);
}

float racingLine::getNearest2RacingLinePoints(int racingLineIdx, vector3 *position,int prevpt,
	rlvector *rlp1,rlvector *rlp2,int *idx1,int *idx2,float *dist1,float *dist2)
{
	float d1,d2,tt,mind2=9999999.9f;
	int found,q,p;
	rlvector *v;

	// find the nearest point

	if(prevpt > pointsPerLine)
	{
		// prevpt is invalid, do a complete search
		*dist1 = (float)getNearestRacingLinePointOfAll( racingLineIdx,position,rlp1,idx1);
	}
	else
	{
		for(int i=0;i<5;i++)
		{
			v = &racingLinePoints[q=increment(prevpt,i-2)];
			d2 = (v->x - position->x); 
			d2 *= d2;
			tt = (v->z - position->z); 
			d2 += tt*tt;
			if(d2<mind2)
			{
				mind2=d2;found=q;
			}
		}

		*idx1 = found;
		*rlp1 = racingLinePoints[found];
	}

	// find the second nearest point by looking at the two points around this one

	v = &racingLinePoints[p=decrement(found,1)];
	d1 = (v->x - position->x); d1 *= d1;
	tt = (v->z - position->z); d1 += tt*tt;

	v = &racingLinePoints[q=increment(found,1)];
	d2 = (v->x - position->x); d2 *= d2;
	tt = (v->z - position->z); d2 += tt*tt;

	if(d1<d2)
		found = p;
	else
		found = q;

	*idx2 = found;
	*rlp2 = racingLinePoints[found];


	// get the three line equations for the line

	float f,g,t;

	int i1,i2;

	if(*idx2>*idx1)
	{
		i1 = *idx1;i2=*idx2;
	}
	else
	{
		i1 = *idx2;i2=*idx1;
	}


	f = racingLinePoints[i2].x - racingLinePoints[i1].x;
	g = racingLinePoints[i2].z - racingLinePoints[i1].z;

	// get the parameter for the line at the closest point from the position
	
	t = (	f*(position->x-racingLinePoints[i1].x)+
			g*(position->z-racingLinePoints[i1].z)
		) / (f*f + g*g);

	if(t<0)		t=0;
	if(t>1)		t=1;

	if(*idx1 == pointsPerLine-1 && !*idx2)
		return ((float)pointsPerLine)-t;
	else if(!*idx1 && *idx2 == pointsPerLine-1)
		return -t;
	else
		return ((float)i1)+t;
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
