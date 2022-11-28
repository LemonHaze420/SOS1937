/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\car.cpp,v 1.10 1999/01/19 13:12:15 jcf Exp jcf $
	$Log: car.cpp,v $
	Revision 1.10  1999/01/19 13:12:15  jcf
	Another more-working version, but you decelerate unnaturally in 3rd and 4th

	Revision 1.9  1999/01/18 17:22:23  jcf
	Changes to deal with variable tick lengths. Not yet complete, but working.

	Revision 1.8  1999/01/15 12:26:28  jcf
	Working version.

	Revision 1.7  1999/01/14 15:03:38  jcf
	Working version, but I'm about to chop out the weird engine friction code, or at least make it non-varying for each gear.

	Revision 1.6  1999/01/12 17:35:14  jcf
	Working now, but still needs more work.

	Revision 1.5  1999/01/12 11:45:30  jcf
	This version seems to work

	Revision 1.4  1999/01/12 09:33:28  jcf
	DO NOT USE
	Just done basic power curve code. Halfway through automatic clutch code, crashes when clutch engages!

	Revision 1.3  1999/01/08 16:44:28  jcf
	Making a lot of changes now; so this is a safety checkpoint for if they don't work

*/

static const char *rcs_id="$Id: car.cpp,v 1.10 1999/01/19 13:12:15 jcf Exp jcf $";

//================================================================================================================================
// car.cpp
// --------
//
//================================================================================================================================

#include "ALLcar.h"
#include "carsound.h"
#include "frontend.h"
#include <stdio.h>

extern FILE *debugfile;

extern spriteFX *explosFX,
	*fire1FX,
	*fire2FX,
	*fire3FX,
	*smokeFX;

// JCF engine constants

#define	THROTTLE_DAMPING	0.96f



extern void dprintf(char *,...);
extern vector3 collvector;
extern vector3 collpoint;
extern engine * boris2;
extern gamesetup GlobalGame;
extern car * cararray[];





#define HULL


void car::startZoom( float distance, float timeofzoom )
{
	#define ZOOMTICKS 10000
	#define ZOOMTICKSF 10000.0f

	if( zoomdone )
	{
		zoomdistance = distance;
		totalzoomticks = ZOOMTICKSF * timeofzoom;
		oneovertotalzoomticks = RECIPROCAL( totalzoomticks  );
		zoomtickselapsed = 0;
		zoomproportiondone = 0;
		zoompertick  = distance  * oneovertotalzoomticks;
		zoomtimer.setTimer( ZOOMTICKS );
		zoomdone = false;
	};
};


bool car::getZoomStatus()
{
	return(zoomdone);
};


void car::setBodyAngle_X( float a )
{
	mainObject->setAngle_X( a );
};

void car::setBodyAngle_Y( float a )
{
	mainObject->setAngle_Y( a );
};

void car::setBodyAngle_Z( float a )
{
	mainObject->setAngle_Z( a );
};

float anglesum = 0.0f;




void getPlaneNormal( vector3 * n, vector3 * j, vector3 * k, vector3 * l )
{
		float xkj = k->x - j->x;
		float ykj = k->y - j->y;
		float zkj = k->z - j->z;
		float xlj = l->x - j->x;
		float ylj = l->y - j->y; 
		float zlj = l->z - j->z;

		n->x = ykj * zlj - zkj * ylj;
		n->y = zkj * xlj - xkj * zlj;
		n->z = xkj * ylj - ykj * xlj;
		n->Normalise();			
};

