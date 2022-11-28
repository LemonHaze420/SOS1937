/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\car.cpp,v 1.21 1999/02/03 14:49:10 jcf Exp jcf $
	$Log: car.cpp,v $
	Revision 1.21  1999/02/03 14:49:10  jcf
	Updated plonkOnGround and moved a few things around. Still no joy.

	Revision 1.20  1999/02/01 16:28:17  jcf
	Turning seems to work, but the car doesn't stay on the ground when it goes up a hill!

	Revision 1.19  1999/01/28 17:22:30  jcf
	Quick check in. Not working.

	Revision 1.18  1999/01/27 12:51:24  jcf
	Phase one of turning point code.

	Revision 1.17  1999/01/26 16:16:35  jcf
	First round of tidying-up.

	Revision 1.16  1999/01/25 17:19:36  jcf
	Removed most of the spurious stuff before I start hacking the physics code around.. particularly the rotational stuff.

	Revision 1.15  1999/01/25 14:57:47  jcf
	Made a few changes, tidying up

	Revision 1.14  1999/01/22 14:58:06  jcf
	Folded in functionality from carengine

	Revision 1.13  1999/01/22 11:29:40  jcf
	Another check in before a major change.

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

static const char *rcs_id="$Id: car.cpp,v 1.21 1999/02/03 14:49:10 jcf Exp jcf $";

//================================================================================================================================
// car.cpp
// --------
//
//================================================================================================================================

#include "ALLcar.h"


// JCF engine constants

#define	THROTTLE_DAMPING	0.96f


extern engine * boris2;




#define HULL



extern void dprintf(char *,...);



void grdebugmat(char *s,matrix3x4 *m)
{

	dprintf("MATRIX : %s\n",s);

	dprintf("%f    %f    %f\n",m->E00,m->E01,m->E02);
	dprintf("%f    %f    %f\n",m->E10,m->E11,m->E12);
	dprintf("%f    %f    %f\n",m->E20,m->E21,m->E22);

	dprintf("TX : %f     %f     %f\n",m->translation.x,m->translation.y,m->translation.z);
}




// engine stuff



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
	body->bodyObject->setAngle_X( a );
};

void car::setBodyAngle_Y( float a )
{
	body->bodyObject->setAngle_Y( a );
};

void car::setBodyAngle_Z( float a )
{
	body->bodyObject->setAngle_Z( a );
};



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
	triangle * bodytrianglePtr, *trianglePtr0, *trianglePtr1, *trianglePtr2, *trianglePtr3;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3;
	
#define DELTAANGLE 0.002f
	
	
	//setBodyAngle( &( vector3(0, cycles * localTurn, 0) + body->bodyObject->angle) );//angle ); 
	//moveBodyPositionInDirection( &vector3(0.0f, 0.0f, engine->velocity.z) );
	
	//
 vector3 tempv = body->bodyObject->transform.translation;	
 body->bodyObject->transform = wheelTurnMatrix;
 body->bodyObject->transform.translation = tempv;
 body->bodyObject->changed = true;
 body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY (car object tree)
	
	float min, max;

		if( !boris2->currentworld->getY(&body->bodyObject->concatenatedTransform.translation, &bodyY -1, &bodytrianglePtr, false ) )
		{
			bodytrianglePtr = NULL;
			bodyY = 0.0f;
		};
		
		// Get heights of wheels and body etc. from the new position
		
		if( !wheel[0]->getWorldHeight( &heightvalue0 , &trianglePtr0) )
		{
			heightvalue0 = 0.0f;
			trianglePtr0 = NULL;
		};
		heightvalue0 += wheel[0]->radius;
		
		if( !wheel[1]->getWorldHeight( &heightvalue1 , &trianglePtr1 ) )
		{
			heightvalue1 = 0.0f;
			trianglePtr1 = NULL;
		};
		heightvalue1 += wheel[1]->radius;
		
		if( !wheel[2]->getWorldHeight( &heightvalue2 , &trianglePtr2) )
		{
			heightvalue2 = 0.0f;
			trianglePtr2 = NULL;
		};
		heightvalue2 += wheel[2]->radius;
		
		if( !wheel[3]->getWorldHeight( &heightvalue3 , &trianglePtr3 ) )
		{
			heightvalue3 = 0.0f;
			trianglePtr3 = NULL;
		};
		heightvalue3 += wheel[3]->radius;
		
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
		float sinz = ( dyB *  RECIPROCAL( sqrt( dxB * dxB + dyB * dyB + dzB * dzB ) ));
		
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
		
		tempm.multiply3x3submats( &RX, &RZ);
		temp1.multiply3x3submats( &body->bodyObject->transform, &tempm  );			
		
		temp1.translation = body->bodyObject->transform.translation;
		
		body->bodyObject->transform = temp1;
		
		body->bodyObject->changed   = true;
		
		
		//minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		
		/*
		// reget heights
		body->bodyObject->inheritTransformations();
		
		  if( !wheel[0]->getWorldHeight( &heightvalue0 , &trianglePtr0) )
		  {
		  heightvalue0 = 0.0f;
		  };
		  heightvalue0 += wheel[0]->radius;
		  
			if( !wheel[1]->getWorldHeight( &heightvalue1 , &trianglePtr1 ) )
			{
			heightvalue1 = 0.0f;
			};
			heightvalue1 += wheel[1]->radius;
			
			  if( !wheel[2]->getWorldHeight( &heightvalue2 , &trianglePtr2) )
			  {
			  heightvalue2 = 0.0f;
			  };
			  heightvalue2 += wheel[2]->radius;
			  
				if( !wheel[3]->getWorldHeight( &heightvalue3 , &trianglePtr3 ) )
				{
				heightvalue3 = 0.0f;
				};
				heightvalue3 += wheel[3]->radius;
				
		*/
		
		
		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		
		max = (max+min)*0.5f;
		
		setBodyPosition_Y( max + 0.2f);			// Note 'changed' flag set ready for new resolve of geometry
		body->bodyObject->inheritTransformations();


		float dh;
		float maxinc = 0.0f;

		carwheel ** wheelptr = wheel;
		for(long iii = 4;iii--;)
		{
			if( (*wheelptr)->getWorldHeight( &heightvalue0 , &trianglePtr0) )
			{
				float f = ((*wheelptr)->wheelObject->transform.translation.y);
				dh = f - heightvalue0;
				if( dh > maxinc ) maxinc = dh;			
			};
			wheelptr++;
		};
	
		setBodyPosition_Y(  max + maxinc + 0.2f ); // Note 'changed' flag set ready for new resolve of geometry
		body->bodyObject->inheritTransformations();
