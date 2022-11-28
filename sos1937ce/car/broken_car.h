/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\car.h,v 1.11 1999/01/28 17:22:31 jcf Exp jcf $
	$Log: car.h,v $
	Revision 1.11  1999/01/28 17:22:31  jcf
	Quick check in. Not working.

	Revision 1.10  1999/01/26 16:16:36  jcf
	First round of tidying-up.

	Revision 1.9  1999/01/25 17:19:38  jcf
	Removed most of the spurious stuff before I start hacking the physics code around.. particularly the rotational stuff.

	Revision 1.8  1999/01/25 14:57:48  jcf
	Made a few changes, tidying up

	Revision 1.7  1999/01/22 16:31:12  jcf
	put the gear stuff out of carengine.h into this file

	Revision 1.6  1999/01/22 15:22:18  jcf
	Engine members folded in

	Revision 1.5  1999/01/18 17:22:24  jcf
	Changes to deal with variable tick lengths. Not yet complete, but working.

	Revision 1.3  1999/01/12 11:45:28  jcf
	This version seems to work

*/

//================================================================================================================================
// car.h
// -----
//				main class for cars. References the object geometry in BORIS2.lib
//				since the transforms must be used for both geometry and car physics
//================================================================================================================================

#ifndef _BS_car
#define _BS_car

#define MAXWHEELS 4


		// Types of cvar for use on constructor of 'car'

#define CARTYPE_FOURWHEELER   0
#define CARTYPE_THREEWHEELER  1

#define MAXGEARS  8

class cargearData {

public:	

	float frictionvalue;	// the HIGHER the value, the HIGHER the friction at low revs.
	float ratio;
};


class car {
	
	public:	

		~car();

		//car( ulong TypeOfCar, model * carBodyModel,
		//				   model * frontWheelModel,
		//				   model * backWheelModel,
		//				   model * inviewModel,
		//				   model * bshadowModel,
		//				   float modelscale,
		//float maxAccel,
		//  float maxBrake,
		//float maxSpeed);

		car( char * arcname, float carscale, spriteFX * exhaustFX );

		bool findTimeOfImpact( float * toi , float timetoconsider );

		void setviewIncar();
		void setviewOutcar();

		void move( ulong ticks,float turn, float throttle, float brake, ulong cycles );

		bool viewIn;
				//
				// insert ... make a car active and visible
				// remove ... makes a car inactibve and therefore invisible too (the car is NOT deleted, it still exists)
				//

		void insertIntoWorld();
		void removeFromWorld();
				
				// 
				// attaching a camera to a car
				//

		void attachCamera( camera * c, uchar aflags, vector3 * attachPoint );
		void attachCamera( uchar aflags, vector3 * attachPoint  );				// uses current camera
		void detachCamera( camera * c);
		void detachCamera();

				//
				// car physics control
				//

		void doPhysics(ulong tickTotal);

				//
				// set / change the position / angle of thc car (car body) in the world
				//
		
		void setBodyPosition( vector3 * p );
		void setBodyPosition_Y( float p );

		void moveBodyPosition( vector3 * p );

		void setBodyAngle( vector3 * a );
		void setBodyAngleNOCAMERA( vector3 * a );

		void setBodyAngle_X( float a );
		void setBodyAngle_Y( float a );
		void setBodyAngle_Z( float a );

		void moveBodyAngle( vector3 * a);
		void moveBodyAngleNOCAMERA( vector3 * a);

			
				//
				// set / change the position of the car RELATIVE to its own local axes -
				//              essential for moving the car in the direction it faces!
				//

		void moveBodyPositionInDirection( vector3 * p );					// no consideration of world

				//
				// set / change the position of the car RELATIVE to its own local axes -
				//              essential for moving the car in the direction it faces!
				// - BUT THEN REPOSITION 'Y' at 'deltaY' aABOVE THE GROUND.
				//
				// This is for testing only - the final car must rest on its WHEELS!!!!

		//bool moveBodyPositionInDirection_GROUNDLOCKED( vector3 * p, world * w, float deltaY );	// set y to the height


		void changeGear( ulong gearNumber );
		void shiftUpGear();
		void shiftDownGear();

		long  getGear();

		// jcf
		class Controller *controller;


//		void setWorldPosition( float wx, float wy, float wz );



//	private:


