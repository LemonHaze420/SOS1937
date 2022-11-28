/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\car.h,v 1.23 1999/08/31 11:26:42 jcf Exp jcf $
	$Log: car.h,v $
	Revision 1.23  1999/08/31 11:26:42  jcf
	First check in for ages :)

	Revision 1.22  1999/03/23 10:59:48  jcf
	Start of week checkin

	Revision 1.21  1999/03/17 17:01:52  jcf
	First working threaded version

	Revision 1.20  1999/03/12 12:26:49  jcf
	Working arcade physics

	Revision 1.19  1999/03/11 11:12:58  jcf
	First version with spin and slide working. Still needs some work, of course.

	Revision 1.18  1999/03/10 12:33:24  jcf
	About to add threading

	Revision 1.17  1999/03/08 09:30:36  jcf
	First working version of physics rewrite

	Revision 1.16  1999/03/04 14:17:06  jcf
	Primitive spin and slide

	Revision 1.15  1999/03/01 09:56:08  jcf
	Stopping this revision for a bit... now I'm going to try to implement another system based on  previous work

	Revision 1.14  1999/02/17 15:09:50  jcf
	Getting closer...

	Revision 1.13  1999/02/16 12:03:11  jcf
	sort of works, but looks rather like a handbrake turn!

	Revision 1.12  1999/02/12 16:22:23  jcf
	First attempt at spin code, too complex. Will work on it more later.

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
#define	TICKRATE	50.0 // sds was 50.0


		// Types of cvar for use on constructor of 'car'

#define CARTYPE_FOURWHEELER   0
#define CARTYPE_THREEWHEELER  1

#include "Pitstop.h"

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

		car( char * arcname, float carscale, spriteFX * exhaustFX,int _id );

		bool findTimeOfImpact( float * toi , float timetoconsider );

		//Andy's Stuff

		
		
		float angvel;
		float mph;

		//Velocity is in the local coordinates of the car
		//It's magnitude is the speed (i.e. not normalised)
		vector3 velocity;
		vector3 CofG;

		int id;

		float wskid[4];

		long wpolytype[4];
		triangle * gravpoly[4];
		float wvel[4];

		float wangoffset[4];
		vector3 wheelturn[4];
		float wheeldip[4];

		float skidlookup[8][8];
		float revspin;

		//float fskidthresh[6],rskidthresh[6];
		//float fgripthresh[6],rgripthresh[6];
		float FDF[6],RDFupper[6],RDFlower[6];
		float carmass,mofinertia;
		float brk;
	

		float lorevlim[6],uprevlim[6];
		vector3 wheelv[4];
		vector3 exhaustpoint1,exhaustpoint2;

		//engine & gearing constants
		float dragfactor;
		float engbrake;
		bool changedgear;
		float idle;
		float revscale,revscaleinv;
		float maxrevs;
		float engblowuprevs;
		bool engdead;
		float engdamage;
		float engdamagethresh;
		float fmax[7];
		float intcpt1[7];
		float grad1[7];
		float intcpt2[7];
		float grad2[7];
		float lowerthresh[7];
		float upperthresh[7];
		float termvel[7];
		float speed[6];
		float grad[5];
		float dynfric[6][6];
		//Downforce is specified in g's i.e. usually downforce = 1.0
		float downforce;
		long curgear;
		//numgears=number of forward gears plus one
		long numgears;
		float revs;
		float gearratio[7],gearratioinv[7];
		
		float getEngineForce(float throttle, float brake, float wspeed2,float wspeed3);
		float skidthresh(float cspeed,bool front, bool wantskid);
		float dynamicfriction(float cspeed,bool front, bool upper);
		float gearForce(float revs);
		void showxzvector(int n, vector3 vect);

		spriteFXentry *explos,*fire1,*fire2,*fire3;
		
		vector3 firepos1,firepos2,firepos3;


		long carcarcollide,carcarhull;
		vector3 carcarpoint;
		car * carcol;
		vector3 carcarvel,carcardir,carcarforce;
		float carcarmassratio,carcarsink;

		ulong spherecount;
		Sphere * carspheres[30];

		boundBox carbb;

		float vertvel[4],oldvertvel[4],oldheight[4],vertaccel[4];
		bool flying[4],bouncemode[4];
		float gravval,bounciness;
		int readycount;

		#define POLYDAMPING 5
		float rcx[POLYDAMPING];

		//End of Andy's Stuff



		void setviewIncar();
		void setviewOutcar();

		void move(float turn, float throttle, float brake);
		bool usesmoke;
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

		//void turnAroundWorldSpacePoint( vector3 * point,  vector3 * angle );

			
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
		void MakeCollisionSound(float x,float force);
		void DoSkid(int whlnum,float skidval);
		void AddSkid(int whlnum);