/*
	{
		//if( !boris2->currentworld->getY(&body->bodyObject->concatenatedTransform.translation, &bodyY -1, &bodytrianglePtr, false ) )
		//{
		//	bodytrianglePtr = NULL;
		//	bodyY = 0.0f;
		//};
		
		// Get heights of wheels and body etc. from the new position
		
		if( !wheel[0]->getWorldHeight( &heightvalue0 , &trianglePtr0) )
		{
			heightvalue0 = 0.0f;
			trianglePtr0 = NULL;
		};
		heightvalue0 += //wheel[0]->radius + 
						(wheel[0]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[0]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[0]->wheelObject->transform.translation.y; 

		
		if( !wheel[1]->getWorldHeight( &heightvalue1 , &trianglePtr1 ) )
		{
			heightvalue1 = 0.0f;
			trianglePtr1 = NULL;
		};
		//heightvalue1 += wheel[1]->radius;
		heightvalue1 += //wheel[0]->radius + 
						(wheel[1]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[1]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[1]->wheelObject->transform.translation.y; 

		if( !wheel[2]->getWorldHeight( &heightvalue2 , &trianglePtr2) )
		{
			heightvalue2 = 0.0f;
			trianglePtr2 = NULL;
		};
		//heightvalue2 += wheel[2]->radius;
		heightvalue2 += //wheel[0]->radius + 
						(wheel[2]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[2]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[2]->wheelObject->transform.translation.y; 

		
		if( !wheel[3]->getWorldHeight( &heightvalue3 , &trianglePtr3 ) )
		{
			heightvalue3 = 0.0f;
			trianglePtr3 = NULL;
		};
		//heightvalue3 += wheel[3]->radius;
		heightvalue3 += //wheel[0]->radius + 
						(wheel[3]->wheelObject->modelData->boundingBox.hi.y - 
						 wheel[3]->wheelObject->modelData->boundingBox.lo.y) * 0.5f 
						- wheel[3]->wheelObject->transform.translation.y; 
	
		//heightsum = heightvalue0 + heightvalue1 + heightvalue2 + heightvalue3;
		//


		static vector3 v0 = wheel[0]->wheelObject->concatenatedTransform.translation ;
		static vector3 v1 = wheel[1]->wheelObject->concatenatedTransform.translation ;
		static vector3 v2 = wheel[2]->wheelObject->concatenatedTransform.translation ;
		static vector3 v3 = wheel[3]->wheelObject->concatenatedTransform.translation ;

		v0.y = heightvalue0 ;
		v1.y = heightvalue1;
		v2.y = heightvalue2;
		v3.y = heightvalue3;

		static vector3 pn0,pn1,pn2,pn3, pn;
		static vector3 U,V,V1;
		static matrix3x4 RX, RY, RZ,  tempm, temp1;


		getPlaneNormal( &pn0, &v0, &v2, &v1 );
		getPlaneNormal( &pn1, &v1, &v2, &v3 ); 
		getPlaneNormal( &pn2, &v2, &v3, &v0 ); 
		getPlaneNormal( &pn3, &v3, &v1, &v0 ); 


		pn = (pn0 + pn1 + pn2 + pn3 ) * 0.25f;		// average plane normal

		V1 = vector3(0,0,1);

		float dp = V1.DotProduct(&pn);
		V = V1 - pn * dp;
		V.Normalise();
		U =  pn * V;			// Cross product
		U.Normalise();

		temp1.set3x3submat( -U.x, pn.x, -V.x,
							-U.y, pn.y, -V.y,
							-U.z, pn.z, -V.z );


//temp1.orthogonalise();

		tempm.multiply3x3submats(&temp1, &wheelTurnMatrix); //&body->bodyObject->transform, &temp1 );			
		temp1 = tempm;

//temp1 = wheelTurnMatrix;
		temp1.translation = body->bodyObject->transform.translation;


		
		body->bodyObject->transform = temp1;
		body->bodyObject->changed   = true;	

		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		
		max = (max+min)*0.5f;
		
		setBodyPosition_Y(  max  );//+ 0.25f);			// Note 'changed' flag set ready for new resolve of geometry
//		body->bodyObject->inheritTransformations();
*/
/*
		float dh;
		float maxinc = 0.0f;

		carwheel ** wheelptr = wheel;
		for(long iii = 4;iii--;)
		{
			if( (*wheelptr)->getWorldHeight( &heightvalue0 , &trianglePtr0) )
			{
				float f = ((*wheelptr)->wheelObject->transform.translation.y);
				dh = f - heightvalue0;
				//if( dh> 0.0f )
					if( dh > maxinc ) maxinc = dh;			
			};
			wheelptr++;
		};
	
		setBodyPosition_Y(  max  );//+ lofter );// + maxinc ); // Note 'changed' flag set ready for new resolve of geometry
*/

