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


// JCF engine constants

#define	THROTTLE_DAMPING	0.96f


extern engine * boris2;




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


void car::changeGear( ulong gearNumber )
{
	engine->changeGear( gearNumber );
};

void car::shiftUpGear()
{
	engine->shiftUpGear();
};

void car::shiftDownGear()
{
	engine->shiftDownGear();
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


extern float lofter;

void car::plonkOnGround()
{
	
	float deltaW, deltaB, bodyY;
	triangle * bodytrianglePtr, *trianglePtr0, *trianglePtr1, *trianglePtr2, *trianglePtr3;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3;
	
#define DELTAANGLE 0.002f
	
	
	//setBodyAngle( &( vector3(0, cycles * localTurn, 0) + body->bodyObject->angle) );//angle ); 
	//moveBodyPositionInDirection( &vector3(0.0f, 0.0f, engine->velocity.z) );
	
	//

//body->bodyObject->transform.set3x3submat(1,0,0, 0,1,0, 0,0,1);
//body->bodyObject->changed   = true;	

	body->bodyObject->inheritTransformations();	// update geometry for this object tree ONLY (car object tree)


	float min, max;
//	long iii;23
//	for(iii = 2;iii--;)
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

		tempm.multiply3x3submats( &body->bodyObject->transform, &temp1 );			
		temp1 = tempm;
	
		temp1.translation = body->bodyObject->transform.translation;
		
		body->bodyObject->transform = temp1;

		body->bodyObject->changed   = true;	

		minmax4( heightvalue0, heightvalue1, heightvalue2, heightvalue3, &min, &max );
		
		max = (max+min)*0.5f;
		
		setBodyPosition_Y(  max  );//+ 0.25f);			// Note 'changed' flag set ready for new resolve of geometry

		body->bodyObject->inheritTransformations();

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

//		body->bodyObject->inheritTransformations();
};

//setBodyPosition_Y(  max + 0.2f + wheel[0]->radius);
};

extern void dprintf(char *,...);