		carbody    * body;

		carwheel   * wheel[ MAXWHEELS ];			// up to 4 wheels

		ulong      totalWheels;						// depends on type of car, either 3 or 4

			//
			// FIZIX
			//
	//BroadVector vel;
	//BroadMatrix orientation;

	vector3 extrarotvel,extraforce;
	void Integrate(float interval);
	float mass;
	float oneOverMass;

	vector3 acc;

		void CalculatePhysicalProperties();


			//
			// altering the position of the wheels in the world INDEPENDENTLY of the car body - this is only useful
			// when initially building a car in the 'car' constructor.
			//

		void setWheelPosition( ulong wheelNumber, vector3 * p);
		void setWheelPosition_Y( ulong wheelNumber, float p);

		void moveWheelPosition( ulong wheelNumber, vector3 * p );
		void rotateCarAroundLocalPoint(vector3 &tp,float carturnangle);
		void rotateVelocityAroundY(float carturnangle);

			//
			// altering the angle of the wheels in the world INDEPENDENTLY of the car body - this is only useful
			// when initially building a car in the 'car' constructor.
			//

		void setWheelAngle( ulong wheelNumber, vector3 * a );
		void getWheelAngle( ulong wheelNumber, vector3 * a );
		void moveWheelAngle( ulong wheelNumber, vector3 * a );


		spriteFX * exhaustSpriteFX;
		timer    exhaustSpriteTimer;			// Test of 3 times per second
	    ulong exhaustMaxSequences;				// Maximum number of exhaust sprite sequences allowed
		ulong exhaustUsedSequences;				// actual number of sequences current.

		void updateExhaust();

			void startZoom( float distance, float rate );
			float zoompertick;
			float zoomdistance;
			timer zoomtimer;
			float totalzoomticks;
			float zoomproportiondone;
			float oneovertotalzoomticks;
			long  zoomtickselapsed;
			bool getZoomStatus();
			bool zoomdone;


		bool isCollidingAtTime( float tdelta );
		bool isInCollision();
		void plonkOnGround();

		bool collision;
		timer collisionSoundTimer;	
		timer skidSoundTimer;

		vector3 collisionPoint;

		vector3 kick, kickDelta;
		float SpinCam;
		float SpinDelta;
		bool isInSpin();

		// JCF engine stuff

		float gearbox(float engrevs);	// takes engine revs, returns wheel revs
		float invgearbox(float whlrevs);	
		float damped_throttle,transrevs;

		float getEngineRevs();
		float getWheelRevs();


			object * wheelShadow[4];

		void readOff( char * outStr, char ** strPtr, char stopChar );
		void readOffVector( vector3 * v, char ** strPtr );

		object * bodyObject, *driverObject, *FwheelObjectLEFT, * FwheelObjectRIGHT, 
			   * RwheelObjectLEFT,  * RwheelObjectRIGHT, *FaxleObject, *RaxleObject,
			   * bodyShadowObject, * driverShadowObject, * FwheelShadowObjectLEFT, * FwheelShadowObjectRIGHT,
			   * RwheelShadowObjectLEFT, * RwheelShadowObjectRIGHT, * FaxleShadowObject, * RaxleShadowObject;

		void setGearData( ulong gearNumber, float Ratio, float Friction)
		{
			cargearData * gearPtr = &gear[gearNumber];
			gearPtr->ratio = Ratio;
			gearPtr->frictionvalue = Friction;
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

		float	 maxVelocity;			// actual maximum velocity achievable

		cargearData * currentGearPtr;
		ulong       currentGearNumber;

		cargearData  gear[ MAXGEARS ];

		ulong	 gearCount;

		float massOfEngine;			// mass of engine will affect the weight-shift of the car.

		float revs;					// [0..1]
		//float velocity;				// [0..1]
		vector3 velocity;			// WORLD space velocity vector
		vector3 localVelocity;		// in local space for display purposes only
		float braking;

		float previousTransRevs;			// used for the automatic clutch
		ulong       previousGearNumber;	// JCF

		matrix3x4 wheelTurnMatrix;
		float totalcarturnangle;
};

#endif	// _BS_car

//================================================================================================================================
//END OF FILE
//================================================================================================================================