//turnAroundWorldSpacePoint( &vector3(940,0,40), &vector3( 0,PI,0) );

		body->bodyObject->inheritTransformations();
//setBodyPosition_Y(  max + 0.2f + wheel[0]->radius);
};

/*



void car::plonkOnGround()
{
	
	float deltaW, deltaB, bodyY;
	triangle * bodytrianglePtr, *trianglePtr0, *trianglePtr1, *trianglePtr2, *trianglePtr3;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3;
	
#define DELTAANGLE 0.002f
	
	
	float min, max;
	{
		if( !boris2->currentworld->getY(&body->bodyObject->concatenatedTransform.translation, &bodyY -1, &bodytrianglePtr, false ) )
		{
			bodytrianglePtr = NULL;
			bodyY = 0.0f;
		};
		
		// Get heights of wheels and body etc. from the new position
		
		if( !wheel[0]->getWorldHeight( &heightvalue0 , &trianglePtr0) )
		{
			heightvalue0 = 0.0f;
			trianglePtr0 = NULL;
		};
		heightvalue0 += wheel[0]->radius  ;
		
		if( !wheel[1]->getWorldHeight( &heightvalue1 , &trianglePtr1 ) )
		{
			heightvalue1 = 0.0f;
			trianglePtr1 = NULL;
		};
		heightvalue1 += wheel[1]->radius;
		
		if( !wheel[2]->getWorldHeight( &heightvalue2 , &trianglePtr2) )
		{
			heightvalue2 = 0.0f;
			trianglePtr2 = NULL;
		};
		heightvalue2 += wheel[2]->radius;
		
		if( !wheel[3]->getWorldHeight( &heightvalue3 , &trianglePtr3 ) )
		{
			heightvalue3 = 0.0f;
			trianglePtr3 = NULL;
		};
		heightvalue3 += wheel[3]->radius;
		
		//heightsum = heightvalue0 + heightvalue1 + heightvalue2 + heightvalue3;
		
		//
		
		vector3 *v0 = &(wheel[0]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v1 = &(wheel[1]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v2 = &(wheel[2]->wheelObject->concatenatedTransform.translation) ;
		vector3 *v3 = &(wheel[3]->wheelObject->concatenatedTransform.translation) ;
			
		dprintf("wheel 0 %f %f %f",v0->x,v0->y,v0->z);

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
		
		tempm.multiply3x3submats( &RX, &RZ);

		temp1.multiply3x3submats( &body->bodyObject->transform, &tempm  );			
		
		temp1.translation = body->bodyObject->transform.translation;
		
		body->bodyObject->transform = temp1;
		
		body->bodyObject->changed   = true;
		
		
		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		
		max = (max+min)*0.5f;
		
		setBodyPosition_Y(  max + 0.25f);			// Note 'changed' flag set ready for new resolve of geometry
		body->bodyObject->inheritTransformations();


		float dh;
		float maxinc = 0.0f;

		carwheel ** wheelptr = wheel;
		for(long iii = 4;iii--;)
		{
			if( (*wheelptr)->getWorldHeight( &heightvalue0 , &trianglePtr0) )
			{
				float f = ((*wheelptr)->wheelObject->transform.translation.y);
				dh = f - heightvalue0;
				//if( dh> 0.0f )
					if( dh > maxinc ) maxinc = dh;			
			};
			wheelptr++;
		};
	
		setBodyPosition_Y(  max + 0.3f );// + maxinc ); // Note 'changed' flag set ready for new resolve of geometry
		body->bodyObject->inheritTransformations();


};
//setBodyPosition_Y(  max + 0.2f + wheel[0]->radius);
};




*/






#define GRAVITY 0.10f

extern vector3 collvector;
extern vector3 collpoint;
#define MINIMUMTOI	0.1f
#define	TIME_DIVISOR	10

float xxxx = 0;
vector3 wheeldisp = vector3(0,0,0);

