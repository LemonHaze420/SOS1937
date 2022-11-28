/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\carengine.cpp,v 1.3 1999/01/12 11:45:29 jcf Exp jcf $
	$Log: carengine.cpp,v $
	Revision 1.3  1999/01/12 11:45:29  jcf
	This version seems to work

	Revision 1.2  1999/01/12 09:33:28  jcf
	DO NOT USE
	Just done basic power curve code. Halfway through automatic clutch code, crashes when clutch engages!

*/

static const char *rcs_id="$Id: carengine.cpp,v 1.3 1999/01/12 11:45:29 jcf Exp jcf $";

//================================================================================================================================
// carengine.cpp
// -------------
//
//================================================================================================================================

#include "ALLcar.h"


carengine::carengine( float maxSpeed, ulong gears )
{
			
				maxVelocity		=  maxSpeed;
				//acceleration = 0.0f;
				revs = 0.0f;
				localVelocity.x = localVelocity.y = localVelocity.z=
				velocity.x = velocity.y = velocity.z  = 0.0f;
				gearCount =  gears;		//  1..gearCount = legal gears ( 0 = out of gear )
				maxrevs = 0;
				prevpoint = 0;
				previousGearNumber=0;
				previousTransRevs=0;
};


//class cargearData {
//
//	float minRevs, maxRevs;
//	float ratio;
//};

extern void dprintf(char *,...);

float carengine::getpower(float revs)
{
	// emergency checks

	if(revs<0)revs = -revs;
	if(revs<100)
		prevpoint=0;
	else
	{

		if(revs>=maxrevs)return 0;	// no more power over this, definitely!

		if(!(points[prevpoint*2]<=revs && revs< points[prevpoint*2+2]))
		{
			if(revs<points[prevpoint*2])
			{
				// backwards search

				while(--prevpoint>=0)
				{
					if(points[prevpoint*2]<=revs && revs< points[prevpoint*2+2])
						break;
				}
			}
			else
			{
				// forwards search

				while(++prevpoint<10)
				{
					dprintf("scanning forwards %d : %f < %f < %f ?\n",
						prevpoint,points[prevpoint*2],revs,points[prevpoint*2+2]);

					if(points[prevpoint*2]<=revs && revs<points[prevpoint*2+2])
						break;
				}
			}
		}
	}

	// ok, prevpoint is the beginning of the line segment we want, now we need to lerp.
grdebug("point",(int)prevpoint);

	float rev1 = points[prevpoint*2];
	float rev2 = points[prevpoint*2+2];

	float val1 = points[prevpoint*2+1];
	float val2 = points[prevpoint*2+3];

	float q = (revs - rev1)/(rev2-rev1);

	return q*(val2-val1)+val1;
}



void carengine::changeGear( ulong gearNumber )
{
	if( gearNumber > gearCount) 
		gearNumber = gearCount;

	currentGearPtr = &gear[gearNumber];	
	currentGearNumber = gearNumber;
};

void carengine::shiftUpGear()
{

	if( currentGearNumber < gearCount )
	{
		//currentGearPtr++;
		currentGearNumber++;
		currentGearPtr = &gear[currentGearNumber];
	}
};

void carengine::shiftDownGear()
{
	if( currentGearNumber > 1 )
	{
		//currentGearPtr--;
		currentGearNumber--;
		currentGearPtr = &gear[currentGearNumber];

	}
};

//================================================================================================================================
// END OF FILE
//================================================================================================================================