void car::move(ulong ticks, float turn, float throttle, float  brake,  ulong cycles )
{
	triangle * trianglePtr0, * trianglePtr1, * trianglePtr2, * trianglePtr3, * bodytrianglePtr;
	float heightvalue0, heightvalue1, heightvalue2, heightvalue3, heightsum, bodyY, minY;
	float oneoverphysicsTicks = boris2->oneoverphysicsTicks;
	float localTurn;
	bool engaged;

#define	REVSTOVELOCITY	(6000.0	* 200 * boris2->oneoverphysicsTicks )// factor to divide revs by to get velocity (i.e. final drive function)
#define ONEOVERREVSTOVELOCITY  ( 1.0f / REVSTOVELOCITY )
#define	AUTOCLUTCHLIMIT	150	// revs at which the autoclutch engages

	float timeElapsed = oneoverphysicsTicks * (float)ticks ;

	grdebug("time",timeElapsed);

	engaged = engine->currentGearNumber && (engine->revs<0 ? -engine->revs : engine->revs)>AUTOCLUTCHLIMIT;

	// now we get the wheel revs back from the velocity

	transrevs = engine->localVelocity.z*REVSTOVELOCITY;
	grdebug("postvel",transrevs);
//	transrevs /= wheel[0]->oneoverradius;
	grdebug("P-transrevs",transrevs);

	grdebug("P-engrevs",engine->revs);

	if(engaged)
	{
		// the engine only slows down if the geared transmission revs are close to the engine revs.

		float ttt = invgearbox(transrevs);
		float gap = engine->revs - ttt;
		if(gap<0)gap=-gap;
		if(engine->previousGearNumber)
		{
				if(gap > 500)
					engine->revs = invgearbox(transrevs);
		}
		else
		{
			engine->revs = engine->revs/2;
		}
	}
if( engine->revs < 0.0f )  engine->revs = 0.0f;


//dprintf("post-trans: %f, post-engine: %f\n",transrevs,engine->revs);
	
	localTurn = turn * timeElapsed * 13;
 
	// JCF - new drive train stuff

	// damp throttle value

	damped_throttle = damped_throttle*(1-THROTTLE_DAMPING) + throttle*THROTTLE_DAMPING; // change to logs?
	grdebug("throttle",damped_throttle);
	grdebug("brake",brake);


	// add the 'kick' of the throttle to the engine revs.
	// NB - change this later so that if revs<n it only works sometimes, so
	// we can get the engine to splutter, kangaroo and stall :))

	// first, we use the power curve for this gear. Each gear has a curve of
	// power against revs - the throttle value is multiplied by this.

	float powr = engaged ? engine->getpower(engine->revs) : 3;
	grdebug("power",powr);

	engine->revs += damped_throttle*700*powr*timeElapsed;

	grdebug("pre-eng",engine->revs);

	// allow for engine friction, which is greater at low revs

	// revs from 0-5000

	float engine_friction;
	static float flimits[5]={0.99,0.99,0.99,0.99,0.99};
	static float fvalues[5]={0,1,1500,3500,4500}; // the higher the value, the higher the friction

	float friction_limit = flimits[engine->currentGearNumber];


	if(damped_throttle>0.8)
	{
		engine_friction = engine->currentGearNumber ? 0.6+engine->revs/(fvalues[
				engine->currentGearNumber]*engine->getRatio()) : 0.95;
		if(engine_friction>friction_limit)engine_friction=friction_limit;
	}
	else
	{
		engine_friction = 0.99;
	}

	if(timeElapsed)
	{
		engine->revs *=( engine_friction < 0.0f ) ?	
			-pow(-engine_friction,timeElapsed*10) : 
			 pow(engine_friction,timeElapsed*10);
	};


	if(engine->revs>7000)engine->revs=7000;

	grdebug("friction",engine_friction);

	// apply the gearbox function to get the transmission revs

	if(engaged)transrevs = gearbox(engine->revs);

	grdebug("transrevs",transrevs);

//dprintf("gear: %d, engine(pre acc): %f, engine(post acc): %f, transmission: %f, ",engine->currentGearNumber,
//engine->revs-damped_throttle*40,engine->revs,transrevs);
	


	// convert engine revs into car velocity

	brake *= pow(0.01,timeElapsed*10);

	transrevs *= 1.0-brake;

	engine->localVelocity.z = transrevs * ONEOVERREVSTOVELOCITY;

//engine->localVelocity.x = 0;
engine->localVelocity.y *= 0.9f;

	// transform car velocity to world space

//#define CRITICALVELOCITY 2.2f
//if(engine->localVelocity.z < CRITICALVELOCITY)
//engine->localVelocity.x = 0.01f; //( engine->localVelocity.z) / CRITICALVELOCITY;

	body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &engine->velocity, &engine->localVelocity );




//	moveBodyAngle( &vector3(0,0,0) );
//body->bodyObject->changed = true;

	extrarotvel.x = 0;
	extrarotvel.y = -localTurn*0.1f;//3f;
	extrarotvel.z = 0;

	turnY += extrarotvel.y* 0.8f;

//	turnY += kick.y * 100;

	if(turnY > (PI * 0.07f))
		turnY = (PI * 0.07f);
	if(turnY < -(PI * 0.07f))
		turnY = -(PI * 0.07f);

	float k = (  (engine->localVelocity.z - engine->localVelocity.x)  / 25.0f);
	if(k < 0) k = 0;
	else
		if(k > 1.0f) k = 1.0f;

	if (localTurn== 0) turnY *= (1.0f - k);//0.9f;

//moveBodyAngle( &extrarotvel) ;	// TEMP

	grdebug("prevel",engine->localVelocity.z);

	doPhysics(ticks);
	grdebug("","");
	engine->previousGearNumber = engine->currentGearNumber;

}