void car::rotateCarAroundLocalPoint(vector3 &tp,float carturnangle)
{
	// translate the turning point to world space, rotate the car around it around the car's local
	// Y axis.

	// this involved translating the TP and Y axis to the world, rotating the car point
	// around that.


	float tangle = ( carturnangle );
	float cosy = cos(tangle );
	float siny = sin(tangle );
	static matrix3x4 tempmatrix, tempmatrix2;
	
	static vector3 disp;

	tempmatrix.set3x3submat(	cosy, 0, -siny,
						0,  1, 0, 
						siny, 0, cosy
						    );
	tempmatrix2.multiply3x3submats( &wheelTurnMatrix, &tempmatrix );
	
	wheelTurnMatrix = tempmatrix2;

	static vector3 t1;

	t1 = tp;
	t1.y = 0;
	t1.x = t1.x;
	t1.z = t1.z;

	disp.multiplyByMatrix3x3( &t1, &tempmatrix );
	wheeldisp -= disp;

	body->bodyObject->transform.translation += disp * 0.01f; //wheeldisp*0.01f;

	body->bodyObject->changed = true;

/*
	static vector3 worldyaxis,worldtp;	

		vector3 yaxis(0,1,0);

//	body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &worldyaxis, &yaxis);
//	body->bodyObject->concatenatedTransform.multiplyV3By3x4mat( &worldtp, &tp);

	// rotation around an axis from a point:
	// 1) translate from TP the origin
	// 2) rotate around the axis
	// 3) translate back again

	static matrix3x4 t1,r,t2,tmp,tmp2;

	t1.set3x3submat(1,0,0,0,1,0,0,0,1);
	t2.set3x3submat(1,0,0,0,1,0,0,0,1);

	t1.translation.negate(&worldtp);
	t2.translation = worldtp;

	float sina, cosa;

	float x,y,z;

	if(tp.x > 0) carturnangle = -carturnangle;

	cosa = mySineAndCosine( carturnangle, &sina);

	float cosa1 = 1-cosa;

	x=worldyaxis.x;	y=worldyaxis.y;	z=worldyaxis.z;

	// this might actually be its transpose.
	r.setmat(	x*x+cosa*(1-x*x),	x*y*cosa1+z*sina,	z*x*cosa1+y*sina,
				x*y*cosa1+z*sina,	y*y+cosa*(1-y*y),	y*z*cosa1-x*sina,
				z*x*cosa1-y*sina,	y*z*cosa1+x*sina,	z*z+cosa*(1-z*z),
				0,					0,					0);


	dprintf("=============================================================================\n");

	dprintf("axis: %f,%f,%f\n",x,y,z);

	grdebugmat("rotation",&r);

	dprintf("TP : %f %f %f",tp.x,tp.y,tp.z);
	dprintf("WTP : %f %f %f",worldtp.x,worldtp.y,worldtp.z);

	grdebugmat("old transform",&body->bodyObject->transform);
	dprintf("old position : %f %f %f\n",
		body->bodyObject->transform.translation.x,
		body->bodyObject->transform.translation.y,
		body->bodyObject->transform.translation.z);

//	tmp.multiply3x4mats(&body->bodyObject->transform,&t1);
wheelTurnMatrix.translation = body->bodyObject->transform.translation;
tmp.multiply3x4mats(&t1, &wheelTurnMatrix) ;//&body->bodyObject->transform);

	dprintf("translation after t1 : %f %f %f",
		tmp.translation.x,
		tmp.translation.y,
		tmp.translation.z);

	tmp2.multiply3x4mats(&r,&tmp);
	dprintf("translation after r : %f %f %f",
		tmp2.translation.x,
		tmp2.translation.y,
		tmp2.translation.z);



//body->bodyObject->transform.multiply3x4mats(&t2,&tmp2);
wheelTurnMatrix.multiply3x4mats(&t2,&tmp2);

//body->bodyObject->transform.translation = wheelTurnMatrix.translation;
	
	dprintf("translation after t2 : %f %f %f\n",
		body->bodyObject->transform.translation.x,
		body->bodyObject->transform.translation.y,
		body->bodyObject->transform.translation.z);

	grdebugmat("new transform",&body->bodyObject->transform);

//	body->bodyObject->changed = true;
//	body->bodyObject->inheritTransformations();
//
*/
}



// and the velocity by the angle
void car::rotateVelocityAroundY(float carturnangle)
{
	static matrix3x4 m;
	static vector3 v;

	float cAngle,sAngle;

	cAngle = mySineAndCosine( carturnangle, &sAngle);
	
	m.set3x3submat(	cAngle, 0, -sAngle,
						0,  1, 0, 
						sAngle, 0, cAngle
					 );

	v.multiplyByMatrix3x3(&velocity,&m);
	velocity=v;
}