//		void setWorldPosition( float wx, float wy, float wz );



//	private:

		carbody    * body;

		carwheel   * wheel[ MAXWHEELS ];			// up to 4 wheels

		ulong      totalWheels;						// depends on type of car, either 3 or 4

		float turnY;


			//
			// FIZIX
			//
	//BroadVector vel;
	//BroadMatrix orientation;
	matrix3x4 inverse_inertia_tensor_world,inverse_inertia_tensor_body;
	vector3 angular_momentum;
	vector3 angular_velocity;
	vector3 totaltorque,extrarotvel,extraforce;

	bool stuckinwall,lastcollided;
	bool stuckincar,lastcarcollided;


	void Integrate(float interval);
	float mass;
	float oneOverMass;
	float lastzvel,lastxvel;
	float xaccel,zaccel;
	int numskids,smokectr;

	vector3 acc;

		void CalculatePhysicalProperties();


			//
			// altering the position of the wheels in the world INDEPENDENTLY of the car body - this is only useful
			// when initially building a car in the 'car' constructor.
			//

		void setWheelPosition( ulong wheelNumber, vector3 * p);
		void setWheelPosition_Y( ulong wheelNumber, float p);

		void moveWheelPosition( ulong wheelNumber, vector3 * p );

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
		float accelvalue;

		vector3 collisionPoint;

		float kick, kickDelta;
		float SpinCam;
		float SpinDelta;
		int spinsoundchan;
		float spinspd,spinamt,spindir;

		model * FBlurMedModel,* FBlurHiModel;
		model * RBlurMedModel,* RBlurHiModel;
		model * usualWModel[4];

		float wheelrotateoffset[4];

		vector3 slidevel;

		bool previousspinstate;

		object * wheelShadow[4];
	
		void readOff( char * outStr, char ** strPtr, char stopChar );
		void readOffVector( vector3 * v, char ** strPtr );

		object * mainObject;

		object * bodyObject, *driverObject, *FwheelObjectLEFT, * FwheelObjectRIGHT, 
			   * RwheelObjectLEFT,  * RwheelObjectRIGHT, *FaxleObject, *RaxleObject,
			   * bodyShadowObject, * driverShadowObject, * FwheelShadowObjectLEFT, * FwheelShadowObjectRIGHT,
			   * RwheelShadowObjectLEFT, * RwheelShadowObjectRIGHT, * FaxleShadowObject, * RaxleShadowObject;

		void moveBodyAngleFromLocalPosition( vector3 * a , vector3 * p);

		//float steerYangle;

		
		float yRotationAngle;
		matrix3x4 yRotationMatrix;


		timer   carRevsCalculationTimer;
		int dummy[500];
		bool car::willCarCollide( vector3 * velocity, vector3 * acceleration, float maximumTime,
			 							      vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime, ulong * part);


		void Test();
		void Crumple(float velsc, vector3 *collisionVector,car * car2crumple);
		void DoTheCrumple(float velsc,vector3 *collisionVector, mesh *meshdata, car * car2crumple, bool norand);
		void CrumpleAPoint(vector3 *vectocrumple,float velsc,vector3 *collisionVector,car *car2crumple,bool norand);

		void doSuspension(float turn);
		bool wchange[4];
		int wrongwayct;
		bool disqualified;
		int outoffuelct;

		float initialwheely[4];

		float rollvx,rollvz;
		float xangle,zangle;	// car angles
		float zangle2,rollvz2;
		float xangledamped,zangledamped;

		float lasty,ydiff,ydiffdiff;		// first and second differentials for driver y-displacement
		float lastydiff;
		float ydiffdamped;

		bool shadow_on,pitstop_on,reverbhi_on,reverblo_on,revlimiter_on,blackflag_on;
		bool no_roll,nomove;
		float flyheight;
		vector3 bbox[8];
		void getBoundingBox();
		bool HasHitGround(float *diff);

		Pitstop *CurrentPitstop;

		int sound;		// index of the sounds, 0 is a default.
		bool compsoundplaying;	// indicates that my sound is being played
		bool skidding[4];

		float fuel,temperature,oil,fuel_low_power_drop;
		float tpc1,tpc2,tpc3,tpc4;
		float fractemprevs;
		bool blow_engine,blow_enginequietly;

		float fueltank,tyrewear,tyresfront,tyresback;

		void ProcessFuelOilAndTemperature();

		int gearchange,gearrevdampingmode;
		float gearrevdamping;
		int grevdampingdelay;
		void checkGearChange();

		// Added by IMG
		float camposzModifierInCar;

};


// Thread stuff..

#define	FRAMESPERSECOND 60





#endif	// _BS_car

//================================================================================================================================
//END OF FILE
//================================================================================================================================
