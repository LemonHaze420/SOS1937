/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\carengine.h,v 1.5 1999/01/12 17:35:15 jcf Exp jcf $
	$Log: carengine.h,v $
	Revision 1.5  1999/01/12 17:35:15  jcf
	Working now, but still needs more work.

	Revision 1.4  1999/01/12 11:45:30  jcf
	This version seems to work

*/

//================================================================================================================================
// carengine.h
// -----------
//
//================================================================================================================================

#ifndef _BS_carengine
#define _BS_carengine

#define MAXGEARS  8

class cargearData {

public:	

	float ratio;
};

class carengine {
	
	public:	
		carengine( float maxSpeed, ulong gears );
		~carengine(){};

		void setGearRatio( ulong gearNumber, float Ratio)
		{
			cargearData * gearPtr = &gear[gearNumber+1];
			gearPtr->ratio = Ratio;
		}

		void setPowerPoint(ulong p,float revs,float power)
		{
			points[p*2] = revs;
			if(revs>maxrevs)maxrevs=revs;
			points[p*2+1] = power;
		}

		ulong prevpoint;
		float points[20];
		float maxrevs;
		float getpower(float revs);
		float getRatio()
		{
			cargearData * gearPtr = &gear[currentGearNumber+1];
			return gearPtr->ratio;
		}

		void changeGear( ulong gearNumber );

		
		float	 maxVelocity;			// actual maximum velocity achievable

	//private:
	
		float massOfEngine;			// mass of engine will affect the weight-shift of the car.

		float revs;					// [0..1]
		//float velocity;				// [0..1]
		vector3 velocity;			// WORLD space velocity vector
		vector3 localVelocity;		// in local space for display purposes only
		float braking;

		float previousTransRevs;			// used for the automatic clutch
		ulong       previousGearNumber;	// JCF

		void shiftUpGear();
		void shiftDownGear();


		int       currentGearNumber;
		cargearData * currentGearPtr;
	private:

		cargearData  gear[ MAXGEARS ];
		int	 gearCount;

};

#endif	// _BS_carengine

//================================================================================================================================
//END OF FILE
//================================================================================================================================