void car::move(ulong ticks, float turn, float throttle, float  brake,  ulong cycles )
{
	triangle * trianglePtr0, * trianglePtr1, * trianglePtr2, * trianglePtr3, * bodytrianglePtr;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3, heightsum, bodyY, minY;
	float oneoverphysicsTicks = boris2->oneoverphysicsTicks;
	float localTurn,transrevs;
	bool engaged;

#define	REVSTOVELOCITY	(3000.0	* 200 * boris2->oneoverphysicsTicks )// factor to divide revs by to get velocity (i.e. final drive function)
#define ONEOVERREVSTOVELOCITY  ( 1.0f / REVSTOVELOCITY )
#define	AUTOCLUTCHLIMIT	150	// revs at which the autoclutch engages

	float timeElapsed = oneoverphysicsTicks * (float)ticks ;

//	grdebug("time",timeElapsed);

	// get the amount of steering. The 5 here is a constant controlling the amount of steering

	localTurn = turn * timeElapsed * 5;	

	
	// determine whether the clutch is engaged
	engaged = currentGearNumber && (revs<0 ? -revs : revs)>AUTOCLUTCHLIMIT;

	// now we get the wheel revs back from the velocity

	transrevs = localVelocity.z*REVSTOVELOCITY;
//	grdebug("postvel",transrevs);
//	transrevs /= wheel[0]->oneoverradius;
//	grdebug("P-transrevs",transrevs);

//	grdebug("P-engrevs",revs);

	if(engaged)
	{
		// the engine only slows down if the geared transmission revs are close to the engine revs.
		// it's a hack, but there you go.

		float ttt = invgearbox(transrevs);
		float gap = revs - ttt;
		if(gap<0)gap=-gap;
		if(previousGearNumber)
		{
				if(gap > 500)
					revs = invgearbox(transrevs);
		}
		else
		{
			revs = revs/2;
		}
	}
	if(revs<0)revs=0;
 
	// JCF - new drive train stuff

	// damp throttle value

	damped_throttle = damped_throttle*(1-THROTTLE_DAMPING) + throttle*THROTTLE_DAMPING; // change to logs?
//	grdebug("throttle",damped_throttle);
//	grdebug("brake",brake);


	// add the 'kick' of the throttle to the engine revs.

	// first, we use the power curve for this car to get us a power
	// multiplier. Neutral has a power of 3 automatically.

	float powr = engaged ? getpower(revs) : 3;
	//grdebug("power",powr);

	// we add some factor of the throttle times the power to the revs

	revs += damped_throttle*700*powr*timeElapsed;


	//grdebug("pre-eng",revs);

	// allow for engine friction, which is greater at low revs. This is a hack to simulate
	// the lower power in higher gears.

	float engine_friction;

	if(timeElapsed)
	{
		if(damped_throttle>0.8)
		{
			engine_friction = currentGearNumber ? 0.6+revs/(
				currentGearPtr->frictionvalue*currentGearPtr->ratio) : 0.95;
			if(engine_friction>0.99)engine_friction=0.99;
		}
		else
		{
			engine_friction = 0.99; 
		}
		
		revs *=( engine_friction < 0.0f ) ?	
			-pow(-engine_friction,timeElapsed*10) : 
		pow(engine_friction,timeElapsed*10);
	};


	if(revs>7000)revs=7000;	// clamp the revs

	//grdebug("friction",engine_friction);

	// apply the gearbox function to get the transmission revs

	if(engaged)transrevs = gearbox(revs);

	//grdebug("transrevs",transrevs);

//dprintf("gear: %d, engine(pre acc): %f, engine(post acc): %f, transmission: %f, ",currentGearNumber,
//revs-damped_throttle*40,revs,transrevs);
	

	// decrease the transmission revs by some factor of the brake pedal value.

	brake *= pow(0.95,timeElapsed*10);
	transrevs *= 1.0-brake;

	// convert engine revs into car velocity
	localVelocity.z = transrevs * ONEOVERREVSTOVELOCITY;
	// transform car velocity to world space
	body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &velocity, &localVelocity );


	//grdebug("prevel",localVelocity.z);

	float timeleft;
	float interval;
	
	static vector3 gravforce;
	static vector3 v, v1;
	vector3 tp(0,0,0);	// local space turning point
	
	static vector3 damping, dampVector;
	static vector3 tempf;
	static matrix3x4 m;
	float float_cycles = (float)cycles;

	ulong i;

	if(cycles)
	{
		static vector3 a;
		float wheelturnangle = -turn/3;

		//grdebug("wheelangle",wheelturnangle);

		cycles /= TIME_DIVISOR;
		if(!cycles)cycles=1;

//static matrix3x4 tttt = body->bodyObject->transform;

		
		// turning code goes here
		// * work out the angle of the wheels - some factor of localTurn
		// * set the wheel objects to this angle - and also rotate around x for moving across the ground.

#define WANGLE(a)	(localVelocity.z  * wheel[a]->oneoverradius)

		getWheelAngle(0,&a);	a.y=wheelturnangle;	a.x += WANGLE(0);		setWheelAngle(0,&a);	
		getWheelAngle(1,&a);	a.y=wheelturnangle;	a.x += WANGLE(1);		setWheelAngle(1,&a);	

		getWheelAngle(2,&a);	a.x += WANGLE(2);		setWheelAngle(2,&a);	
		getWheelAngle(3,&a);	a.x += WANGLE(3);		setWheelAngle(3,&a);	


		// * work out the position of the turning point
		//		this is on the intersection of the front axle and back axle lines, moving forward at
		//		high speeds as the back wheels lose grip.
	

//	body->bodyObject->changed = true;
//	body->bodyObject->inheritTransformations();
//	plonkOnGround();
//	body->bodyObject->changed = true;
//	body->bodyObject->inheritTransformations();

		if(wheelturnangle)
		{

			tp.y = 0;
			tp.z = 0;
			tp.x = wheel[0]->wheelObject->transform.translation.z * tan(PI/2-wheelturnangle) / 2;
			//grdebug("tpx",tp.x);

			// add speed adjustment to tp.z here
			
		}

	
		if(cycles > 2) cycles = 2;
		
		dampVector = vector3( 1, 1, 1 );
		body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &damping, &dampVector );
		
		// collision loop
		for(i=cycles;i--;)
		{
		body->bodyObject->inheritTransformations();
		plonkOnGround();
		body->bodyObject->inheritTransformations();

			timeleft=boris2->oneoverphysicsTicks;

			gravforce = vector3(0,GRAVITY,0);

			//
			// Now we must turn the 'gravforce' from world space into the local objects space
			// and damp the local 'x' component to simulate heavily damped sideways movement.
			// Then turn the force back into world space (phew!!)
			//
			
			body->bodyObject->inheritTransformations();
			m.Transpose3x4(&body->bodyObject->concatenatedTransform); //Transpose3x4( &m );
			
			
			// Reduce force in 'x'
			
			m.multiplyV3By3x3mat( &tempf, &gravforce );
			tempf.x *= 0.01f;//0;//.2f;
			body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &gravforce, &tempf );
			
			
			ulong ctest = 0;
			while(	isInCollision() )
			{
				
				// Attempt to recover from collision by moving the car gently out along the 
				// collision surface normal
				
				object * whichObject;
				
				v1 = collvector * 0.5f;
				whichObject = body->bodyObject;
				
				
				whichObject->movePosition( &v1 );	// updates the position for this interval
				whichObject->inheritTransformations();			
				
				ctest++;
				if( ctest == 20 ) break;
			};
			
			if( ctest )	 // change the directional stuff ...
			{
				collision = true;			// hold this value for main code 
				collisionPoint = collpoint;
				float mag;
				
				//grdebug("wturn",wheelturnangle);

//	new move code.
				if(wheelturnangle)
				{
					// we work out the angle around the turning point through
					// which we would need to rotate the car in order to make it
					// move at the current velocity. Length of an arc = r.theta

					float mag = sqrt(velocity.x*velocity.x + velocity.y*velocity.x + velocity.z*velocity.z);
					float carturnangle = (mag * float_cycles) / tp.x;	// angle = v/r

					//grdebug("carturn",carturnangle);

					// we now need to rotate the car around the point by the abs.val of this angle

					rotateCarAroundLocalPoint(tp,carturnangle);//<0?-carturnangle:carturnangle);
					// and the velocity by the angle

					rotateVelocityAroundY(carturnangle);
				}
				else
				{
					// it's a straight line move

					v = velocity * float_cycles;
					body->bodyObject->movePosition( &v);	// updates the position for this interval
					body->bodyObject->inheritTransformations();
				}

				
				v = velocity ;
				v.NormaliseWithCheckAndMag(&mag);
				v1 = v - collvector * ( 2.0f * collvector.DotProduct(&v) ) ;
				
				
				static vector3 cp;
				
				
				cp = v * collvector;


				kick.x = 0;//cp.x * 0.1f;
				kick.z = 0;//cp.z * 0.1f;
				kick.y = -(0.7f - cp.y) *  0.21f ;

				
				kick.y *= fsqrt(fsqrt(fabs(localVelocity.z)  * 0.04f)); /// 25.0f));
				
				velocity = v1 * mag * 0.86f;
				
				kickDelta.x = kickDelta.z = 0.98f;
				
				kickDelta.y = 0.96f;
				
			}
			else
			{
				if( findTimeOfImpact( &interval, timeleft ) )	// How long to collide ?
				{
					// We shall collide at approx. 'interval' seconds
					
				} else		// We shall not collide so move continuously through the interval
					interval = timeleft;

				if(wheelturnangle)
				{
					// we work out the angle around the turning point through
					// which we would need to rotate the car in order to make it
					// move at the current velocity. Length of an arc = r.theta

					float mag = sqrt(velocity.x*velocity.x + velocity.y*velocity.x + velocity.z*velocity.z);
					float carturnangle = (mag * float_cycles) / tp.x;	// angle = v/r

					//grdebug("carturn",carturnangle);

					// we now need to rotate the car around the point by the abs.val of this angle

					rotateCarAroundLocalPoint(tp,carturnangle);//<0?-carturnangle:carturnangle);
//tttt = body->bodyObject->transform;


					// and the velocity by the angle

					rotateVelocityAroundY(carturnangle);
				}
				else
				{

					// it's a straight line move

					v = velocity * float_cycles;
					body->bodyObject->movePosition( &v);	// updates the position for this interval
					body->bodyObject->inheritTransformations();
				}
				v1 = acc * interval;
				velocity += v1;
				
				
				
				if(isInSpin() )
				{
					SpinCam = 1.0f;
					SpinDelta = 0.0001f;
				};
				
				body->bodyObject->inheritTransformations();
				
				//(!isInSpin() )
				if(body->bodyObject->attached)
				{
					static vector3 temp;
					
					temp = body->bodyObject->transformedattachmentPoint - body->bodyObject->concatenatedTransform.translation;
					
					
					body->bodyObject->attached->position = 
						(body->bodyObject->concatenatedTransform.translation - temp*3 ) * ( 1.0f - SpinCam) + 
						body->bodyObject->attached->position * SpinCam;
					
					temp = body->bodyObject->transformedattachmentPoint * ( 1.0f - SpinCam) +
						body->bodyObject->concatenatedTransform.translation * SpinCam;
					
					body->bodyObject->attached->faceTowardPoint(  &temp );
				};	
				
				SpinCam -= SpinDelta;
				SpinDelta *= 1.01f;
				if(SpinCam > 1.0f )
					SpinCam = 1.0f;
				else
					if(SpinCam < 0.0f )
						SpinCam = 0.0f;
					
					
					m.Transpose3x4(&body->bodyObject->concatenatedTransform); //Transpose3x4( &m );
					
			};
			
			// reduce velocity in 'x' for friction
			// reduce velocity in 'z' for braking
			
			m.multiplyV3By3x3mat( &localVelocity, &velocity );

			localVelocity.x *= 0.95f;		// was 0.68
			localVelocity.x *= 0.1f;		// snark

			float b = braking;
			if( b < (1.0f-0.3f)) b += 0.3f;
			localVelocity.z *= (1.0f - b*0.01f);
			
			
			if( localVelocity.z > 25.0f ) localVelocity.z = 25.0f;		// 25 * 6 = 150
			
			body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &velocity, &localVelocity );
			
		};
	