void car::plonkOnGround()
{
	float deltaW, deltaB, bodyY;
	triangle * bodytrianglePtr;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3;
	
	#define DELTAANGLE 0.002f

	mainObject->inheritTransformations();	
	
	float min, max;

		if( !boris2->currentworld->getY(&mainObject->concatenatedTransform.translation, &bodyY -1, &bodytrianglePtr, false ) )
		{
			bodytrianglePtr = NULL;
			bodyY = 0.0f;
		};
		
		// Get heights of wheels and body etc. from the new position
		
		if( !wheel[0]->getWorldHeight( &heightvalue0 , &gravpoly[0]) )
		{
			heightvalue0 = 0.0f;
			gravpoly[0] = NULL;
		};
		heightvalue0 += //wheel[0]->radius + 
						(wheel[0]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[0]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[0]->wheelObject->transform.translation.y; 

		
		if( !wheel[1]->getWorldHeight( &heightvalue1 , &gravpoly[1] ) )
		{
			heightvalue1 = 0.0f;
			gravpoly[1] = NULL;
		};
		heightvalue1 += //wheel[1]->radius;
		//wheel[0]->radius + 
						(wheel[1]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[1]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[1]->wheelObject->transform.translation.y; 

		if( !wheel[2]->getWorldHeight( &heightvalue2 , &gravpoly[2]) )
		{
			heightvalue2 = 0.0f;
			gravpoly[2] = NULL;
		};
		heightvalue2 += //wheel[2]->radius;
		//wheel[0]->radius + 
						(wheel[2]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[2]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[2]->wheelObject->transform.translation.y; 

		if( !wheel[3]->getWorldHeight( &heightvalue3 , &gravpoly[3] ) )
		{
			heightvalue3 = 0.0f;
			gravpoly[3] = NULL;
		};
		heightvalue3 += //wheel[3]->radius;
		//wheel[0]->radius + 
						(wheel[3]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[3]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[3]->wheelObject->transform.translation.y; 

		//heightsum = heightvalue0 + heightvalue1 + heightvalue2 + heightvalue3;
		
		//
		
		vector3 *v0 = &(wheel[0]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v1 = &(wheel[1]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v2 = &(wheel[2]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v3 = &(wheel[3]->wheelObject->concatenatedTransform.translation) ;
		
		static vector3 mid01;
		static vector3 mid23;
		
		mid01.x = (v0->x + v1->x) * 0.5f;
		mid01.y = (heightvalue0 + heightvalue1) * 0.5f;
		mid01.z = (v0->z + v1->z) * 0.5f;
		
		mid23.x = (v2->x + v3->x) * 0.5f;
		mid23.y = (heightvalue2 + heightvalue3) * 0.5f;
		mid23.z = (v2->z + v3->z) * 0.5f;
		
		static vector3 mid02;
		static vector3 mid13;
		
		mid02.x = (v0->x + v2->x) * 0.5f;
		mid02.y = (heightvalue0 + heightvalue2) * 0.5f;
		mid02.z = (v0->z + v2->z) * 0.5f;
		
		mid13.x = (v1->x + v3->x) * 0.5f;
		mid13.y = (heightvalue1 + heightvalue3) * 0.5f;
		mid13.z = (v1->z + v3->z) * 0.5f;
				
		
		float  dyA = -(mid01.y - mid23.y);
		float  dxA = -(mid01.x - mid23.x);
		float  dzA = -(mid01.z - mid23.z);
		
		
		float dyB = -(mid13.y - mid02.y);
		float dxB = -(mid13.x - mid02.x);
		float dzB = -(mid13.z - mid02.z);
		
		
		float sinx = ( dyA * RECIPROCAL( sqrt( dxA * dxA + dyA * dyA + dzA * dzA ) ));
		float sinz = ( dyB * RECIPROCAL( sqrt( dxB * dxB + dyB * dyB + dzB * dzB ) ));
		
		float cosx = sqrt(1 - sinx*sinx);
		float cosz = sqrt(1 - sinz*sinz);
		
		// We need to calculate the final transform by calculating a transform matrix for this angle and
		// multiplying the actual 'transform' matrix by this one. WE CANNOT JUST ADD THIS ANGLE SINCE IT
		// represents a transform on the object AFTER the first transform.
		
		
		static matrix3x4 RX, RZ,  tempm, temp1;
		
		RX.set3x3submat(	1,   0,   0,  
			0,  cosx,  sinx,  
			0,  -sinx,  cosx
			);
		
		RZ.set3x3submat( 	cosz,  sinz, 0, 
			-sinz, cosz, 0, 
			0, 0, 1
			);

		temp1.multiply3x3submats( &RX, &RZ);


		tempm.multiply3x3submats( &yRotationMatrix, &temp1);
		tempm.translation=mainObject->transform.translation;

		mainObject->transform = tempm;
		mainObject->changed   = true;
		
		
		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
				
		max = (max+min)*0.5f;
		
		setBodyPosition_Y(  max );			// Note 'changed' flag set ready for new resolve of geometry
		mainObject->inheritTransformations();



};



extern car *playercar;
extern spriteFX *tyresmokeFX;



#define REVCALCTICKS 100

void car::AddSkid(int whlnum)
{
	vector3 p;
	p = *(wheel[whlnum]->getPositionInWorld());
	p.y -= 0.5;
	boris2->appendSpriteFX( tyresmokeFX, &p, NULL, false, false);

	numskids++;
}



void car::DoSkid(int whlnum,float skidval)
{
	// removed a lot of tests from here... this used to be quite big.
	if(gravpoly[whlnum])AddSkid(whlnum);
}




void dumpmatrix(char *s,matrix3x4 *m)
{
return;
	dprintf("\n\nMATRIX DUMP : %s",s);
	dprintf("%f %f %f",m->E00,m->E01,m->E02);
	dprintf("%f %f %f",m->E10,m->E11,m->E12);
	dprintf("%f %f %f",m->E20,m->E21,m->E22);
	dprintf("TX : %f %f %f",m->translation.x,m->translation.y,m->translation.z);
}


void car::StartTumble(float strength)
{
	tumbling=true;
	tumstrength = strength*0.01;
	tumpoint = -1;
}


#define	CHECKOBJECT(q)	\
		if(q->modelData->boundingBox.lo.x < tminx) tminx = q->modelData->boundingBox.lo.x;	\
		if(q->modelData->boundingBox.lo.y < tminy) tminy = q->modelData->boundingBox.lo.y;	\
		if(q->modelData->boundingBox.lo.z < tminz) tminz = q->modelData->boundingBox.lo.z;	\
		if(q->modelData->boundingBox.hi.x > tmaxx) tmaxx = q->modelData->boundingBox.hi.x;	\
		if(q->modelData->boundingBox.hi.y > tmaxy) tmaxy = q->modelData->boundingBox.hi.y;	\
		if(q->modelData->boundingBox.hi.z > tmaxz) tmaxz = q->modelData->boundingBox.hi.z;

void car::UpdateTumblingRotationPoint()
{
	if(recalculate_overall_bounding_box)
	{
		tminx = tminy = tminz = 9999.0f;
		tmaxx = tmaxy = tmaxz = -9999.0f;

		CHECKOBJECT(bodyObject);

		CHECKOBJECT(FwheelObjectLEFT);
		CHECKOBJECT(FwheelObjectRIGHT);
		CHECKOBJECT(RwheelObjectLEFT);
		CHECKOBJECT(RwheelObjectRIGHT);

		recalculate_overall_bounding_box = false;
	}


	// OK, we have a boundy box in case we need it. Let's now get the actual coordinates of
	// the tumpoint.

	localtp.x = (tumpoint & 1) ? tmaxx : tminx;
	localtp.z = (tumpoint & 2) ? tmaxz : tminz;
	localtp.y = (tumpoint & 4) ? tmaxy : tminy;

	mainObject->transform.multiplyV3By3x4mat(&tumpointv,&localtp);

	dprintf("rotation point: %f %f %f",localtp.x,localtp.y,localtp.z);

//	localtp=vector3(0,0,2);	//snark
}

void car::DoTumble()
{
	float cosa,sina,cosa1;
	cosa = cos(tumstrength);sina=sin(tumstrength);cosa1=1-cosa;

	// at this point, tumaxis is a quaternion describing a rotation. Really. So now we build a rotation
	// matrix for a rotation of 'strength' around 'tumaxis'

	static matrix3x4 rotmat,transmat,invtransmat,r1,r2,r3;

	rotmat.translation=vector3(0,0,0);

//	rotmat.set3x3submat(	cosa,sina,0,	-sina,cosa,0,	0,0,1	);

	float x=1,y=0,z=0;

	rotmat.set3x3submat(	x*x+cosa*(1-x*x),	x*y*cosa1+z*sina,	z*x*cosa1-y*sina,
							x*y*cosa1-z*sina,	y*y+cosa*(1-y*y),	y*z*cosa1+x*sina,
							z*x*cosa1+y*sina,	y*z*cosa1-x*sina,	z*z+cosa*(1-z*z)	);

//	vector3 tmpv(0,3,0);
//	mainObject->transform.multiplyV3By3x4mat(&tumpointv,&tmpv);

	transmat.set3x3submat(1,0,0,0,1,0,0,0,1);
	invtransmat=transmat;
	invtransmat.translation = mainObject->transform.translation * -1;
	transmat.translation = mainObject->transform.translation;

	dumpmatrix("t1",&invtransmat);
	dumpmatrix("rot",&rotmat);
	dumpmatrix("t2",&transmat);

	r1.multiply3x4mats(&invtransmat,&rotmat);
	r2.multiply3x4mats(&r1,&transmat);

	dumpmatrix("transformation",&r2);

	dumpmatrix("Car before Tx to pos",&mainObject->transform);


	r1.multiply3x4mats(&mainObject->transform,&rotmat);

	mainObject->transform = r1;

	dumpmatrix("Car after Tx to pos",&mainObject->transform);


	

	mainObject->changed = true;
	mainObject->inheritTransformations();
	// 4) if the car is more or less flat, then turn off tumbling and set the rotation point to -1



}








void car::move(float turn, float throttle, float  brake)
{



	float linearity=1.0f;

//	if(this == playercar)linearity = 1.0;

	if (turn<0.0f)
		turn=linearity*turn - (1.0-linearity)*turn*turn;
	else
		turn=linearity*turn + (1.0-linearity)*turn*turn;

	numskids = 0;
	
	//Note: Throttle will have to be changed to accomodate engine characteristics
	
	float engpow = throttle;
	float friction=1.0f;



	//Set wheel turning - fall off sensitivity exponentially with velocity
	//  0.5235f = 30 degrees
//	float turnmax=0.5235f;
	float turnmax=0.35f;

	//float turnmax=0.05f+(0.4735f * exp(-0.05f*velocity.Mag()));
	float relax = 0.7f;


	static vector3 wheelturn[4]={vector3 (0.0f,0.0f,0.0f),
								vector3 (0.0f,0.0f,0.0f),
								vector3 (0.0f,0.0f,0.0f),
								vector3 (0.0f,0.0f,0.0f)};
	
	vector3 curang;
	wheel[0]->wheelObject->getAngle(&curang);

	if (turn == 0.0f)
	{
		 wheelturn[0].y = float(curang.y * relax);
	}
	else
	{
		wheelturn[0].y = -float(turnmax * turn);
	}
	

	//Set wheel angles
	wheelturn[0] -= curang;

	//use for looking at wheel angles later
	curang+=wheelturn[0];


	//get wheels 0's & 2's z vector wrt car body - they both have zero z rotation as default.
	//wheels 1 & 3 have z rotation = pi

	//0 = front right
	//1 = front left
	//2 = rear right
	//3 = rear left

	vector3 fwheelz;
	
	//rear wheels always point in car direction
	vector3 rwheelz = vector3 (0.0f,0.0f,1.0f);


	fwheelz.x = sin(curang.y);
	fwheelz.y = 0.0f;
	fwheelz.z = cos(curang.y);

	//Need to add in velocity of rotation for each wheel
	//Assume for the moment that we are rotating around CofG
	//(Might need to change that later)
	vector3 pos[4],rpos[4],npos[4];
	
	pos[0]=(wheel[0]->wheelObject->transform.translation)-CofG;
	pos[1]=(wheel[1]->wheelObject->transform.translation)-CofG;
	pos[2]=(wheel[2]->wheelObject->transform.translation)-CofG;
	pos[3]=(wheel[3]->wheelObject->transform.translation)-CofG;

	//zero the y component as we're considering a 2D case

	pos[0].y=0.0f;
	pos[1].y=0.0f;
	pos[2].y=0.0f;
	pos[3].y=0.0f;

	//normalise the position vectors - needed for later
	npos[0] = pos[0]; npos[0].Normalise();//(pos[0])*(1.0f/(pos[0].Mag()));
	npos[1] = pos[1]; npos[1].Normalise();
	npos[2] = pos[2]; npos[2].Normalise();
	npos[3] = pos[3]; npos[3].Normalise();

	
	//Get the vectors perpendicular to pos in the xz plane for rotational direction
	//Don't need to normalise as we need the size of 'r' for v=rw.
	rpos[0].x=pos[0].z;
	rpos[0].y=0.0f;
	rpos[0].z=-pos[0].x;
	rpos[1].x=pos[1].z;
	rpos[1].y=0.0f;	
	rpos[1].z=-pos[1].x;
	rpos[2].x=pos[2].z;
	rpos[2].y=0.0f;
	rpos[2].z=-pos[2].x;
	rpos[3].x=pos[3].z;
	rpos[3].y=0.0f;
	rpos[3].z=-pos[3].x;
	
	//Do vector sum of rotational velocity with body velocity to get wheel velocities
	vector3 wheelv[4];
	float wheelvmag[4];

	//Shouldn't need this next line, it's just to make sure nothing funny is happening
	//to angvel around zero to cause wierd effects elsewhere
	//Remove line below at earliest opportunity
	if ((angvel<0.000001f)&&(angvel>-0.000001f)) angvel=0.0f;
	//Remove above line at earliest opportunity
	
	wheelv[0]=velocity+(rpos[0]*angvel);
	wheelv[1]=velocity+(rpos[1]*angvel);
	wheelv[2]=velocity+(rpos[2]*angvel);
	wheelv[3]=velocity+(rpos[3]*angvel);

	wheelvmag[0]=wheelv[0].Mag();
	wheelvmag[1]=wheelv[1].Mag();
	wheelvmag[2]=wheelv[2].Mag();
	wheelvmag[3]=wheelv[3].Mag();

	//Turn wheels here now that we know the wheel velocities

	float locwheelv[4];
	float timeint = 1.0f/TICKRATE;

	locwheelv[0] = fwheelz.x*wheelv[0].x + fwheelz.z*wheelv[0].z;
	locwheelv[1] = fwheelz.x*wheelv[1].x + fwheelz.z*wheelv[1].z;
	locwheelv[2] = wheelv[2].z;
	locwheelv[3] = wheelv[3].z;
	
	wheelturn[1]=wheelturn[0];


	//Sort out wheel turning for skidding and wheel spinning

	if (wskid[0]>0.0f && brake>0.0f)
		//brake is locking the wheel
		wheelturn[0].x=0.0f;
	else
		//normal turning
		wheelturn[0].x=timeint*locwheelv[0]*wheel[0]->oneoverradius;
	
	if (wskid[1]>0.0f && brake>0.0f)
		//brake is locking the wheel
		wheelturn[1].x=0.0f;
	else
		//normal turning
		wheelturn[1].x=timeint*locwheelv[1]*wheel[1]->oneoverradius;
	
	//define normal turning rate for wheel2
	float normturn;
	normturn = timeint*locwheelv[2]*wheel[2]->oneoverradius;

	if (wskid[2]<0.0f)
		//normal turning
		wheelturn[2].x=normturn;
	else if (brake>0.0f)
		//brake is locking the wheel
		wheelturn[2].x=0.0f;
	else
	{
		//wheel spinning - turning is proportional to revs
		wheelturn[2].x=timeint*revs*gearratioinv[curgear]*revscaleinv*PI;
		//don't let the wheels spin slower than the normal turning rate
		if (fabs(wheelturn[2].x) <fabs(normturn))wheelturn[2].x=normturn;
	}
	
	//define normal turning rate for wheel3
	normturn = timeint*locwheelv[3]*wheel[3]->oneoverradius;
	if (wskid[3]<0.0f)
		//normal turning
		wheelturn[3].x = normturn;
	else if (brake>0.0f)
		//brake is locking the wheel
		wheelturn[3].x=0.0f;
	else
	{
		//wheel spinning - turning is proportional to revs
		wheelturn[3].x=timeint*revs*gearratioinv[curgear]*revscaleinv*PI;
		//don't let the wheels spin slower than the normal turning rate
		if (fabs(wheelturn[3].x) <fabs(normturn)) wheelturn[3].x=normturn;
	}
	

	wheel[0]->wheelObject->moveAngle(&wheelturn[0]);
	wheel[1]->wheelObject->moveAngle(&wheelturn[1]);
	wheel[2]->wheelObject->moveAngle(&wheelturn[2]);
	wheel[3]->wheelObject->moveAngle(&wheelturn[3]);


	// (gods, I hope this works)

	if(tumbling)
	{
		DoTumble();return;
	}
















	//Get cosine and sine of angle between velocity and front & rear wheels
	float cosang[4],sinang[4];

	float thresh=0.001f;
	
	if (wheelvmag[0]<thresh)
	{
		cosang[0]=1.0f;
		sinang[0]=0.0f;
		wheelvmag[0]=0.0f;
	}
	else
	{
		cosang[0]=((wheelv[0].x * fwheelz.x) + (wheelv[0].z * fwheelz.z))/wheelvmag[0];
		sinang[0]=((wheelv[0].z * fwheelz.x) - (wheelv[0].x * fwheelz.z))/wheelvmag[0];
	}

	if (wheelvmag[1]<thresh)
	{
		cosang[1]=1.0f;
		sinang[1]=0.0f;
		wheelvmag[1]=0.0f;
	}
	else
	{
		cosang[1]=((wheelv[1].x * fwheelz.x) + (wheelv[1].z * fwheelz.z))/wheelvmag[1];
		sinang[1]=((wheelv[1].z * fwheelz.x) - (wheelv[1].x * fwheelz.z))/wheelvmag[1];
	}


	if (wheelvmag[2]<thresh)
	{
		cosang[2]=1.0f;
		sinang[2]=0.0f;
		wheelvmag[2]=0.0f;
	}
	else
	{
		cosang[2]=(wheelv[2].z * rwheelz.z)/wheelvmag[2];
		sinang[2]=-wheelv[2].x/wheelvmag[2];
	}
	
	if (wheelvmag[3]<thresh)
	{
		cosang[3]=1.0f;
		sinang[3]=0.0f;
		wheelvmag[3]=0.0f;
	}
	else
	{
		cosang[3]=(wheelv[3].z * rwheelz.z)/wheelvmag[3];
		sinang[3]=-wheelv[3].x/wheelvmag[3];
	}

	//--------------------------------------------------------------------------
	//Find the forces on each of the wheels
	//--------------------------------------------------------------------------
	vector3 fvect[4];
	float forcex1[4],forcex2[4],forcez1[4],forcez2[4],inter[4],eng[4];
	float scalef=10000.0f;
	
	//Fudge the friction for doing power slides
	float cspeed=velocity.Mag();
	if (wskid[2]>-0.5f && wskid[3]>-0.5f)
	{
		friction=1.57f-0.04f*cspeed;
		if(friction<0.05f)
		{
			AddSkid(2);AddSkid(3);
		}
		if (friction>1.0f) friction=1.0f;
		if (friction<0.1f) friction=0.2f;

	}
	else
	{
		friction=1.0f;
	}

	inter[0]=wheelvmag[0]*friction*scalef;
	inter[1]=wheelvmag[1]*friction*scalef;
	inter[2]=wheelvmag[2]*friction*scalef;
	inter[3]=wheelvmag[3]*friction*scalef;

	float sgn2=(cosang[2]<0.0f)?-1.0f:1.0f;
	float sgn3=(cosang[3]<0.0f)?-1.0f:1.0f;


	float unmodfriction=1.0f;
	eng[0]=unmodfriction*getEngineForce(throttle,brake,wheelvmag[2]*sgn2,wheelvmag[3]*sgn3);
	//eng[0]=friction*engstrength*engpow*scalef;
	eng[1]=eng[0];
	eng[2]=eng[0];
	eng[3]=eng[0];


	//Calculate the contribution due to gravity
	
	//Gravity must be taken into account at the wheel level 
	//for it to correctly affect the car
	vector3 wheelgrav[4];
	
	wheelgrav[0]= vector3 (0.0f,1.0f,0.0f);
	wheelgrav[1]= vector3 (0.0f,1.0f,0.0f);
	wheelgrav[2]= vector3 (0.0f,1.0f,0.0f);
	wheelgrav[3]= vector3 (0.0f,1.0f,0.0f);
	
	matrix3x4 invbodymat;
	invbodymat.Transpose3x4(&mainObject->transform);

	//if (gravpoly[0])
	if (gravpoly[0])
	{
		vector3 gp = gravpoly[0]->faceNormal;
		vector3 tmp = vector3 (gp.x * gp.y,(gp.y * gp.y) -1.0f,gp.y * gp.z);
		invbodymat.multiplyV3By3x3mat(&wheelgrav[0],&tmp);
	}
	
	if (gravpoly[1])
	{
		vector3 gp = gravpoly[1]->faceNormal;
		vector3 tmp = vector3 (gp.x * gp.y,(gp.y * gp.y) -1.0f,gp.y * gp.z);
		invbodymat.multiplyV3By3x3mat(&wheelgrav[1],&tmp);
	}
	
	if (gravpoly[2])
	{
		vector3 gp = gravpoly[2]->faceNormal;
		vector3 tmp = vector3 (gp.x * gp.y,(gp.y * gp.y) -1.0f,gp.y * gp.z);
		invbodymat.multiplyV3By3x3mat(&wheelgrav[2],&tmp);
	}
	
	if (gravpoly[3])
	{
		vector3 gp = gravpoly[3]->faceNormal;
		vector3 tmp = vector3 (gp.x * gp.y,(gp.y * gp.y) -1.0f,gp.y * gp.z);
		invbodymat.multiplyV3By3x3mat(&wheelgrav[3],&tmp);
	}
	


	//Gravity acting sideways on the wheel is only noticed when the car is moving
	//as there is a small loss in traction. Define a maximum speed at which gravity
	//acts at its fullest sideways, and linearly drop it off down to speed = 0.

	//float maxgravsp=0.025f; //Reciprocal of max speed i.e. in this case 1 / 40 m.s-1
	float maxgravsp=0.5f; //Reciprocal of max speed i.e. in this case 1 / 2 m.s-1

	float gravspmod=cspeed*maxgravsp;
	if (gravspmod>1.0f)gravspmod = 1.0f;

	//This is assuming that the gravitational forces on the front wheels are the same.
	//Similarly for the rear wheels.
	wheelgrav[0]=(wheelgrav[0]+wheelgrav[1])*0.5;
	wheelgrav[2]=(wheelgrav[2]+wheelgrav[3])*0.5;

	//grdebug("fwheelgrav x",wheelgrav[0].x);
	//grdebug("fwheelgrav y",wheelgrav[0].y);
	//grdebug("fwheelgrav z",wheelgrav[0].z);

	float fgravx,fgravz,rgravx,rgravz;
	vector3 fx,fz,rx,rz; //Vectors for local wheel x and z directions in body space	

	fx = vector3 (fwheelz.z,0.0f,-fwheelz.x);
	fz = fwheelz;
	rx = vector3 (1.0f,0.0f,0.0f);
	rz = vector3 (0.0f,0.0f,1.0f);

	//Ignore y component. Plonk on ground has effectively taken care of that.
	
	fgravx = fx.x * wheelgrav[0].x + fx.z * wheelgrav[0].z;
	fgravz = fz.x * wheelgrav[0].x + fz.z * wheelgrav[0].z;
	rgravx = wheelgrav[2].x;
	rgravz = wheelgrav[2].z;

	//The gravitational force on the car body will act on the front and rear wheels according
	//to their relative positions to the centre of gravity

	float posgrav = pos[0].z/(pos[0].z-pos[2].z);
	fgravx*=(1-posgrav);
	fgravz*=(1-posgrav);
	rgravx*=posgrav;
	rgravz*=posgrav;

	//Put in a fudge factor for making the car turn 'correctly' around banked corners
	//Speed for max gravity effect will be 60 m.s-1
	//float gravxmult=12.0f*(1-exp(-cspeed*0.05));
	float gravxmult=1.0f;

	fgravx*=carmass*10.0f*gravxmult;
	fgravz*=carmass*10.0f;
	rgravx*=carmass*10.0f*gravxmult;
	rgravz*=carmass*10.0f;

	//grdebug("posgrav",posgrav);
	grdebug("fgravx",fgravx);
	grdebug("fgravz",fgravz);
	grdebug("rgravx",rgravx);
	grdebug("rgravz",rgravz);

	//boris2->showmagnitude(0,-1,1,cosang[0]);
	//boris2->showmagnitude(1,-1,1,sinang[0]);

	#define FRONTW		true
	#define REARW		false
	#define UPPERF		true
	#define DONTCARE	true
	#define LOWERF		false

	float mphspeed=cspeed*2.25;
	
	//Wheel 0 Forces (Front Right)
	if(wskid[0]<-0.5f)
	{
		forcex1[0]=inter[0]*sinang[0]+fgravx*gravspmod;
		forcez1[0]=-inter[0]*brk*brake*cosang[0]+fgravz;
		//force*1 is for moving the car correctly
		//force*2 is for working out the force thresholds correctly
		forcex2[0]=forcex1[0]-2.0f*fgravx*gravspmod;
		forcez2[0]=forcez1[0]-fgravz-fgravz;
	}
	else
	{
		forcex1[0]=inter[0]*sinang[0] * (1.0f+wskid[0]*(dynamicfriction(mphspeed,FRONTW,DONTCARE)-1.0f))+fgravx;
		forcez1[0]=-inter[0]*brk*brake*cosang[0] * (1.0f+wskid[0]*(dynamicfriction(mphspeed,FRONTW,DONTCARE)-1.0f))+fgravz;
		forcex2[0]=forcex1[0]-fgravx-fgravx;
		forcez2[0]=forcez1[0]-fgravz-fgravz;
	}

	//Wheel 1 Forces (Front Left)	
	if(wskid[1]<-0.5f)
	{
		forcex1[1]=inter[1]*sinang[1]+fgravx*gravspmod;
		forcez1[1]=-inter[1]*brk*brake*cosang[1]+fgravz;
		forcex2[1]=forcex1[1]-2.0f*fgravx*gravspmod;
		forcez2[1]=forcez1[1]-fgravz-fgravz;
	}
	else
	{
		forcex1[1]=inter[1]*sinang[1] * (1.0f+wskid[1]*(dynamicfriction(mphspeed,FRONTW,DONTCARE)-1.0f))+fgravx;
		forcez1[1]=-inter[1]*brk*brake*cosang[1] * (1.0f+wskid[1]*(dynamicfriction(mphspeed,FRONTW,DONTCARE)-1.0f))+fgravz;
		forcex2[1]=forcex1[1]-fgravx-rgravx;
		forcez2[1]=forcez1[1]-fgravz-rgravz;
	}

	
	float varfrict = (dynamicfriction(mphspeed,REARW,LOWERF)+(dynamicfriction(mphspeed,REARW,UPPERF)-dynamicfriction(mphspeed,REARW,LOWERF))*(1.0f-(throttle*throttle*throttle)));
	
	//Wheel 2 Forces (Rear Right)
	if(wskid[2]<-0.5f)
	{
		forcex1[2]=inter[2]*sinang[2]+rgravx*gravspmod;
		forcez1[2]=eng[2]-(inter[2]*brk*brake)*cosang[2]+rgravz;
		forcex2[2]=forcex1[2]-2.0f*rgravx*gravspmod;
		forcez2[2]=forcez1[2]-rgravz-rgravz;
	}
	else
	{
		forcex1[2]=inter[2]*sinang[2] * (1.0f+wskid[2]*(varfrict-1.0f))+rgravx;
		forcez1[2]=eng[2]-((inter[2]*brk*brake)*cosang[2]) * (1.0f+wskid[2]*(varfrict-1.0f))+rgravz;
		//forcez[2]=(eng[2]-(inter[2]*brk*brake)*cosang[2])*RDFupper[curgear];
		forcex2[2]=forcex1[2]-rgravx-rgravx;
		forcez2[2]=forcez1[2]-rgravz-rgravz;
	}

	//Wheel 3 Forces (Rear Left)
	if(wskid[3]<-0.5f)
	{

		forcex1[3]=inter[3]*sinang[3]+rgravx*gravspmod;
		forcez1[3]=eng[3]-(inter[3]*brk*brake)*cosang[3]+rgravz;
		forcex2[3]=forcex1[3]-2.0f*rgravx*gravspmod;
		forcez2[3]=forcez1[3]-rgravz-rgravz;
	}
	else
	{

		forcex1[3]=inter[3]*sinang[3] * (1.0f+wskid[3]*(varfrict-1.0f))+rgravx;
		forcez1[3]=eng[3]-((inter[3]*brk*brake)*cosang[3]) * (1.0f+wskid[3]*(varfrict-1.0f))+rgravz;
		//forcez[3]=(eng[3]-(inter[3]*brk*brake)*cosang[3])*RDFupper[curgear];
		forcex2[3]=forcex1[3]-rgravx-rgravx;
		forcez2[3]=forcez1[3]-rgravz-rgravz;

	}

	//Find force size and direction on wheel by vector sum
	
	float fvmag[4];

	fvect[0] = fx*forcex1[0] + fz*forcez1[0];
	fvect[1] = fx*forcex1[1] + fz*forcez1[1];
	fvect[2] = rx*forcex1[2] + rz*forcez1[2];
	fvect[3] = rx*forcex1[3] + rz*forcez1[3];

	float fvmag2[4];
	
	fvmag2[0]=sqrt(forcex1[0]*forcex1[0]+forcez1[0]*forcez1[0]);
	fvmag2[1]=sqrt(forcex1[1]*forcex1[1]+forcez1[1]*forcez1[1]);
	fvmag2[2]=sqrt(forcex1[2]*forcex1[2]+forcez1[2]*forcez1[2]);
	fvmag2[3]=sqrt(forcex1[3]*forcex1[3]+forcez1[3]*forcez1[3]);
	
	fvmag[0]=sqrt(forcex2[0]*forcex2[0]+forcez2[0]*forcez2[0]);
	fvmag[1]=sqrt(forcex2[1]*forcex2[1]+forcez2[1]*forcez2[1]);
	fvmag[2]=sqrt(forcex2[2]*forcex2[2]+forcez2[2]*forcez2[2]);
	fvmag[3]=sqrt(forcex2[3]*forcex2[3]+forcez2[3]*forcez2[3]);
	
	float fskidincrement,rskidincrement;
	fskidincrement = 0.05f;
	rskidincrement = 0.05f;

	//if (wskid[0]>-0.5f) boris2->showmagnitude(0,-1,2,(1.0f+wskid[0]*(FDF[curgear]-1.0f)));
	//if (wskid[1]>-0.5f) boris2->showmagnitude(1,-1,2,(1.0f+wskid[1]*(FDF[curgear]-1.0f)));
	//if (wskid[2]>-0.5f) boris2->showmagnitude(2,-1,2,(1.0f+wskid[2]*(varfrict-1.0f)));
	//if (wskid[3]>-0.5f) boris2->showmagnitude(3,-1,2,(1.0f+wskid[3]*(varfrict-1.0f)));

//	float maxf = 80.0f*carmass;
	float maxf = 80000.0f;
	

	float mod[4];

	if (fvmag2[0]>maxf)
	{
		mod[0]=maxf/fvmag2[0];
		fvect[0]*=mod[0];
		fvmag[0]*=mod[0];
		fvmag2[0]=maxf;
	}

	if (fvmag2[1]>maxf)
	{
		mod[1]=maxf/fvmag2[1];
		fvect[1]*=mod[1];
		fvmag[1]*=mod[1];
		fvmag2[1]=maxf;
	}

	if (fvmag2[2]>maxf)
	{
		mod[2]=maxf/fvmag2[2];
		fvect[2]*=mod[2];
		fvmag[2]*=mod[2];
		fvmag2[2]=maxf;
	}

	if (fvmag2[3]>maxf)
	{
		mod[3]=maxf/fvmag2[3];
		fvect[3]*=mod[3];
		fvmag[3]*=mod[3];
		fvmag2[3]=maxf;
	}

	
	
	
//#define FRONTW	true
//#define REARW		false
#define SKIDVAL	true
#define GRIPVAL	false

	grdebug("mphspeed",mphspeed);
	grdebug("frontskid",skidthresh(mphspeed,FRONTW,SKIDVAL));



	if (wskid[0]<-0.5f)
	{
		if (fvmag[0]>skidthresh(mphspeed,FRONTW,SKIDVAL))
		{
			wskid[0]=0.5f;	
			//fvect[0]=fvect[0]*FDF[curgear];
		}
	}
	else
	{
		if (fvmag[0]<skidthresh(mphspeed,FRONTW,GRIPVAL))
		{
			wskid[0]-=fskidincrement;
			if (wskid[0]<0.0f)wskid[0]=-1.0f;
		}
		else
		{
			DoSkid(0,(fvmag[0]-skidthresh(mphspeed,FRONTW,GRIPVAL)));
			if (wskid[0]<1.0f)wskid[0]+=fskidincrement;
		}
	}



	
	if (wskid[1]<-0.5f)
	{
		if (fvmag[1]>skidthresh(mphspeed,FRONTW,SKIDVAL))
		{
			wskid[1]=0.5f;
			//fvect[1]=fvect[1]*FDF[curgear];
		}
	}
	else
	{
		if (fvmag[1]<skidthresh(mphspeed,FRONTW,GRIPVAL))
		{
	
			wskid[1]-=fskidincrement;
			if (wskid[1]<0.0f)wskid[1]=-1.0f;
		}
		else
		{
			DoSkid(1,fvmag[1]-skidthresh(mphspeed,FRONTW,GRIPVAL));
			if (wskid[1]<1.0f)wskid[1]+=fskidincrement;
		}
	}


	if (wskid[2]<-0.5f)
	{
		if (fvmag[2]>skidthresh(mphspeed,REARW,SKIDVAL)) 
		{
			wskid[2]=0.5f;
			//fvect[2]=fvect[2]*varfrict;
		}
	}
	else
	{
		if (fvmag[2]<skidthresh(mphspeed,REARW,GRIPVAL))
		{
			wskid[2]-=rskidincrement;
			if (wskid[2]<0.0f)wskid[2]=-1.0f;
		}
		else
		{
			DoSkid(2,fvmag[2]-skidthresh(mphspeed,REARW,GRIPVAL));
			if (wskid[2]<1.0f)wskid[2]+=rskidincrement;
		}
	}



	if (wskid[3]<-0.5f)
	{
		if (fvmag[3]>skidthresh(mphspeed,REARW,SKIDVAL)) 
		{
			wskid[3]=0.5f;
			//fvect[3]=fvect[3]*varfrict;
		}
	}
	else
	{
		if (fvmag[3]<skidthresh(curgear,REARW,GRIPVAL))
		{
			wskid[3]-=rskidincrement;
			if (wskid[3]<0.0f)wskid[3]=-1.0f;
		}
		else
		{
			DoSkid(3,fvmag[3]-skidthresh(curgear,REARW,GRIPVAL));
			if (wskid[3]<1.0f)wskid[3]+=rskidincrement;
		}
	}




	/*
	char myBuffer[1024];
	sprintf(myBuffer,"\nSkidding = %.1f  Force on 3 = %.1f\n",wskid[3],fvmag[3]);
	OutputDebugString(myBuffer);
	*/



	grdebug("skid0",wskid[0]);
	grdebug("skid1",wskid[1]);
	grdebug("skid2",wskid[2]);
	grdebug("skid3",wskid[3]);
	
	//Calculate torques and sum them.    Torque = r x F
	//Since vectors are in 2D, only have to calc part of X-product
	//Centre of gravity will be taken into account

	float torq[4]; //torque is always in y direction so don't store as vector3

	//pos is defined at the wheel velocity stage

	
	boris2->showmagdir(0,20000.0f,fvect[0]);
	boris2->showmagdir(1,20000.0f,fvect[1]);
	boris2->showmagdir(2,20000.0f,fvect[2]);
	boris2->showmagdir(3,20000.0f,fvect[3]);
	
	wheelv[0].Normalise();
	wheelv[1].Normalise();
	wheelv[2].Normalise();
	wheelv[3].Normalise();

	torq[0]=(fvect[0].x*pos[0].z)-(fvect[0].z*pos[0].x);
	torq[1]=(fvect[1].x*pos[1].z)-(fvect[1].z*pos[1].x);	
	torq[2]=(fvect[2].x*pos[2].z)-(fvect[2].z*pos[2].x);
	torq[3]=(fvect[3].x*pos[3].z)-(fvect[3].z*pos[3].x);

	
	//if torqtot is +ve, then it indicates a +ve lefthanded rotation about y 

	float torqtot = torq[0]+torq[1]+torq[2]+torq[3];

	//Find translational component
	//This needs to be done at each wheel
	//Do the vector sum of r^(r^.F)           (r^ indicates unit vector of r)

	vector3 transvect;
	float dotp[4];

	dotp[0]=(npos[0].x*fvect[0].x)+(npos[0].y*fvect[0].y)+(npos[0].z*fvect[0].z);
	dotp[1]=(npos[1].x*fvect[1].x)+(npos[1].y*fvect[1].y)+(npos[1].z*fvect[1].z);
	dotp[2]=(npos[2].x*fvect[2].x)+(npos[2].y*fvect[2].y)+(npos[2].z*fvect[2].z);
	dotp[3]=(npos[3].x*fvect[3].x)+(npos[3].y*fvect[3].y)+(npos[3].z*fvect[3].z);

/*		
	//Show torques on screen
	
	float crsp[4];

	crsp[0]=(npos[0].z*fvect[0].x)-(npos[0].x*fvect[0].z);
	crsp[1]=(npos[1].z*fvect[1].x)-(npos[1].x*fvect[1].z);
	crsp[2]=(npos[2].z*fvect[2].x)-(npos[2].x*fvect[2].z);
	crsp[3]=(npos[3].z*fvect[3].x)-(npos[3].x*fvect[3].z);
	
	rpos[0].Normalise();
	rpos[1].Normalise();
	rpos[2].Normalise();
	rpos[3].Normalise();

	boris2->showmagdir(0,10000.0f,rpos[0]*crsp[0]);
	boris2->showmagdir(1,10000.0f,rpos[1]*crsp[1]);
	boris2->showmagdir(2,10000.0f,rpos[2]*crsp[2]);
	boris2->showmagdir(3,10000.0f,rpos[3]*crsp[3]);
*/	

	//Now do vector sum

	transvect.x=dotp[0]*npos[0].x+dotp[1]*npos[1].x+dotp[2]*npos[2].x+dotp[3]*npos[3].x;
	
	//this component should be zero but probably won't be due to the placement
	//of the centre of gravity relative to the wheels
	transvect.y=0.0f;

	transvect.z=dotp[0]*npos[0].z+dotp[1]*npos[1].z+dotp[2]*npos[2].z+dotp[3]*npos[3].z;

	//Define the moveit vector using a=F/m
	vector3 accel;

	accel.x=transvect.x/carmass;
	accel.y=transvect.y/carmass;
	accel.z=transvect.z/carmass;

	acc = acc * 0.6 + accel * 0.4;
	
	//Change in velocity is dv=a dt
	velocity+=(accel*timeint);

	//Change in position is ds= v dt
	vector3 moveit=(velocity*timeint);

	//Torque gives rise to angular acceleration
	angvel+=timeint*torqtot/mofinertia;
	yRotationAngle+=angvel*timeint;

	//Test car for collision with the world

	vector3 worldvelocity,collisionVector,collisionPoint;
	float collisionTime;

	mainObject->transform.multiplyV3By3x3mat(&worldvelocity,&velocity);

	//worldvelocity*=timeint;
	bool collision;
	collision = willCarCollide(&worldvelocity,&vector3(0,0,0),timeint,
								&collisionVector,&collisionPoint,&collisionTime);
	
	grdebug("collide",collision);
	if (collision)
	{
		grdebug("collide",1);
		if (lastcollided) stuckinwall=true;
		vector3 bodycolvector,bodycolpoint;
		matrix3x4 locinvbodymat;
		locinvbodymat.Transpose3x4(&mainObject->transform);
		locinvbodymat.multiplyV3By3x3mat(&bodycolvector,&collisionVector);	
		
		
		collisionPoint -= mainObject->transform.translation;

		locinvbodymat.multiplyV3By3x3mat(&bodycolpoint,&collisionPoint);

		bodycolvector.Normalise();
		
		if (!stuckinwall)
		{
			float dotp = bodycolvector.x*velocity.x + bodycolvector.z*velocity.z;	
			vector3 refvel = velocity - bodycolvector * 2.0f * dotp;	
			velocity = refvel;

			static int collsnds[] = {SND3D_COLLISION1,SND3D_COLLISION2,SND3D_COLLISION3};

			carsounds.SpotEffect(this,collsnds[rand()%3]);
		}
		else
		{
			velocity+=bodycolvector*1.0f;
		}

		Crumple(&bodycolpoint,&bodycolvector);

		/*
		char myBuffer[1024];
		sprintf(myBuffer,"\nwcolv.x = %f wcolv.y = %f wcolv.z = %f",collisionVector.x,collisionVector.y,collisionVector.z);
		OutputDebugString(myBuffer);
		sprintf(myBuffer,"\ncolv.x = %f colv.y = %f colv.z = %f",bodycolvector.x,bodycolvector.y,bodycolvector.z);
		OutputDebugString(myBuffer);
		sprintf(myBuffer,"\nvel.x = %f vel.y = %f vel.z = %f",velocity.x,velocity.y,velocity.z);
		OutputDebugString(myBuffer);
		sprintf(myBuffer,"\nrvel.x = %f rvel.y = %f rvel.z = %f\n",refvel.x,refvel.y,refvel.z);
		OutputDebugString(myBuffer);
		*/


		velocity.y = 0;

		moveit=(velocity*timeint);
//		yRotationAngle-=angvel*timeint;
//		angvel=0.0f;
		lastcollided=true;
	

	}
	else
	{
		grdebug("collide",0);
		lastcollided=false;
		stuckinwall=false;
	}
	grdebug("velocity",velocity.Mag());
	grdebug("moveit",moveit.Mag());


	//Update car location using a local space vector 'moveit'	

	vector3 wldmove,oldpos;
	
	oldpos=mainObject->transform.translation;
	mainObject->transform.multiplyV3By3x3mat(&wldmove,&moveit);
	mainObject->movePosition(&wldmove);
	mainObject->changed = true;
	mainObject->inheritTransformations();

	//Set car angle to yRotationAngle
	float siny =  sin(yRotationAngle);
	float cosy =  cos(yRotationAngle);

	
	

	yRotationMatrix.set3x3submat(
					cosy, 0.0f, -siny,
					0.0f,  1.0f, 0.0f, 
					siny, 0.0f, cosy
					);


	//rotate about CofG

	siny =  sin(angvel*timeint);
	//if(cosang[0]<0.0)siny=-siny;
	cosy =  cos(angvel*timeint);

	//Rotate the velocity to the new angle by rotating it back through
	//the change in angle (opposite direction to the car)
	matrix3x4 yRotationIncrement;
	yRotationIncrement.set3x3submat(
					cosy, 0.0f, siny,
					0.0f,  1.0f, 0.0f, 
					-siny, 0.0f, cosy
					);
	vector3 nvel;
	yRotationIncrement.multiplyV3By3x3mat(&nvel,&velocity);
	velocity=nvel;


	vector3 newCofG;

	//Have simplified this next block into the three lines following the comment
	/*
	matrix3x4 yRotationIncrement;
	yRotationIncrement.set3x3submat(
					cosy, 0.0f, -siny,
					0.0f,  1.0f, 0.0f, 
					siny, 0.0f, cosy
					);
	//yRotationIncrement.multiplyV3By3x3mat(&newCofG,&CofG);
	*/

	newCofG.x = cosy * CofG.x + siny * CofG.z ;  
	newCofG.y = CofG.y;  
	newCofG.z = -siny * CofG.x + cosy * CofG.z ;  


	//This vector describes the movement needed to make the car
	//look as if it is rotating about the new CofG
	vector3 tmp=(CofG-newCofG);

	vector3 tmp2=tmp;
	mainObject->transform.multiplyV3By3x3mat(&tmp2,&tmp);
	mainObject->movePosition(&tmp2);
	mainObject->changed = true;
	mainObject->inheritTransformations();

	plonkOnGround();
	



	//------------------------------------------------------
	//----Test for car/car collision------------------------
	//------------------------------------------------------

	vector3 mypos=mainObject->transform.translation + moveit;
	vector3 yourpos;
	Sphere *curhull;
	SphereSet *curspheres;
	ulong coll=0;
	ulong mycnt,yourcnt,mybodysph,yourbodysph;
	vector3 colpoint;
	

	Sphere *myspheres[40],*yourspheres[40];


	char myBuffer[1024];
	Sphere *mysphere,*yoursphere;

	//don't bother checking for any collision if another car has already told us
	//that we have collided with it

	if(cararray[0]==this)
	{
//		fprintf(debugfile,"\nCar0");
	}

	if(cararray[1]==this)
	{
//		fprintf(debugfile,"\nCar1");
	}


	if (carcarcollide==0)
	{
		for(ulong i=0;i<GlobalGame.numcars;i++)
		{		
			if(cararray[i]!=this)
			{
				yourpos=cararray[i]->mainObject->transform.translation;
				yourpos-=mypos;
				float distsq=yourpos.x*yourpos.x+yourpos.y*yourpos.y+yourpos.z*yourpos.z;
				if(distsq<49.0f)
				{
					for(ulong j=0;j<spherecount;j++)
					{
						for(ulong k=0;k<cararray[i]->spherecount;k++)
						{
							mysphere=carspheres[j];
							yoursphere=cararray[i]->carspheres[k];
							vector3 sep=yoursphere->v - mysphere->v;
							float distsep=sep.Mag();
							float linksep=distsep;
							distsep-=(mysphere->rad + yoursphere->rad);
							if(distsep<=0.0f)
							{
								distsep*=(-1.0f);
								coll=1;
								sep.Normalise();
								colpoint=mysphere->v + sep * mysphere->rad;
								cararray[i]->carcarhull=k;
								cararray[i]->carcarcollide=1;
								cararray[i]->carcarpoint=colpoint;
								cararray[i]->carcarsink=distsep;
								cararray[i]->carcardir=sep;
								cararray[i]->carcarvel=velocity;
								cararray[i]->carcarmassratio=1.0f/(carmass+cararray[i]->carmass);
								cararray[i]->carcol=this;
								carcarhull=j;
								carcol=cararray[i];
								carcarpoint=colpoint;
								carcardir=sep*(-1.0f);
								carcarvel=cararray[i]->velocity;
								carcarmassratio=cararray[i]->carcarmassratio;
								carcarsink=distsep;
								k=500;
								j=500;
								
							}
						}
					}
				}
				if (coll==0) cararray[i]->carcarcollide=0;
			}
			if(coll==1) i=500;
		}
	}



	//if we have collided with another car but it hasn't been detected by it yet

	if (coll==1 && carcarcollide==0)
	{
		if (lastcarcollided) stuckincar=true;
		vector3 bodycolpoint,bodydirvector,bodycolpointnorm,bodyyourvel,tmp;
		matrix3x4 locinvbodymat,carcollocinvbodymat;

		locinvbodymat.Transpose3x4(&body->bodyObject->transform);
		carcarpoint-=mainObject->transform.translation;
		locinvbodymat.multiplyV3By3x3mat(&bodycolpoint,&carcarpoint);		
		locinvbodymat.multiplyV3By3x3mat(&bodydirvector,&carcardir);
		
		carcol->mainObject->transform.multiplyV3By3x3mat(&tmp,&carcol->velocity);
		locinvbodymat.multiplyV3By3x3mat(&bodyyourvel,&tmp);

		bodycolpointnorm=bodycolpoint;
		bodycolpointnorm.Normalise();
		bodydirvector.Normalise();
		
		//Bodydirvector is the normalised direction of the force
		//Bodycolpoint is the point of application of the force on the body
		//Bodycolpointnorm is the direction to the point of application on the body
		//Bodyyourvel is the velocity vector of the other car in our car's space

		//bodydirvector.x = (bodydirvector.x>0.707f ?1.0f:0.0f) + (bodydirvector.x<-0.707f ?-1.0f:0.0f);
		bodydirvector.y = 0.0f;
		//bodydirvector.z = (bodydirvector.z>0.707f ?1.0f:0.0f) + (bodydirvector.z<-0.707f ?-1.0f:0.0f);

		vector3 veldif=bodyyourvel - velocity;
		vector3 impforce;
		float velscale,tforce,imptorq;
		if (!stuckincar)
		{
			//we didn't collide with car last time round

			//char myBuffer[1024];
			//sprintf(myBuffer,"\nvelscale %f impforce.x %f impforce.z %f",velscale,impforce.x,impforce.z);
			//OutputDebugString(myBuffer);
			
			//accel.x=impforce.x/carmass;
			//accel.y=0.0f;
			//accel.z=impforce.x/carmass;
			
			//angvel=timeint*imptorq/mofinertia;
			//yRotationAngle+=angvel*timeint;
		}
		else
		{
			//we collided with a car last time round
			//vector3 getout;
			//getout=body->bodyObject->transform.translation - carcol->body->bodyObject->transform.translation;
			//getout.x = (getout.x>0.707f ?1.0f:0.0f) + (getout.x<-0.707f ?-1.0f:0.0f);
			//getout.y = 0.0f;
			//getout.z = (getout.z>0.707f ?1.0f:0.0f) + (getout.z<-0.707f ?-1.0f:0.0f);
			/*
			getout*=0.005f;
			body->bodyObject->movePosition(&getout);
			vector3 locdispl;
			body->bodyObject->transform.multiplyV3By3x3mat(&locdispl,&getout);
			velocity-=locdispl*(1.0f/timeint);
			getout*=-1.0f;
			carcol->body->bodyObject->movePosition(&getout);
			carcol->body->bodyObject->transform.multiplyV3By3x3mat(&locdispl,&getout);
			carcol->velocity-=locdispl*(1.0f/timeint);
			grdebug("WAAAAAAAAH",0);
			*/
		}

		
		//Change in velocity is dv=a dt
		//velocity+=(accel*timeint);

		//Change in position is ds= v dt
		//moveit=(velocity*timeint)-moveit;
		//moveit=vector3(0.0f,0.0f,0.0f);
		float disttogo=moveit.Mag();


		vector3 linksep=carspheres[carcarhull]->v-carcol->carspheres[carcol->carcarhull]->v;
		
		if (disttogo<carcarsink)
		{
			moveit*=((carcarsink+0.001f)/disttogo);
		}
		//velocity-=moveit*(1.0f/timeint);
		
		vector3 lineofcol = carspheres[carcarhull]->v - carcol->carspheres[carcol->carcarhull]->v;
		lineofcol.Normalise();
		float velref = lineofcol.x*velocity.x + lineofcol.y*velocity.y +lineofcol.z*velocity.z;

		velocity-=lineofcol*velref*1.5f;
		velocity.y=0;
		wldmove*=(-1.0f);
		moveit*=(-1.0f);
		mainObject->transform.multiplyV3By3x3mat(&wldmove,&moveit);

		mainObject->movePosition(&wldmove);
		angvel=0.0f;
		lastcarcollided=true;
	}	
	else
	{
		//if we have collided with another car and it has been detected by it yet
		//we haven't gone through the collision process - we know where it occured
		if(carcarcollide==99)
		{
//			if (lastcarcollided) stuckincar=true;
			if (!stuckincar)
			{

				//Change in velocity is dv=a dt
//				velocity+=(accel*timeint);

				//Change in position is ds= v dt
				//moveit=(velocity*timeint)-moveit;
				//moveit=vector3(0.0f,0.0f,0.0f);
				float disttogo=moveit.Mag();
				if (disttogo<carcarsink)
				{
					moveit*=((carcarsink+0.1f)/disttogo);
				}
				//velocity-=moveit*(1.0f/timeint);
				vector3 lineofcol = carspheres[carcarhull]->v - carcol->carspheres[carcol->carcarhull]->v;
				lineofcol.Normalise();
				float velref = lineofcol.x*velocity.x + lineofcol.y*velocity.y +lineofcol.z*velocity.z;
				velocity-=lineofcol*velref*2.0f;
				wldmove*=(-1.0f);
				mainObject->movePosition(&wldmove);
				carcarcollide=0;
				lastcarcollided=true;
			}
		}
		else
		{
			lastcarcollided=false;
			stuckincar=false;
		}
	}






	
/*	//a tool for changing gear ratios easily
	static ulong hold=0;
	if (hold>0) hold--;
	grdebug("cgearratio",gearratio[curgear]*1000.0f);
	if((GetKeyState(VK_F7) & 0x08000)&&hold==0)
	{
		hold=10;
		gearratio[curgear]-=0.005;
	}
	if((GetKeyState(VK_F8) & 0x08000)&&hold==0)
	{
		hold=10;
		gearratio[curgear]+=0.005;
	}
*/
	//a tool for measuring 0 to 60 and 0 to 100 times
	static float nto60=0.0f;
	static float nto100=0.0f;
	static float timer=0.0f;
	static bool nt6def=false;
	static bool nt1def=false;
	static bool accelon=false;

	timer+=1.0f/60.0f;
	if (throttle>0.0f && !accelon)
	{
		accelon=true;
		timer=0.0f;
	}

	if(mphspeed<60.0f && !nt6def && accelon) nto60=timer;
	if(mphspeed<100.0f && !nt1def && accelon) nto100=timer;
	if(mphspeed>60.0f) nt6def=true;
	if(mphspeed>100.0f) nt1def=true;

	
	if(GetKeyState(VK_F6) & 0x08000)
	{
		nt6def=nt1def=accelon=false;
		nto60=nto100=timer=0.0f;
	}
	

	grdebug("nto60",nto60);
	grdebug("nto100",nto100);
	

	//Update smoke, explosion and fire sprites
	if (engdead)
	{
		vector3 pos1,pos2,pos3;

		mainObject->transform.multiplyV3By3x4mat(&pos1,&firepos1);
		mainObject->transform.multiplyV3By3x4mat(&pos2,&firepos2);
		mainObject->transform.multiplyV3By3x4mat(&pos3,&firepos3);


		if(! --smokectr)
		{
			boris2->appendSpriteFX( smokeFX, &pos1, NULL, false, false);
			smokectr=(rand()%4)+2;
		}

		if (boris2->doesSpriteFXentryExist(explos))explos->worldPosition = pos1;


		if (boris2->doesSpriteFXentryExist(fire1))
			fire1->worldPosition = pos1;
		else if(!(rand()%17))
			fire1 = boris2->appendSpriteFX( fire1FX, &pos1, NULL, false, false);


		if (boris2->doesSpriteFXentryExist(fire2))
			fire2->worldPosition = pos2;
		else if(!(rand()%17))
			fire2 = boris2->appendSpriteFX( fire2FX, &pos2, NULL, false, false);


		if (boris2->doesSpriteFXentryExist(fire3))
			fire3->worldPosition = pos3;
		else if(!(rand()%17))
			fire3 = boris2->appendSpriteFX( fire3FX, &pos3, NULL, false, false);

	}

}



void car::showxzvector(int n, vector3 vect)
{
	float vmag=vect.Mag();
	float ang;
	if (vmag<0.01f)
	{
		ang=0.0f;
	}
	else
	{
		ang=acos(vect.z/vmag);
		if( vect.x < 0 ) ang = -ang;
	}
	boris2->wheelDirection(n,ang);

}


float car::getEngineForce(float throttle,float brake, float wspeed2,float wspeed3)
{
	
	float timeint = 1.0f/TICKRATE;
	float curspeed = (wspeed2+wspeed3)*0.5f;

	float engforce=0.0f;
	
	
	float desiredrevs;
	float revdamping = 0.1f;
	float skidrevdamping = 0.4f;
	float brakerevdamping = 0.1f;
	//Resistive forces for stopping on hills with brakes
	//Putting it here because it's most convenient to express it
	//through the engine forces
	//It won't make any difference to driving characteristics as
	//it only happens at low speed

	mph = curspeed*2.25;

	if (curspeed<10.0f && curspeed>-10.0f && brake>0.0f)
	{
		engforce=-0.1f*carmass*curspeed/timeint;
		if (engforce<5000.0f) engforce*=5.0f;
		//+ve restoring force
		engforce*=brake*brake*brake;


		desiredrevs=idle;
		revs+=brakerevdamping*(desiredrevs-revs);		
		return engforce;
	}

	if (curspeed>0.0f && curspeed<10.0f && brake>0.0f)
	{
		engforce=-0.1f*carmass*curspeed/timeint;
		if (engforce>-5000.0f) engforce*=5.0f;
		//-ve restoring force
		engforce*=brake*brake*brake;
		
		desiredrevs=idle;
		revs+=brakerevdamping*(desiredrevs-revs);		

		return engforce;
	}

	if (engdead)
	{
		engforce=-0.04*carmass*curspeed/timeint;
		return engforce;
	}
	

	if(revs>engblowuprevs)
	{
		if(!engdead)
		{
			carsounds.SpotEffect(this,SND3D_EXPLODE);
			engdead=true;
		}
		vector3 pos1,pos2,pos3;

		mainObject->transform.multiplyV3By3x4mat(&pos1,&firepos1);
		mainObject->transform.multiplyV3By3x4mat(&pos2,&firepos2);
		mainObject->transform.multiplyV3By3x4mat(&pos3,&firepos3);

		explos = boris2->appendSpriteFX( explosFX, &pos1, NULL, false, false);
	}

	
	//Engine braking
	if (throttle<0.01f)
	{
		engforce=-0.008f*carmass*curspeed/timeint;
		if (curgear!=1)
		{
			engforce*=gearratio[curgear]*gearratio[curgear];
		}
		else
		{
			engforce*=0.01f;
		}
		desiredrevs=revscale*gearratio[curgear]*curspeed+idle;
		if (curgear==1) desiredrevs=idle;
		revs+=brakerevdamping*(desiredrevs-revs);		
		return engforce;
	}


	//Simulate clutch depression
	if ((curspeed<0.0f && curgear>1)||(curspeed>0.0f && curgear==0))
	{
		float nonlin=(1-throttle)*(1-throttle);
		nonlin*=nonlin;
		nonlin=1-nonlin;

		//Only allow clutch to accept up to 40% of max revs of engine
		desiredrevs=throttle*0.4*maxrevs;
		desiredrevs=(desiredrevs>idle)?desiredrevs:idle;
		if(brake>0.0f)desiredrevs=idle;

		revs+=skidrevdamping*(desiredrevs-revs);		
		engforce=throttle*gearForce(revs);
		if (curgear==0) engforce= -engforce;
	}

	if(wskid[2]<-0.5f && wskid[3]<-0.5f && curgear!=1)
	{
		//Calculate revs for current velocity
		desiredrevs=revscale*gearratio[curgear]*curspeed+idle;

		revs+=revdamping*(desiredrevs-revs);
		//Calculate force developed at current revs
		engforce=throttle*gearForce(revs);
	}
	else
	{
		//Make revs proportional to 1-(1-throttle)^4 as we are skidding
		float nonlin=(1-throttle)*(1-throttle);
		nonlin*=nonlin;
		nonlin=1-nonlin;

		desiredrevs=throttle*maxrevs;
		desiredrevs=(desiredrevs>idle)?desiredrevs:idle;
		if(brake>0.0f)desiredrevs=idle;

		revs+=skidrevdamping*(desiredrevs-revs);		

		//Make sure that we are using the smallest speed
		float altspeed=velocity.Mag();
		if (altspeed>curspeed)curspeed=altspeed;

		//Make sure engine force is zero at maximum speed for current gear
		float falloff=(termvel[curgear]-curspeed)/termvel[curgear];
		if (curgear!=1)engforce=gearForce(revs)*falloff;
	
	}
	
	
	if (engdead) revs = 0.0f;

	grdebug("force",engforce);
	grdebug("gear",curgear);
	grdebug("revs",revs);

	return engforce;
}

float car::gearForce(float revs)
{
	float force;

	//Preset default condition
	force=fmax[curgear];

	if (revs<lowerthresh[curgear])
		//If revs are lower than the point we get to Fmax then use slope 1
		force=grad1[curgear]*revs+intcpt1[curgear];
		if (force<0.0f) force=0.0f;
	else if (revs>upperthresh[curgear])
		//If revs are higher than the point after Fmax then use slope 2
		force=grad2[curgear]*revs+intcpt2[curgear];
	
	//Downsize the force to consider gear ratios.
	force*=gearratio[curgear];

	return force;
}

float car::skidthresh(float cspeed,bool front, bool wantskid)
{
	ulong index=0;
	if(!front) index=2;
	if(!wantskid) index+=1;

	if (cspeed<speed[1])
	{
		cspeed-=speed[0];
		return (skidlookup[index][0]+cspeed*skidlookup[index+4][0]);
	}

	if (cspeed<speed[2])
	{
		cspeed-=speed[1];
		return (skidlookup[index][1]+cspeed*skidlookup[index+4][1]);
	}
		
	if (cspeed<speed[3])
	{
		cspeed-=speed[2];
		return (skidlookup[index][2]+cspeed*skidlookup[index+4][2]);
	}

	if (cspeed<speed[4])
	{
		cspeed-=speed[3];
		return (skidlookup[index][3]+cspeed*skidlookup[index+4][3]);
	}

	if (cspeed<speed[5])
	{
		cspeed-=speed[4];
		return (skidlookup[index][4]+cspeed*skidlookup[index+4][4]);
	}
	
	return skidlookup[index][5];
}	

float car::dynamicfriction(float cspeed,bool front, bool upper)
{
	ulong index=0;
	if(!front) index=1;
	if(!upper) index+=1;

	if (cspeed<speed[1])
	{
		cspeed-=speed[0];
		return (dynfric[index][0]+cspeed*dynfric[index+3][0]);
	}

	if (cspeed<speed[2])
	{
		cspeed-=speed[1];
		return (dynfric[index][1]+cspeed*dynfric[index+3][1]);
	}
		
	if (cspeed<speed[3])
	{
		cspeed-=speed[2];
		return (dynfric[index][2]+cspeed*dynfric[index+3][2]);
	}

	if (cspeed<speed[4])
	{
		cspeed-=speed[3];
		return (dynfric[index][3]+cspeed*dynfric[index+3][3]);
	}

	if (cspeed<speed[5])
	{
		cspeed-=speed[4];
		return (dynfric[index][4]+cspeed*dynfric[index+3][4]);
	}
	
	return dynfric[index][5];
}	


void car::changeGear( ulong gearNumber )
{
	curgear = gearNumber;
};

void car::shiftUpGear()
{
	if (curgear<numgears) curgear++;
};

void car::shiftDownGear()
{
	if (curgear>0) curgear--;
};


//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::setBodyPosition_Y( float p )
{
	mainObject->setPosition_Y( p );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::setWheelPosition_Y( ulong wheelNumber, float p)
{
	wheel[wheelNumber]->wheelObject->setPosition_Y( p );
};


void car::updateExhaust()
{
	ulong eticks = exhaustSpriteTimer.getTicks();
	if( eticks )
	{
		if( exhaustUsedSequences < exhaustMaxSequences )
		{


		mainObject->concatenatedTransform.multiplyV3By3x4mat( &body->transformedexhaustCloudPoint, &body->exhaustCloudPoint);


		boris2->appendSpriteFX( exhaustSpriteFX, 
								&body->transformedexhaustCloudPoint,
								&exhaustUsedSequences,false,false);

		};
		
	};
};


//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

car::~car()
{
	slong i;
	

	
	delete body;
	for(i = 0; i<4; i++)
		delete wheel[i];

	//body->~bodyObject();			// Remove the geometry of the car 
}



//--------------------------------------------------------------------------------------------------------------------------------

void car::setBodyPosition( vector3 * p )
{
	mainObject->setPosition( p );


}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::moveBodyPosition( vector3 * p )
{
	mainObject->movePosition( p );

	//body->transformedexhaustCloudPoint = body->bodyObject->transform.translation;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------


void car::moveBodyPositionInDirection( vector3 * p )
{
	static vector3 tempvector;

	
	tempvector.negate( &mainObject->transform.translation );

	mainObject->transform.multiplyIntoTransform(  p );

	if( (mainObject->attached != 0) && ((mainObject->attachedValue & ATTACH_FOLLOW)!=0) )
	{
		tempvector += mainObject->transform.translation;
	
		mainObject->attached->moveCameraPosition( &tempvector);
		
	};

	mainObject->changed = true;


}


void car::setWheelPosition( ulong wheelNumber, vector3 * p )
{
	wheel[wheelNumber]->wheelObject->setPosition( p );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::moveWheelPosition( ulong wheelNumber, vector3 * p )
{
	wheel[wheelNumber]->wheelObject->movePosition( p );
}





void car::setBodyAngle( vector3 * a )
{
	mainObject->setAngle( a );
	yRotationAngle = a->y;
}	

void car::setBodyAngleNOCAMERA( vector3 * a )
{
	mainObject->setAngleNOCAMERA( a );
	yRotationAngle = a->y;
}

void car::setWheelAngle( ulong wheelNumber, vector3 * a )
{
	wheel[wheelNumber]->wheelObject->setAngle( a );

}

void car::getWheelAngle( ulong wheelNumber, vector3 * a )
{
	wheel[wheelNumber]->wheelObject->getAngle( a );
}

void car::moveBodyAngle( vector3 * a )
{
	mainObject->moveAngle( a );
	yRotationAngle += a->y;
}

void car::moveBodyAngleFromLocalPosition( vector3 * a , vector3 * p)
{
	mainObject->moveAngleFromLocalPosition( a, p );
}

void car::moveBodyAngleNOCAMERA( vector3 * a )
{
	mainObject->moveAngleNOCAMERA( a );
}

void car::moveWheelAngle( ulong wheelNumber, vector3 * a )
{
	wheel[wheelNumber]->wheelObject->moveAngle( a );
}



void car::detachCamera( camera * c)
{
	c->detach();	
};

void car::detachCamera()
{
	if(boris2->currentcamera)
		boris2->currentcamera->detach();	
};

void car::attachCamera( camera * c, uchar aflags, vector3 * attachPoint  )
{
	c->attach( mainObject,  aflags, attachPoint);
}

void car::attachCamera( uchar aflags, vector3 * attachPoint  ) // uses current camera
{
	if(boris2->currentcamera)
		boris2->currentcamera->attach( mainObject,  aflags, attachPoint);
}

void car::insertIntoWorld()
{
	boris2->insertObject( mainObject );		
}

void car::removeFromWorld()
{
	boris2->removeObject( mainObject );
}


void car::CalculatePhysicalProperties()
{
	matrix3x4 * orientation = &mainObject->transform;

	extrarotvel=extraforce=totaltorque=vector3(0,0,0);

	mass = 700;// TEST TEMP VALUE
	oneOverMass = RECIPROCAL( mass );

	acc = vector3(0,0,0);

	// initial orientation matrix

	// find the body-relative moment of inertia tensor

	// these are 1/2 the dimensions of the box

	float dx2 = 0.75;
	float dy2 = 0.5;
	float dz2 = 1;

	inverse_inertia_tensor_body.setmat(
		3.0f/(mass*(dy2*dy2+dz2*dz2)),		0,		0,		
		0,		3.0f/(mass*(dx2*dx2+dz2*dz2)),		0,		
		0,		0,		3.0f/(mass*(dx2*dx2+dy2*dy2)),		
		0,		0,		0
	);

	angular_momentum = vector3(0,0,0);


	static matrix3x4 tempmatrix;

	// get the new inverse inertia tensor. ugh... bloody classes! This can be optimised to hell and back.

	//inverse_inertia_tensor_world = orientation;
	//inverse_inertia_tensor_world.Transpose( );
	  inverse_inertia_tensor_world.Transpose3x4( orientation );

	//inverse_inertia_tensor_world = inverse_inertia_tensor_body * inverse_inertia_tensor_world;
	//inverse_inertia_tensor_world = orientation * inverse_inertia_tensor_world;

  	tempmatrix.multiply3x3submats( &inverse_inertia_tensor_body, &inverse_inertia_tensor_world );
	inverse_inertia_tensor_world.multiply3x3submats( orientation, &tempmatrix );


	// and get the new angular acceleration

	// angular_velocity = inverse_inertia_tensor_world * angular_momentum;

	inverse_inertia_tensor_world.multiplyV3By3x3mat( &angular_velocity, &angular_momentum );

	//angular_velocity = inverse_inertia_tensor_world * angular_momentum;
};

long collObjCode;

bool car::isInCollision()
{
  bool collided;
  object * obj;


  collided = 	  false;

 
	 obj = wheel[0]->wheelObject;

	 collided = false;

	 if( obj->transformedSpheres )
		collided = obj->transformedSpheres->isCollidingWithWorld( &obj->transformedCentroid, 1130.0f );
	 if( collided )
	 {
		collObjCode = 0;
	 }
	 else
	 {
		obj = wheel[1]->wheelObject;
		if( obj->transformedSpheres )
			collided = obj->transformedSpheres->isCollidingWithWorld( &obj->transformedCentroid, 1130.0f );
		if( collided )
		{
		   collObjCode = 1;
		}
		else
		{
		   obj = wheel[2]->wheelObject;
   		   if( obj->transformedSpheres )
				collided = obj->transformedSpheres->isCollidingWithWorld( &obj->transformedCentroid, 1130.0f );
		   if( collided )
		   {
			   collObjCode = 2;
		   }
		   else
		   {
				obj = wheel[3]->wheelObject;
   				if( obj->transformedSpheres )
					collided = obj->transformedSpheres->isCollidingWithWorld( &obj->transformedCentroid, 1130.0f ) ;
				if( collided )
				{
					collObjCode = 3;
				}
				else
				{
					obj = body->bodyObject;
	   				if( obj->transformedSpheres )
						collided = obj->transformedSpheres->isCollidingWithWorld( &obj->transformedCentroid, 1130.0f );
					if( collided )
					{
						collObjCode = 4;
					}
				}
		   }
		}
	 }
  
 
  return(collided);
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

bool car::isCollidingAtTime( float tdelta )
{
	
  object * obj;
  bool collided;
  static vector3 distanceToCollision;

  return false;

};


//--------------------------------------------------------------------------------------------------------------------------------
// findTimeOfImpact
// ----------------
//				find the first point, within the next tick, at which the car collides by using a binary chop to move it.
//		return 'true' iff a collision occured, otherwise return false;
//--------------------------------------------------------------------------------------------------------------------------------


#define MINIMUMTOI 0.1f
bool car::findTimeOfImpact( float * toi, float timetoconsider )
{
		ulong chopcount;
			
		float hi = timetoconsider;			// maximum time in seconds
		float lo = 0.0f;

		float mid = (lo + hi ) * 0.5f;
		float oldmid;



		bool collisionOccured = false;
		do{
		 
   		   if( isCollidingAtTime(mid) )
		   {
				hi = mid;
				collisionOccured = true;
		   } else
				lo = mid;

		   oldmid = mid;
		   mid = (lo + hi ) * 0.5f;

		  
		} while( ( mid != oldmid ) && ( (hi-lo) > MINIMUMTOI ) );

	
		*toi = lo;						
		
		return( collisionOccured );
};



	//
	// NEW STUFF
	//



void car::setviewIncar()
{
	// mark the inviewModel ONLY as visible for rendering
	bodyObject->visible = false;
	driverObject->visible = false;
	FwheelObjectLEFT->visible = false;
	RwheelObjectLEFT->visible = false;
	FwheelObjectRIGHT->visible = false;
	RwheelObjectRIGHT->visible = false;
	FaxleObject->visible = false;
	RaxleObject->visible = false;

	if(	bodyShadowObject )
			bodyShadowObject->visible = false;

	if(	RwheelShadowObjectLEFT )
			RwheelShadowObjectLEFT->visible = false;
	if(	RwheelShadowObjectRIGHT )
			RwheelShadowObjectRIGHT->visible = false;

	if(	FwheelShadowObjectLEFT )
			FwheelShadowObjectLEFT->visible = false;
	if(	FwheelShadowObjectRIGHT )
			FwheelShadowObjectRIGHT->visible = false;

	if( FaxleShadowObject )
		FaxleShadowObject->visible = false;
	if( RaxleShadowObject )
		RaxleShadowObject->visible = false;
};

void car::setviewOutcar()
{ 
	bodyObject->visible = true;
	driverObject->visible = true;
	FwheelObjectLEFT->visible = true;
	RwheelObjectLEFT->visible = true;
	FwheelObjectRIGHT->visible = true;
	RwheelObjectRIGHT->visible = true;
	FaxleObject->visible = true;
	RaxleObject->visible = true;

	if(	bodyShadowObject )
			bodyShadowObject->visible = true;

	if(	RwheelShadowObjectLEFT )
			RwheelShadowObjectLEFT->visible = true;
	if(	RwheelShadowObjectRIGHT )
			RwheelShadowObjectRIGHT->visible = true;


	if(	FwheelShadowObjectLEFT )
			FwheelShadowObjectLEFT->visible = true;
	if(	FwheelShadowObjectRIGHT )
			FwheelShadowObjectRIGHT->visible = true;

	if( FaxleShadowObject )
		FaxleShadowObject->visible = true;
	if( RaxleShadowObject )
		RaxleShadowObject->visible = true;
};



char tempBuffer2[128];	
char tempBuffer3[128];

	char  FwheelSSname[64];
	char  RwheelSSname[64];
	char  bodySSname[64];
	char  FaxleSSname[64];
	char  RaxleSSname[64];
	char  driverSSname[64];

void car::readOff( char * outStr, char ** strPtr, char stopChar )
{
	char k;
	char * sPtr = *strPtr;


	while( (*sPtr == 32 ) || (*sPtr == 9) )
		sPtr++;


	k = *sPtr++;
	while( (k != stopChar ) && ( k != 0) && ( k != 10) && (k != 13) )
	{
		if( (k!=32) && (k!=9) )
			*outStr++ = k;

		k = *sPtr++;
	};
	*outStr = 0;		// terminate
	*strPtr = sPtr;
};

void car::readOffVector( vector3 * v, char ** strPtr )
{
	readOff( tempBuffer3, strPtr, ',');
	v->x = atof(tempBuffer3);
	readOff( tempBuffer3, strPtr, ',');
	v->y = atof(tempBuffer3);
	readOff( tempBuffer3, strPtr, 0);
	v->z = atof(tempBuffer3);
};


car::car( char * arcname, float carscale, spriteFX * exhaustFX,int _id)
{
	float radiusF, radiusB, oneoverradiusF, oneoverradiusB;

	float maxSpeed;

#define GENERIC		0
#define AUTOUNION	1
#define MERCEDES	2

	ulong cartype = AUTOUNION;

	id = _id;
	smokectr=1;

	yRotationAngle = 0.0f;
	yRotationMatrix.set3x3submat( 1,0,0,  0,1,0,  0,0,1);

	zoomdone = true;
	collision = false;

	carRevsCalculationTimer.setTimer(REVCALCTICKS);

	collisionSoundTimer.setTimer(2);
	skidSoundTimer.setTimer(3);

	SpinCam = 0.0f;
	SpinDelta = 0.0f;
	turnY = 0;

	//Andy's Stuff
	//explosFX = new spriteFX("explos");
	//fireFX = new spriteFX("fire");

	explos = NULL;
	fire1 = NULL;
	fire2 = NULL;
	fire3 = NULL;

	carcarcollide=0;
	carcarvel=vector3(0.0f,0.0f,0.0f);
	carcardir=vector3(0.0f,0.0f,0.0f);
	carcarpoint=vector3(0.0f,0.0f,0.0f);
	carcarforce=vector3(0.0f,0.0f,0.0f);
	carcarmassratio=1.0f;
	carcol=NULL;
	velocity = vector3 (0.0f,0.0f,0.0f);
	angvel = 0.0;
	curgear=1;
	revs=0.0f;
	wvel[0]=0.0f;
	wvel[1]=0.0f;
	wvel[2]=0.0f;
	wvel[3]=0.0f;
	wheelv[0]=vector3(0.0f,0.0f,0.0f);
	wheelv[1]=vector3(0.0f,0.0f,0.0f);
	wheelv[2]=vector3(0.0f,0.0f,0.0f);
	wheelv[3]=vector3(0.0f,0.0f,0.0f);
	wskid[0]=-1.0f; //reverse
	wskid[1]=-1.0f; //neutral
	wskid[2]=-1.0f;
	wskid[3]=-1.0f;
	gravpoly[0]=NULL;
	gravpoly[1]=NULL;
	gravpoly[2]=NULL;
	gravpoly[3]=NULL;
	downforce=1.0f;
	engdamage=0.0;
	engdead=false;

	// horrible hack to position the fire differently on the two different cars
	// this must be replaced with entries in the description, but I don't want
	// too much about with that too much at the moment.

	dprintf("Car archive %s",arcname);

	
	arcPush(arcname);			// car
	
	ulong texhandle = arcExtract_txt( "description" );
	
	
	if(!strchr(arcname,'m'))
	{
		dprintf("that's a merc");
		firepos1 = vector3 (0.02f,0.0f,1.5f);
		firepos2 = vector3 (0.0f,0.4f,1.7f);
		firepos3 = vector3 (-0.02f,0.0f,1.0f);
	}
	else
	{
		dprintf("that's an audi");
		firepos1 = vector3 (0.06f,0.2f,1.5f);
		firepos2 = vector3 (0.0f,0.3f,1.7f);
		firepos3 = vector3 (-0.06f,0.1f,1.0f);
	}

#define GETVECTOR(x,y) if (!strcmp(tempBuffer3, #y )) { readOffVector( & ## x , &strPtr); x *= carscale; }
#define GETSINGLE(x,y) if (!strcmp(tempBuffer3, #y )) { readOff( tempBuffer3 , &strPtr, ','); x = (float)atof(tempBuffer3);}

#define GETSEVEN(x,y) \
	GETSINGLE(x ## [0],y ## 0) ; GETSINGLE(x ## [1],y ## 1) ; GETSINGLE(x ## [2],y ## 2) ; GETSINGLE(x ## [3],y ## 3) ; \
	GETSINGLE(x ## [4],y ## 4) ; GETSINGLE(x ## [5],y ## 5) ; GETSINGLE(x ## [6],y ## 6) ;

#define GETSIX(x,y) \
	GETSINGLE(x ## [0],y ## 0) ; GETSINGLE(x ## [1],y ## 1) ; GETSINGLE(x ## [2],y ## 2) \
	GETSINGLE(x ## [3],y ## 3) ; GETSINGLE(x ## [4],y ## 4) ; GETSINGLE(x ## [5],y ## 5) ;

#define GETTWOBYSEVEN(x,y) \
	GETSEVEN(x ## 1,y ## 1) ; GETSEVEN(x ## 2,y ## 2);

#define GETTHREEBYSEVEN(x,y) \
	GETSEVEN(x ## [0],y ## 0) ; GETSEVEN(x ## [1].y ## 1) ; GETSINGLE(x ## [2],y ## 2) ;

#define GETFOURBYSIX(x,y) \
	GETSIX(x ## [0],y ## 0) ; GETSIX(x ## [1],y ## 1) ; GETSIX(x ## [2],y ## 2) ; GETSIX(x ## [3],y ## 3) ; 

#define GETTHREEBYSIX(x,y) \
	GETSIX(x ## [0],y ## 0) ; GETSIX(x ## [1],y ## 1) ; GETSIX(x ## [2],y ## 2) ;

	if( texhandle ) 
	{
		
		while(  arcGets( tempBuffer2, 128, texhandle ) )
		{
			
			char * strPtr = tempBuffer2;
			
			readOff( tempBuffer3, &strPtr, ':' );

			GETVECTOR(CofG, COFG);

			GETSINGLE(carscale, SCALE);
			GETSINGLE(carmass, CARMASS);
			GETSINGLE(mofinertia, MOFINERTIA);
			GETSINGLE(brk,BRK);

			GETSINGLE(idle, IDLE);
			GETSINGLE(revscale, REVSCALE);
			GETSINGLE(maxrevs, MAXREVS);
			GETSINGLE(engblowuprevs, ENGBLOWUPREVS);
			GETSINGLE(engdamagethresh, ENGDAMAGETHRESH);
			GETSINGLE(numgears, NUMGEARS);

			GETSIX(speed,SPEED);

			GETSEVEN(fmax, FMAX);
			GETSEVEN(gearratio,GEARRATIO);
			GETSEVEN(termvel, TERMVEL);

			GETTWOBYSEVEN(intcpt,INTCPT);
			GETTWOBYSEVEN(grad,GRAD);

			GETTHREEBYSIX(dynfric, DYNFRIC);

			GETFOURBYSIX(skidlookup,SKIDLOOKUP);
		}
	}

	//----------------------------------------------------------------------
	//------END OF LOADED DATA----------------------------------------------
	//----------------------------------------------------------------------

	revscaleinv=1.0f/revscale;
	
	arcPop();

	arcPush(arcname);			// car
	texhandle = arcExtract_txt( "description" );
	
	//----------------------------------------------------------------------
	//------CAR CALCULATIONS ON LOADED DATA---------------------------------
	//----------------------------------------------------------------------
	
	gearratioinv[0]=1.0f/gearratio[0]; //reverse
	gearratioinv[1]=0.0f; //neutral
	gearratioinv[2]=1.0f/gearratio[2];
	gearratioinv[3]=1.0f/gearratio[3];
	gearratioinv[4]=1.0f/gearratio[4];
	gearratioinv[5]=1.0f/gearratio[5];
	gearratioinv[6]=1.0f/gearratio[6];
	
	for(ulong i=0;i<4;i++)
	{
		skidlookup[i][0]*=carmass;
		skidlookup[i][1]*=carmass;
		skidlookup[i][2]*=carmass;
		skidlookup[i][3]*=carmass;
		skidlookup[i][4]*=carmass;
		skidlookup[i][5]*=carmass;
	}
	
	for(i=0;i<4;i++)
	{
		skidlookup[4+i][0]=(skidlookup[0+i][1]-skidlookup[0+i][0])/(speed[1]-speed[0]);
		skidlookup[4+i][1]=(skidlookup[0+i][2]-skidlookup[0+i][1])/(speed[2]-speed[1]);
		skidlookup[4+i][2]=(skidlookup[0+i][3]-skidlookup[0+i][2])/(speed[3]-speed[2]);
		skidlookup[4+i][3]=(skidlookup[0+i][4]-skidlookup[0+i][3])/(speed[4]-speed[3]);
		skidlookup[4+i][4]=(skidlookup[0+i][5]-skidlookup[0+i][4])/(speed[5]-speed[4]);
	}
	
	for(i=0;i<3;i++)
	{
		dynfric[3+i][0]=(dynfric[0+i][1]-dynfric[0+i][0])/(speed[1]-speed[0]);
		dynfric[3+i][1]=(dynfric[0+i][2]-dynfric[0+i][1])/(speed[2]-speed[1]);
		dynfric[3+i][2]=(dynfric[0+i][3]-dynfric[0+i][2])/(speed[3]-speed[2]);
		dynfric[3+i][3]=(dynfric[0+i][4]-dynfric[0+i][3])/(speed[4]-speed[3]);
		dynfric[3+i][4]=(dynfric[0+i][5]-dynfric[0+i][4])/(speed[5]-speed[4]);
	}

	for(i=0;i<7;i++)
	{
		lowerthresh[i]=(fmax[i]-intcpt1[i])/grad1[i];
		upperthresh[i]=(fmax[i]-intcpt2[i])/grad2[i];
	}
	

//----------------------------------------------------------------------
//------END OF ANDYS STUFF----------------------------------------------
//----------------------------------------------------------------------

	kick = kickDelta = 0;




			//
			// Load in the 'decription.txt' file from the archive
			//

	model * FwheelModel, * RwheelModel, * FaxleModel, * RaxleModel, * bodyModel, * driverModel;
	model * FwheelShadowModel, * RwheelShadowModel, * FaxleShadowModel, * RaxleShadowModel, * bodyShadowModel;
	model * driverShadowModel;


	//object * bodyObject, *driverObject, *FwheelObject, *RwheelObject, *FaxleObject, *RaxleObject;


	FwheelModel = RwheelModel = FaxleModel = RaxleModel = bodyModel = driverModel = 
	FwheelShadowModel = RwheelShadowModel = FaxleShadowModel = RaxleShadowModel = bodyShadowModel = 
	driverShadowModel = 0;


	bool flagdata;

	bool  FwheelSSnameUsed, RwheelSSnameUsed, driverSSnameUsed,
		  bodySSnameUsed, FaxleSSnameUsed, RaxleSSnameUsed;

	 FwheelSSnameUsed = RwheelSSnameUsed = driverSSnameUsed = 
		  bodySSnameUsed = FaxleSSnameUsed = RaxleSSnameUsed  = false;



	vector3 FwheelPosition, RwheelPosition,  driverPosition;

	bodyShadowObject = driverShadowObject = FwheelShadowObjectLEFT = FwheelShadowObjectRIGHT =
	RwheelShadowObjectLEFT = RwheelShadowObjectRIGHT = FaxleShadowObject = RaxleShadowObject = 0;

#define SHADOWSCALE 1.1f

	if( texhandle ) 
	{

		while(  arcGets( tempBuffer2, 128, texhandle ) )
		{

			char * strPtr = tempBuffer2;

			readOff( tempBuffer3, &strPtr, ':' );

			if( strcmp( tempBuffer3,"FRONT_WHEEL" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FwheelModel  = getModelPtr( tempBuffer3, carscale, false, "");
				readOffVector( &FwheelPosition, &strPtr );
				FwheelPosition *= carscale;

			} else
			if( strcmp( tempBuffer3,"REAR_WHEEL" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RwheelModel  = getModelPtr( tempBuffer3, carscale, false, "");
				readOffVector( &RwheelPosition, &strPtr );
				RwheelPosition *= carscale;

			} else
			if( strcmp( tempBuffer3,"BODY" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				// now loads as a unique
				bodyModel  = getModelPtr( tempBuffer3, carscale, true, "");
			} else
			if( strcmp( tempBuffer3,"FRONT_AXLE" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				FaxleModel  = getModelPtr( tempBuffer3, carscale, false, "");	
			} else
			if( strcmp( tempBuffer3,"REAR_AXLE" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				RaxleModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else
			if( strcmp( tempBuffer3,"DRIVER" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				driverModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else
			if( strcmp( tempBuffer3,"FRONT_WHEEL_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FwheelShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
				RwheelShadowModel = FwheelShadowModel;
//DBGR.Dprintf("FW SHADOW\n");
			} else
			if( strcmp( tempBuffer3,"REAR_WHEEL_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				//RwheelShadowModel  = .getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
//DBGR.Dprintf("RW SHADOW\n");

			} else
			if( strcmp( tempBuffer3,"BODY_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				bodyShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
			} else		
			if( strcmp( tempBuffer3,"FRONT_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FaxleShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
			} else		
			if( strcmp( tempBuffer3,"REAR_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RaxleShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
			} else		
			if( strcmp( tempBuffer3,"DRIVER_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				//driverShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, "");
			} else
			if( strcmp( tempBuffer3,"FRONT_WHEEL_CHULL" ) == 0)
			{
				readOff( FwheelSSname, &strPtr, 0 );
				FwheelSSnameUsed = true;
			} else					
			if( strcmp( tempBuffer3,"REAR_WHEEL_CHULL" ) == 0)
			{
				readOff( RwheelSSname, &strPtr, 0 );
				RwheelSSnameUsed = true;
			} else	
			if( strcmp( tempBuffer3,"BODY_CHULL" ) == 0)
			{
				readOff( bodySSname, &strPtr, 0 );
				bodySSnameUsed = true;
			} else	
			if( strcmp( tempBuffer3,"FRONT_AXLE_CHULL" ) == 0)
			{
				readOff( FaxleSSname, &strPtr, 0 );
//				FaxleSSnameUsed = true;
			} else	
			if( strcmp( tempBuffer3,"REAR_AXLE_CHULL" ) == 0)
			{
				readOff( RaxleSSname, &strPtr, 0 );
//				RaxleSSnameUsed = true;
			} else	
			if( strcmp( tempBuffer3,"DRIVER_CHULL" ) == 0)
			{
				readOff( driverSSname, &strPtr, 0 );
//				driverSSnameUsed = true;
			} else	
			if( strcmp( tempBuffer3,"SCALE" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"GEAR_TOTAL" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"GEAR" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"GEAR_REVERSE" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"FRONT_WHEEL_MASS" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"REAR_WHEEL_MASS" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"FRONT_AXLE_MASS" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"REAR_AXLE_MASS" ) == 0)
			{

			} else
			if( strcmp( tempBuffer3,"BODY_MASS" ) == 0)
			{

			};
			
		};
// TEMP...
maxSpeed = 10.0f;


				//
				// we MUST have all the models for the car present
				//

		if( (FwheelModel==0) || (RwheelModel==0) || (FaxleModel==0) || (RaxleModel==0) || 
			(driverModel==0) || (bodyModel==0) )
			fatalError("INCOMPLETE CAR");


				//
				// Attach collision hulls when present
				//

		FwheelModel->meshData->spheres =
		RwheelModel->meshData->spheres =
		FaxleModel->meshData->spheres = 
		RaxleModel->meshData->spheres = 
		driverModel->meshData->spheres =
		bodyModel->meshData->spheres = 0;

		if(  FwheelSSnameUsed )
			FwheelModel->meshData->spheres = new SphereSet(FwheelSSname, carscale);
		if(  RwheelSSnameUsed )
			RwheelModel->meshData->spheres = new SphereSet(RwheelSSname, carscale);
		if(  FaxleSSnameUsed )
			FaxleModel->meshData->spheres = new SphereSet(FaxleSSname, carscale);
		if( RaxleSSnameUsed  )
			RaxleModel->meshData->spheres = new SphereSet(RaxleSSname, carscale);
		if( driverSSnameUsed )
			driverModel->meshData->spheres = new SphereSet(driverSSname, carscale);
		if(  bodySSnameUsed  )
			bodyModel->meshData->spheres = new SphereSet(bodySSname, carscale);

				//
				// Build the car from the loaded components
				//

/*
		bodyObject = new object( bodyModel,  COLLIDE_COLLIDABLE | COLLIDE_OBJECT, false );
		bodyObject->setAngle( &vector3(0, 0, 0 ) );
		bodyObject->setPosition( &vector3(0,0,0) );
*/


		mainObject = new object( NULL,  0, false );
		mainObject->setAngle( &vector3(0, 0, 0 ) );
		mainObject->setPosition( &vector3(0,0,0) );

		bodyObject = new object( bodyModel,  COLLIDE_COLLIDABLE | COLLIDE_OBJECT, false );
		bodyObject->setAngle( &vector3(0, 0, 0 ) );
		bodyObject->setPosition( &vector3(0,0,0) );


  	    body   = new carbody( bodyObject );		// create a new body with these values...

		FaxleObject = new object( FaxleModel,  0, false  );
		FaxleObject->setAngle( &vector3(0, 0, 0 ) );
		FaxleObject->setPosition( &vector3(0, 0, 0 ) );

		RaxleObject = new object( RaxleModel,  0, false  );
		RaxleObject->setAngle( &vector3(0, 0, 0 ) );
		RaxleObject->setPosition( &vector3(0, 0, 0 ) );

		FwheelObjectLEFT = new object( FwheelModel,  0, false  );
		FwheelObjectLEFT->setAngle( &vector3(0, 0, PI ) );
		FwheelObjectLEFT->setPosition( &FwheelPosition );
		FwheelObjectRIGHT = new object( FwheelModel,  0, false );
		FwheelObjectRIGHT->setAngle( &vector3(0, 0, 0 ) );
		FwheelObjectRIGHT->setPosition(&vector3(-FwheelPosition.x,FwheelPosition.y,FwheelPosition.z) );

		RwheelObjectLEFT = new object( RwheelModel,  0, false  );
		RwheelObjectLEFT->setAngle( &vector3(0,  0, PI ) );
		RwheelObjectLEFT->setPosition( &RwheelPosition );
		RwheelObjectRIGHT = new object( RwheelModel,  0, false );
		RwheelObjectRIGHT->setAngle( &vector3(0, 0, 0 ) );
		RwheelObjectRIGHT->setPosition( &vector3(-RwheelPosition.x,RwheelPosition.y,RwheelPosition.z));

		driverObject = new object( driverModel,  0, false  );
		driverObject->setAngle( &vector3(0, 0, 0 ) );
		driverObject->setPosition( &driverPosition );

		mainObject->insertChild(bodyObject);

		bodyObject->insertSister( FwheelObjectLEFT );
		FwheelObjectLEFT->insertSister( FwheelObjectRIGHT );
		FwheelObjectRIGHT->insertSister( RwheelObjectLEFT );
		RwheelObjectLEFT->insertSister( RwheelObjectRIGHT );
		RwheelObjectRIGHT->insertSister( FaxleObject);
		FaxleObject->insertSister(RaxleObject);

		bodyObject->insertChild( driverObject );


		object * currentObject = FaxleObject;
		if(	bodyShadowModel )
		{
			bodyShadowObject = new object( bodyShadowModel,  0, false  );
			bodyShadowObject->setAngle( &vector3(0, 0, 0 ) );
			bodyShadowObject->setPosition( &vector3(0, 0, 0 ) );
			currentObject->insertSister( bodyShadowObject );
			currentObject = bodyShadowObject;
		};

		if(	FaxleShadowModel )
		{
			FaxleShadowObject = new object( FaxleShadowModel,  0, false  );
			FaxleShadowObject->setAngle( &vector3(0, 0, 0 ) );
			FaxleShadowObject->setPosition( &vector3(0, 0.0f, 0 ) );
			currentObject->insertSister( FaxleShadowObject );
			currentObject = FaxleShadowObject;
		};
		if(	RaxleShadowModel )
		{
			RaxleShadowObject = new object( RaxleShadowModel,  0, false  );
			RaxleShadowObject->setAngle( &vector3(0, 0, 0 ) );
			RaxleShadowObject->setPosition( &vector3(0, 0.0f, 0 ) );
			currentObject->insertSister( RaxleShadowObject );
			currentObject = RaxleShadowObject;
		};
		if(	RwheelShadowModel )
		{
			RwheelShadowObjectLEFT = new object( RwheelShadowModel,  0, false  );
			RwheelShadowObjectLEFT->setAngle( &vector3(0, 0, PI ) );
			RwheelShadowObjectLEFT->setPosition( &RwheelPosition  );
			currentObject->insertSister( RwheelShadowObjectLEFT );
			currentObject = RwheelShadowObjectLEFT;
			RwheelShadowObjectRIGHT = new object( RwheelShadowModel,  0, false  );
			RwheelShadowObjectRIGHT->setAngle( &vector3(0, 0, 0 ) );
			RwheelShadowObjectRIGHT->setPosition( &vector3(-RwheelPosition.x,RwheelPosition.y,RwheelPosition.z) );
			currentObject->insertSister( RwheelShadowObjectRIGHT );
			currentObject = RwheelShadowObjectRIGHT;
		};
		if(	FwheelShadowModel )
		{
			FwheelShadowObjectLEFT = new object( FwheelShadowModel,  0, false  );
			FwheelShadowObjectLEFT->setAngle( &vector3(0, 0, PI ) );
			FwheelShadowObjectLEFT->setPosition( &FwheelPosition  );
			currentObject->insertSister( FwheelShadowObjectLEFT );
			currentObject = FwheelShadowObjectLEFT;
			FwheelShadowObjectRIGHT = new object( FwheelShadowModel,  0, false  );
			FwheelShadowObjectRIGHT->setAngle( &vector3(0, 0, 0 ) );
			FwheelShadowObjectRIGHT->setPosition( &vector3(-FwheelPosition.x,FwheelPosition.y,FwheelPosition.z) );
			currentObject->insertSister( FwheelShadowObjectRIGHT );
			currentObject = FwheelShadowObjectRIGHT;
		};


		radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		oneoverradiusF = RECIPROCAL( radiusF );
		oneoverradiusB = RECIPROCAL( radiusB );

				//
				// --- the wheel radius is taken as half the y-distance of the bounding box since
				//	the model of the wheel is 'standing' 
				//

		//radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		//radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		//oneoverradiusF = RECIPROCAL( radiusF );
		//oneoverradiusB = RECIPROCAL( radiusB );

    	wheel[1] = new carwheel(FwheelObjectLEFT); 
		wheel[1]->attached = true;
		wheel[1]->radius = radiusF;
  		wheel[1]->oneoverradius = oneoverradiusF;

		wheel[0] = new carwheel(FwheelObjectRIGHT); 
		wheel[0]->attached = true;
		wheel[0]->radius = radiusF;
  		wheel[0]->oneoverradius = oneoverradiusF;

		wheel[3] = new carwheel(RwheelObjectLEFT); 
		wheel[3]->attached = true;
		wheel[3]->radius = radiusB;
  		wheel[3]->oneoverradius = oneoverradiusB;

		wheel[2] = new carwheel(RwheelObjectRIGHT); 
		wheel[2]->attached = true;
		wheel[2]->radius = radiusB;
  		wheel[2]->oneoverradius = oneoverradiusB;


		wheelShadow[0] =
		wheelShadow[1] =
		wheelShadow[2] =
		wheelShadow[3] = 0;//FwheelObjectLEFT;

		wheelrotateoffset[0]=
		wheelrotateoffset[1]=
		wheelrotateoffset[2]=
		wheelrotateoffset[3]=0.0;

	    totalWheels = 4;

			
			  mainObject->inheritTransformations();		// initially create all concatenatedTransformations

			  exhaustSpriteFX = exhaustFX;
			  exhaustSpriteTimer.setTimer(20);		// Test of 3 times per second (should be prop. to revs
			  exhaustMaxSequences = 12;				// Maximum number of exhaust sprite sequences		
			  exhaustUsedSequences = 0;
			  body->exhaustCloudPoint = vector3(-0.25,-0.1,-2);
			 

		arcDispose( texhandle );

		CalculatePhysicalProperties();

	};

	arcPop();
	lastcollided=stuckinwall=false;
	accelvalue=0.0;
	acc=vector3(0,0,0);
	initialwheely[0]=initialwheely[1]=initialwheely[2]=initialwheely[3]=-1;
	//Store all sphere collision data
	Sphere *curhull;
	SphereSet *curspheres;
	curspheres = bodyObject->transformedSpheres;
	curhull = curspheres->data;
	spherecount=0;
	for(ulong j=0;j<curspheres->num;j++)
	{
		if(curhull->type==SPHERE)
		{
			carspheres[spherecount]=curhull;
			spherecount++;
		}
		curhull++;
	}
/*

	for(ulong k=0;k<4;k++)
	{
		curspheres = wheel[k]->wheelObject->transformedSpheres;
		curhull = curspheres->data;
		for(ulong j=0;j<curspheres->num;j++)
		{
			if(curhull->type==SPHERE)
			{
				carspheres[spherecount]=curhull;
				spherecount++;
			}
			curhull++;
		}
	}
*/
	tumbling=false;
	recalculate_overall_bounding_box = true;

}


bool car::willCarCollide( vector3 * wvelocity, vector3 * acceleration, float maximumTime,
			 							      vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime )
{
  object * obj;
  bool collided = false;

  obj = body->bodyObject;
  if( obj->transformedSpheres )
  {

	if( obj->transformedSpheres->collidesWithWorld(wvelocity, acceleration, maximumTime,
			 											  collisionVector,collisionPoint, collisionTime ) )
	{
		collided = true;
		maximumTime = *collisionTime;
	}
  }


  obj = wheel[0]->wheelObject;
  if( obj->transformedSpheres )
  {
	if( obj->transformedSpheres->collidesWithWorld(wvelocity, acceleration, maximumTime,
			 											  collisionVector,collisionPoint, collisionTime ) )
	{

		collided = true;
		maximumTime = *collisionTime;
	};
  };


  obj = wheel[1]->wheelObject;
  if( obj->transformedSpheres )
  {
	if( obj->transformedSpheres->collidesWithWorld(wvelocity, acceleration, maximumTime,
			 											  collisionVector,collisionPoint, collisionTime ) )
	{

		collided = true;
		maximumTime = *collisionTime;
	};	
  };

  obj = wheel[2]->wheelObject;
  if( obj->transformedSpheres )
  {
	if( obj->transformedSpheres->collidesWithWorld(wvelocity, acceleration, maximumTime,
			 											  collisionVector,collisionPoint, collisionTime ) )
	{

		collided = true;
		maximumTime = *collisionTime;
	};	
  };

  obj = wheel[3]->wheelObject;
  if( obj->transformedSpheres )
  {
	if( obj->transformedSpheres->collidesWithWorld(wvelocity, acceleration, maximumTime,
		 											  collisionVector,collisionPoint, collisionTime ) )
	{

		collided = true;
		maximumTime = *collisionTime;
	};
  }

  if(collided)grdebug("bang?","BANG!!!");
  return(collided);
}



//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
/*

bool car::isCollidingAtTime( float tdelta )
{
	
  bool collided;
  static vector3 distanceToCollision;

  distanceToCollision = engine->velocity;// * tdelta + acc * (0.5f * tdelta * tdelta);		

  moveBodyPositionInDirection( &distanceToCollision ) ;//&vector3(0.0f, 0.0f, (engine->velocity.x * mid) ) );
  body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY

					// look for collisions... Lets add up the total as the sum of the body AND the four wheels.

  collided = isInCollision();

  
 			// recover the body position...

  distanceToCollision.x = -distanceToCollision.x;
  distanceToCollision.y = -distanceToCollision.y;
  distanceToCollision.z = -distanceToCollision.z;

  moveBodyPositionInDirection(  &distanceToCollision ); //&vector3(0.0f, 0.0f, -engine->velocity.x * oldmid) );
  body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY

  return(collided);
};

*/


static __inline float rand1()
{
	return 
		((float)(rand()%1000)-500)*0.0001;
}


void car::Crumple(vector3 *collisionPoint, vector3 *collisionVector)

{
	// we only crumple LOD 0 

	mesh *meshdata = bodyObject->modelData->meshData+0;
	vector3 q;


	float vvv = 0.03 * velocity.Mag();

	if(vvv<1.0)return;

	for(int i=0;i<meshdata->vcount;i++)
	{
		vector3 *v = meshdata->vlistPtr[i].geoCoord;

		// work out the distance between this vector and the collision point
		// there's a bit of a problem with this in that the collision point doesn't seem
		// to be quite accurate.

		q = *v - *collisionPoint;
		float m = q.Mag() * 0.5;

		m = 0.8 - m;	// invert it
		if(m<0)m=0;

		// now move the point along the vector with a bit of random.

		q = vector3(rand1(),rand1(),rand1());
		q += velocity * 0.002;

		*v += q*m*vvv;
	}
	dprintf("coll %f %f %f %f",collisionPoint->x,collisionPoint->y,collisionPoint->z,vvv);
}



#define	ZROTLIMIT	0.1
#define	XROTLIMIT	0.1
	
void car::doSuspension(float turn)
{
	float xangle,zangle,cosx,cosz,sinx,sinz;

	xangle = -acc.z * 0.004;
	zangle = acc.x * 0.004;

	if(xangle>XROTLIMIT)xangle=XROTLIMIT;
	if(xangle<-XROTLIMIT)xangle=-XROTLIMIT;
	if(zangle>ZROTLIMIT)zangle=ZROTLIMIT;
	if(zangle<-ZROTLIMIT)zangle=-ZROTLIMIT;

	cosx=cos(xangle);	cosz=cos(zangle);
	sinx=sin(xangle);	sinz=sin(zangle);


	// OK, now rotate the bodyobject accordingly

	static matrix3x4 RX, RZ, temp1;

	RX.set3x3submat(	1,   0,   0,  
		0,  cosx,  sinx,  
		0,  -sinx,  cosx
		);
	
	RZ.set3x3submat( 	cosz,  sinz, 0, 
		-sinz, cosz, 0, 
		0, 0, 1
		);

	grdebug("cosx",cosx);
	grdebug("cosz",cosz);

	grdebug("xacc",acc.x);
	grdebug("zacc",acc.z);

	temp1.multiply3x3submats( &RX, &RZ);	// aha, a rotation.
	temp1.translation=vector3(0,0,0);

	bodyObject->transform = temp1;

	bodyObject->changed   = true;

	// now do the driver object

	zangle = zangle*2 - turn * mph * 0.002;

	cosz=cos(-zangle);
	sinz=sin(-zangle);


	RZ.set3x3submat( 	cosz,  sinz, 0, 
		-sinz, cosz, 0, 
		0, 0, 1
		);


	RZ.translation=vector3(0,0,0);

	driverObject->transform = RZ;
	driverObject->changed   = true;

	mainObject->inheritTransformations();

	{
		vector3 tmp;
		wheel[0]->wheelObject->getAngle(&tmp);tmp.y+=wheelrotateoffset[0];wheel[0]->wheelObject->setAngle(&tmp);
		wheel[1]->wheelObject->getAngle(&tmp);tmp.y+=wheelrotateoffset[1];wheel[1]->wheelObject->setAngle(&tmp);
		wheel[2]->wheelObject->getAngle(&tmp);tmp.y+=wheelrotateoffset[2];wheel[2]->wheelObject->setAngle(&tmp);
		wheel[3]->wheelObject->getAngle(&tmp);tmp.y+=wheelrotateoffset[3];wheel[3]->wheelObject->setAngle(&tmp);
		
		wheelrotateoffset[0]=wheelrotateoffset[1]=wheelrotateoffset[2]=wheelrotateoffset[3]=0;
	}

	return;

	for(int i=0;i<4;i++)
	{
		triangle *tp;
		float h;

		if( wheel[i]->getWorldHeight( &h , &tp))
		{
			if(initialwheely[i]<-9999)
				initialwheely[i]=h;

			float d = h-initialwheely[i];

			if(d<-0.3)h=initialwheely[i]-0.3;
			if(d<0.3)h=initialwheely[i]+0.3;

			wheel[i]->wheelObject->setPosition_Y(h);
			wheel[i]->wheelObject->changed=true;
		}
	}

	mainObject->inheritTransformations();



}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