float car::getEngineRevs()
{
	return engine->revs;
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
	delete engine;
	for(i = 0; i<4; i++)
		delete wheel[i];

	//body->~bodyObject();			// Remove the geometry of the car 
}



//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

long car::getGear()
{
	return( engine->currentGearNumber );
};



//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

float car::gearbox(float revs)	// jcf 
{
	float gratio = engine->currentGearPtr->ratio;

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
	float gratio = engine->currentGearPtr->ratio;

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

//--------------------------------------------------------------------------------------------------------------------------------
// doPhysics
// ---------
//			Perform physics 
//--------------------------------------------------------------------------------------------------------------------------------


#define TRIM(a)  if( a.x > 

#define GRAVITY 0.10f

extern vector3 collvector;
extern vector3 collpoint;
#define MINIMUMTOI	0.1f
#define	TIME_DIVISOR	10

void car::doPhysics(ulong tickTotal)
{


	float timeleft;
	float interval;
	
	static vector3 totalforce;
	static vector3 v, v1;
	
	static vector3 damping, dampVector;
	static vector3 tempf;
	static matrix3x4 m;
	
	ulong i;
	
	collision = false;
	float time_factor = tickTotal;


	if(tickTotal)	
	{

		#define WANGLE(a)	(engine->localVelocity.z  * wheel[a]->oneoverradius)
		
		static vector3 a;
		float qqq = (55*extrarotvel.y)/time_factor;
		
		tickTotal /= TIME_DIVISOR;if(!tickTotal)tickTotal=1;

		getWheelAngle(0, &a );
		a.y = qqq;
		setWheelAngle(0, &(a + vector3( WANGLE(0), 0, 0 ) ) );
		
		getWheelAngle(1, &a );
		a.y = qqq;;
		setWheelAngle(1, &(a + vector3( WANGLE(1), 0, 0 ) ) );
		
		getWheelAngle(2, &a );
		setWheelAngle(2, &(a + vector3( WANGLE(2), 0, 0 ) ) );
		
		getWheelAngle(3, &a );
		setWheelAngle(3, &(a + vector3( WANGLE(3), 0, 0 ) ) );
		
		if(tickTotal > 2) tickTotal = 2;
		
		moveBodyAngle( &vector3(0,0,0) ) ;

//static matrix3x4 ttt = body->bodyObject->transform;

		for(i=tickTotal;i--;)		// perform each physics tick
		{
			timeleft = boris2->oneoverphysicsTicks;// time left in this tick
			
			totalforce = extraforce;			// Forces in world space
			
			totalforce.y += GRAVITY;

			//
			// Now we must turn the 'totalforce' from world space into the local objects space
			// and damp the local 'x' component to simulate heavily damped sideways movement.
			// Then turn the force back into world space (phew!!)
			//
			
			body->bodyObject->inheritTransformations();
			m.Transpose3x4(&body->bodyObject->concatenatedTransform); //Transpose3x4( &m );
			
			
			// Reduce force in 'x'
			
			m.multiplyV3By3x3mat( &tempf, &totalforce );
		//	tempf.x *= 0;//.001f;//0;//.2f;

			body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &totalforce, &tempf );
			

			ulong ctest = 0;
			while(isInCollision() )
			{
				
				// Attempt to recover from collision by moving the car gently out along the 
				// collision surface normal
				
				object * whichObject;
				
				v1 = collvector * 1.5f;
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
				
				v = engine->velocity * time_factor;
				body->bodyObject->movePosition( &v);	// updates the position for this interval
				body->bodyObject->inheritTransformations();
				
				v = engine->velocity ;
				v.NormaliseWithCheckAndMag(&mag);
				v1 = v - collvector * ( 2.0f * collvector.DotProduct(&v) ) ;
				
				
				static vector3 cp;
				
				
				cp = v * collvector;
				
				kick.x = 0;//cp.x * 0.1f;
				kick.z = 0;//cp.z * 0.1f;
				kick.y = -(0.7f - cp.y) *  0.21f ;
				
				kick.y *= fsqrt(fsqrt(fabs(engine->localVelocity.z)  * 0.04f)); /// 25.0f));
				
				engine->velocity = v1 * mag * 0.86f;
				
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

				v = vector3(engine->velocity.x,0,engine->velocity.z) * time_factor;

				body->bodyObject->movePosition( &v );
				acc = totalforce * oneOverMass;		// F = ma  ....  a = F / m
				v1 = acc * interval;

		//	v1.y = 0;
		//	engine->velocity += v1;

				engine->localVelocity.x *= 0.01f;
				engine->localVelocity.y *= -0.01f;
				
				
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
			
			m.multiplyV3By3x3mat( &engine->localVelocity, &engine->velocity );
			engine->localVelocity.x *= 0.95f;		// was 0.68
			float b = engine->braking;
			if( b < (1.0f-0.3f)) b += 0.3f;
			engine->localVelocity.z *= (1.0f - b*0.01f);
			
			
			if( engine->localVelocity.z > 25.0f ) engine->localVelocity.z = 25.0f;		// 25 * 6 = 150
			
			body->bodyObject->concatenatedTransform.multiplyV3By3x3mat( &engine->velocity, &engine->localVelocity );
			
		};

		
	moveBodyAngle( &kick );
	moveBodyAngle( &extrarotvel) ;

	//body->bodyObject->transform.translation = ttt;

	
	kick.x *= kickDelta.x;
	kick.y *= kickDelta.y;
	kick.z *= kickDelta.z;
		
	//	body->bodyObject->changed = true;
		body->bodyObject->inheritTransformations();
		plonkOnGround();
		body->bodyObject->inheritTransformations();
	}
	else
		body->bodyObject->inheritTransformations();
}


void car::CalculatePhysicalProperties()
{
	matrix3x4 * orientation = &body->bodyObject->transform;

	engine->velocity = vector3(0,0,0);
	extrarotvel=extraforce=totaltorque=vector3(0,0,0);

	mass = 700;// TEST TEMP VALUE
	oneOverMass = RECIPROCAL( mass );
	engine->braking = 0.0f;			

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
	turnY = 0;

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
//bodyObject->attachmentPoint = vector3(0, 110.5f, 0 );

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

			
 
			  engine   = new carengine( maxSpeed, 4 ); 
		
			  bodyObject->inheritTransformations();		// initially create all concatenatedTransformations


			  engine->setPowerPoint(0,	0,0.01);

			  engine->setPowerPoint(1,	100,0.2); // was 500
			  engine->setPowerPoint(2,	1000,0.7);
			  engine->setPowerPoint(3,	2000,1);
			  engine->setPowerPoint(4,	4000,0.7);
			  engine->setPowerPoint(5,	6000,0);

				// Define engine gears

			  engine->setGearRatio( 0,	0.0f  );	// OUT OF GEAR ... initial revving ?

			  engine->setGearRatio( 1,  0.14f);
			  engine->setGearRatio( 2,  0.4f);
			  engine->setGearRatio( 3,  0.7f);
			  engine->setGearRatio( 4,  1.0f);
		
					// Define engine gears
		
			  //engine->setGearData( 0,   0.0f,  25.0f, 0.0f  );	// OUT OF GEAR ... initial revving ?
			  //engine->setGearData( 1,  20.0f, 155.0f, 0.2f / 3200.0f);
			  //engine->setGearData( 2,  20.0f, 155.0f, 0.37f / 3200.0f);
			  //engine->setGearData( 3,  20.0f, 155.0f, 0.74f / 3200.0f);
			  //engine->setGearData( 4,  20.0f, 155.0f, 1.00f / 3200.0f);
			  
					// Initially set the car out of gear

			  engine->changeGear( 0 );

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

	arcPop();
}







//================================================================================================================================
// END OF FILE
//================================================================================================================================