//snark
		//moveBodyAngle( &kick );	// this is the spin around the centre point.
		//moveBodyAngle( &extrarotvel) ;	// this is the spin due to the steering.
		
	body->bodyObject->changed = true;
	body->bodyObject->inheritTransformations();
	plonkOnGround();
	body->bodyObject->changed = true;
	body->bodyObject->inheritTransformations();


//		kick.x *= kickDelta.x;
//		kick.y *= kickDelta.y;
//		kick.z *= kickDelta.z;
//		
//		kick *= 0.1;	//snark

		
	}
	else
		body->bodyObject->inheritTransformations();



	previousGearNumber = currentGearNumber;

}



float car::getEngineRevs()
{
	return revs;
}

float car::getWheelRevs()
{
	return transrevs;
}


//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::setBodyPosition_Y( float p )
{
	body->bodyObject->setPosition_Y( p );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::setWheelPosition_Y( ulong wheelNumber, float p)
{
	wheel[wheelNumber]->wheelObject->setPosition_Y( p );
};


void car::updateExhaust()
{
	/*
	ulong eticks = exhaustSpriteTimer.getTicks();
	if( eticks )
	{
		if( exhaustUsedSequences < exhaustMaxSequences )
		{


		body->bodyObject->concatenatedTransform.multiplyV3By3x4mat( &body->transformedexhaustCloudPoint, &body->exhaustCloudPoint);


		boris2->appendSpriteFX( exhaustSpriteFX, 
								&body->transformedexhaustCloudPoint,
								&exhaustUsedSequences);

		};
		
	};
	*/
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
//--------------------------------------------------------------------------------------------------------------------------------

long car::getGear()
{
	return( currentGearNumber );
};



//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

float car::gearbox(float revs)	// jcf 
{
	float gratio = currentGearPtr->ratio;

	if( gratio )
	{
		float erevs = revs * gratio ;
		return( erevs );
	}
	else
		return(0);
};

float car::invgearbox(float revs)
{
	float gratio = currentGearPtr->ratio;

	if( gratio )
	{
		float erevs = revs * RECIPROCAL( gratio ) ;

		// put limiting in here.
		return( erevs );
	}
	else
		return(0);
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::setBodyPosition( vector3 * p )
{
	body->bodyObject->setPosition( p );


}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void car::moveBodyPosition( vector3 * p )
{
	body->bodyObject->movePosition( p );

	//body->transformedexhaustCloudPoint = body->bodyObject->transform.translation;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------


void car::moveBodyPositionInDirection( vector3 * p )
{
	static vector3 tempvector;

	
	tempvector.negate( &body->bodyObject->transform.translation );

	body->bodyObject->transform.multiplyIntoTransform(  p );

	if( (body->bodyObject->attached != 0) && ((body->bodyObject->attachedValue & ATTACH_FOLLOW)!=0) )
	{
		tempvector += body->bodyObject->transform.translation;
	
		body->bodyObject->attached->moveCameraPosition( &tempvector);
		
	};

	body->bodyObject->changed = true;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------


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
	body->bodyObject->setAngle( a );
}	

void car::setBodyAngleNOCAMERA( vector3 * a )
{
	body->bodyObject->setAngleNOCAMERA( a );
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
	body->bodyObject->moveAngle( a );
}


void car::moveBodyAngleNOCAMERA( vector3 * a )
{
	body->bodyObject->moveAngleNOCAMERA( a );
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
	c->attach( body->bodyObject,  aflags, attachPoint);
}

void car::attachCamera( uchar aflags, vector3 * attachPoint  ) // uses current camera
{
	if(boris2->currentcamera)
		boris2->currentcamera->attach( body->bodyObject,  aflags, attachPoint);
}

void car::insertIntoWorld()
{
	boris2->insertObject( body->bodyObject );		
}

void car::removeFromWorld()
{
	boris2->removeObject( body->bodyObject );
}


bool car::isInSpin()
{
	return( ( fabs(kick.x) >= 0.02f) || ( fabs(kick.y) >= 0.02f) || (fabs(kick.z) >= 0.02f) );

};

void car::CalculatePhysicalProperties()
{
	matrix3x4 * orientation = &body->bodyObject->transform;

	velocity = vector3(0,0,0);
	extrarotvel=extraforce=vector3(0,0,0);

	mass = 700;// TEST TEMP VALUE
	oneOverMass = RECIPROCAL( mass );
	braking = 0.0f;			

	acc = vector3(0,0,0);

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
  
  distanceToCollision = velocity;// * tdelta + acc * (0.5f * tdelta * tdelta);		

  moveBodyPositionInDirection( &distanceToCollision ) ;//&vector3(0.0f, 0.0f, (velocity.x * mid) ) );
  body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY

					// look for collisions... Lets add up the total as the sum of the body AND the four wheels.

  collided = isInCollision();

  
 			// recover the body position...

  distanceToCollision.x = -distanceToCollision.x;
  distanceToCollision.y = -distanceToCollision.y;
  distanceToCollision.z = -distanceToCollision.z;

  moveBodyPositionInDirection(  &distanceToCollision ); //&vector3(0.0f, 0.0f, -velocity.x * oldmid) );
  body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY

  return(collided);

};


//--------------------------------------------------------------------------------------------------------------------------------
// findTimeOfImpact
// ----------------
//				find the first point, within the next tick, at which the car collides by using a binary chop to move it.
//		return 'true' iff a collision occured, otherwise return false;
//--------------------------------------------------------------------------------------------------------------------------------



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



car::car( char * arcname, float carscale, spriteFX * exhaustFX )
{

	
  
	float radiusF, radiusB, oneoverradiusF, oneoverradiusB;

	float maxSpeed;



	zoomdone = true;
	collision = false;

	collisionSoundTimer.setTimer(2);
	skidSoundTimer.setTimer(3);

	SpinCam = 0.0f;
	SpinDelta = 0.0f;

	kick = kickDelta = vector3(0,0,0);

	arcPush(arcname);			// car



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


	ulong texhandle = arcExtract_txt( "description" );


	vector3 FwheelPosition, RwheelPosition,  driverPosition;


	bodyShadowObject =NULL;
	driverShadowObject =NULL;
	FwheelShadowObjectLEFT =NULL;
	FwheelShadowObjectRIGHT =NULL;
	RwheelShadowObjectLEFT =NULL;
	RwheelShadowObjectRIGHT =NULL;
	FaxleShadowObject =NULL;
	RaxleShadowObject = NULL;

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
				bodyModel  = getModelPtr( tempBuffer3, carscale, false, "");
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
				FwheelShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else
			if( strcmp( tempBuffer3,"FRONT_WHEEL_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RwheelShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else
			if( strcmp( tempBuffer3,"BODY_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				bodyShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else		
			if( strcmp( tempBuffer3,"FRONT_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FaxleShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else		
			if( strcmp( tempBuffer3,"REAR_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RaxleShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
			} else		
			if( strcmp( tempBuffer3,"DRIVER_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				driverShadowModel  = getModelPtr( tempBuffer3, carscale, false, "");
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

		FwheelModel->meshData.spheres =
		RwheelModel->meshData.spheres =
		FaxleModel->meshData.spheres = 
		RaxleModel->meshData.spheres = 
		driverModel->meshData.spheres =
		bodyModel->meshData.spheres = 0;

		if(  FwheelSSnameUsed )
			FwheelModel->meshData.spheres = new SphereSet(FwheelSSname, carscale);
		if(  RwheelSSnameUsed )
			RwheelModel->meshData.spheres = new SphereSet(RwheelSSname, carscale);
		if(  FaxleSSnameUsed )
			FaxleModel->meshData.spheres = new SphereSet(FaxleSSname, carscale);
		if( RaxleSSnameUsed  )
			RaxleModel->meshData.spheres = new SphereSet(RaxleSSname, carscale);
		if( driverSSnameUsed )
			driverModel->meshData.spheres = new SphereSet(driverSSname, carscale);
		if(  bodySSnameUsed  )
			bodyModel->meshData.spheres = new SphereSet(bodySSname, carscale);

				//
				// Build the car from the loaded components
				//

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


		bodyObject->insertChild( FwheelObjectLEFT );
		FwheelObjectLEFT->insertSister( FwheelObjectRIGHT );
		FwheelObjectRIGHT->insertSister( RwheelObjectLEFT );
		RwheelObjectLEFT->insertSister( RwheelObjectRIGHT );
		RwheelObjectRIGHT->insertSister( driverObject  );
		driverObject->insertSister( FaxleObject );
		FaxleObject->insertSister( RaxleObject );


		object * currentObject = FaxleObject;
		if(	bodyShadowModel )
		{
			bodyShadowObject = new object( bodyShadowModel,  0, false  );
			bodyShadowObject->setAngle( &vector3(0, 0, 0 ) );
			bodyShadowObject->setPosition( &vector3(0, 0, 0 ) );
			currentObject->insertSister( bodyShadowObject );
			currentObject = bodyShadowObject;
		};

		radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		oneoverradiusF = RECIPROCAL( radiusF );
		oneoverradiusB = RECIPROCAL( radiusB );

				//
				// --- the wheel radius is taken as half the y-distance of the bounding box since
				//	the model of the wheel is 'standing' 
				//

		radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		oneoverradiusF = RECIPROCAL( radiusF );
		oneoverradiusB = RECIPROCAL( radiusB );

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


	    totalWheels = 4;

			  bodyObject->inheritTransformations();		// initially create all concatenatedTransformations


			  setPowerPoint(0,	0,0.01);

			  setPowerPoint(1,	100,0.2); // was 500
			  setPowerPoint(2,	1000,0.7);
			  setPowerPoint(3,	2000,1);
			  setPowerPoint(4,	4000,0.7);
			  setPowerPoint(5,	6000,0);

				// Define engine gears

			  setGearData( 0,	0.0f,0  );	// OUT OF GEAR ... initial revving ?
			  setGearData( 1,  0.14f,	1);
			  setGearData( 2,  0.4f,	1500);
			  setGearData( 3,  0.7f,	4000);
			  setGearData( 4,  1.0f,	6000);
		
					// Define engine gears
		
			  //setGearData( 0,   0.0f,  25.0f, 0.0f  );	// OUT OF GEAR ... initial revving ?
			  //setGearData( 1,  20.0f, 155.0f, 0.2f / 3200.0f);
			  //setGearData( 2,  20.0f, 155.0f, 0.37f / 3200.0f);
			  //setGearData( 3,  20.0f, 155.0f, 0.74f / 3200.0f);
			  //setGearData( 4,  20.0f, 155.0f, 1.00f / 3200.0f);
			  
					// Initially set the car out of gear

			  changeGear( 0 );

					// Now add in the car exhaust Sprite Effect

			  exhaustSpriteFX = exhaustFX;
			  exhaustSpriteTimer.setTimer(9);		// Test of 3 times per second (should be prop. to revs
			  exhaustMaxSequences = 2;				// Maximum number of exhaust sprite sequences		
			  exhaustUsedSequences = 0;
			  body->exhaustCloudPoint = vector3(-0.25,-0.1,-2);
			 

		arcDispose( texhandle );

		CalculatePhysicalProperties();

	};

	damped_throttle = 0;

	maxVelocity		=  maxSpeed;
	//acceleration = 0.0f;
	revs = 0.0f;
	localVelocity.x = localVelocity.y = localVelocity.z=
		velocity.x = velocity.y = velocity.z  = 0.0f;

	gearCount =  4;		//  1..gearCount = legal gears ( 0 = out of gear )
	maxrevs = 7000;
	prevpoint = 0;
	previousGearNumber=0;



	previousTransRevs=0;

	wheelTurnMatrix.setmat( 1,0,0,  0,1,0,  0,0,1, 0,0,0 );
	//wheelTurnMatrix2.setmat( 1,0,0,  0,1,0,  0,0,1, 0,0,0 );

	totalcarturnangle = 0.0f;

	arcPop();
}


float car::getpower(float revs)
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
					if(points[prevpoint*2]<=revs && revs<points[prevpoint*2+2])
						break;
				}
			}
		}
	}

	// ok, prevpoint is the beginning of the line segment we want, now we need to lerp.

	float rev1 = points[prevpoint*2];
	float rev2 = points[prevpoint*2+2];

	float val1 = points[prevpoint*2+1];
	float val2 = points[prevpoint*2+3];

	float q = (revs - rev1)/(rev2-rev1);

	return q*(val2-val1)+val1;
}



void car::changeGear( ulong gearNumber )
{
	if( gearNumber > gearCount) 
		gearNumber = gearCount;

	currentGearPtr = &gear[gearNumber];	
	currentGearNumber = gearNumber;
};

void car::shiftUpGear()
{

	if( currentGearNumber < gearCount )
	{
		//currentGearPtr++;
		currentGearNumber++;
		currentGearPtr = &gear[currentGearNumber];
	}
};

void car::shiftDownGear()
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
