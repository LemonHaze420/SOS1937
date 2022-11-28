//================================================================================================================================
// racingLine.h
// ------------
//
//================================================================================================================================

#ifndef _BS_racingLine
#define _BS_racingLine

#include "sortlistentry.h"

//
//	New racing line set code;
//
//
//	Usage:
//
//		load a racing line set with
//			void loadRacingLineSet(char *filename,float scale); [fatal error on failure]
//
//		get number of racing lines with
//			int numLines();
//
//		get a particular racing line
//			racingLine *getLine(int n);
//
//		get nearest racing line point with
//			double racingLine::getNearestRacingLinePoint( int racingLineIdx, vector3 * position,
//											  rlvector * racingLinePoint, int * idxOfPoint, int oldidx );
//
//			racinglineIdx is the index of the line we're looking at
//			vector3 *position is the address of the point we're looking from
//			rlvector *racingLinePoint is the address of the vector we'll write the result to
//			int *idxOfPoint is the we're we'll write the index of the point we find
//			int oldidx is the index of the previous point, if we want to avoid getting it again. If this
//				is the same as the point we just found, the next one is returned instead.
//			
//
//
//

class RLSetHeader
{
public:
	unsigned long magic;
	short numlines;
	short haswalls[16];
};
// SDS - race line brake points
class rlbrakes
{
public:
 int point,speed;
};

class drivinginfo
{
public:
		int gears;
		float speed;
};

class zbackinfo
{
public:
	float z1,z2;
};

class rlvector : public vector3
{
public:
	float ex1,ey1,ez1,ex2,ey2,ez2; 
	float speed;  // Now used for gradient 
};


class racingLine
{
	public:
			~racingLine();
			racingLine()
			{
				reverse=false;
			}

			// takes:
			//	index of racing line within set
			//	pointer to position of car 
			//	pointer to a place to put the resulting point
			//	pointer to a place to put the index of the point
			//	previous racing line point index, to optimise the search, or zero.
			// returns square of the distance to that point

			double getNearestRacingLinePointOfAll( int racingLineIdx, vector3 * position,
											  rlvector * racingLinePoint, int * idxOfPoint);

			// similar, but only searches the nearest points to the last point the car was at.
			// If >pointsPerLine is passed as the previous nearest RLP, it calls the previous method.

			float getNearest2RacingLinePoints(int racingLineIdx, vector3 *position,int prevpt,
				rlvector *rlp1,rlvector *rlp2,int *idx1,int *idx2,float *dist1,float *dist2);

		
//	private:
			rlbrakes * brakePoints;
			int numBrakes;
			rlvector *   racingLinePoints;	// array of racing line points.
			int   *   sortedRacingLinePoints;
			drivinginfo * driveinfo;
			zbackinfo * zinfo;
		
			int	pointsPerLine;
			int   totalLines;

			bool    sortedOnX;				// True iff sortedReacingLinePoints is keyed on 'X' else keyed on 'Z' -
											// this is decided on load of racing line.

			bool	haswalls;				// true if the last two lines are walls.
			bool	reverse;				// the lines are backwards

			bool gt(int rlpt1,int rlpt2) // returns true if rlpt2>rlpt1 in line
			{
				int dist = (rlpt1>rlpt2) ? dist = rlpt2-rlpt1 : pointsPerLine +	rlpt2-rlpt1;

				if(dist<(pointsPerLine/2)) return reverse ? false : true;
				else return reverse ? true : false;
			}

			int increment(int rlpt,int n=1, int subline = 0)
			{
				rlpt += reverse ? -n : n;

				rlpt %= pointsPerLine;
				if(rlpt<0)rlpt+=pointsPerLine;
				rlpt += subline * pointsPerLine;
				return rlpt;
			}
			int decrement(int rlpt,int n=1, int subline = 0)
			{
				rlpt += reverse ? n : -n;

				rlpt %= pointsPerLine;
				if(rlpt<0)rlpt+=pointsPerLine;
				rlpt += subline * pointsPerLine;
				return rlpt;
			}
};

class RLSet
{
	friend void loadRacingLineSet(char *filename,float scale);
public:
	class racingLine *getLine(int n) { return lines[n] ;}
	int numLines() { return h.numlines; }

	RLSet() { for(int i=0;i<16;i++)lines[i]=NULL; }

	void DeleteAll() { for(int i=0;i<16;i++) if(lines[i])delete lines[i];}
	void Initialise() { DeleteAll();for(int i=0;i<16;i++)lines[i]=NULL; }

	~RLSet() { DeleteAll(); }

private:
	RLSetHeader h;
	class racingLine *lines[16];
};

extern class RLSet racinglineset;


#endif	// _BS_racingLine

//================================================================================================================================
//END OF FILE
//================================================================================================================================
