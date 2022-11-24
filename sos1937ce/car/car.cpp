// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:29:18+00  jjs
// First version that supports the loading of objects.
//
// Revision 1.0  1999-12-06 14:02:28+00  jjs
// Initial revision
//
#define D3D_OVERLOADS

#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "ALLcar.h"

char tempBuffer2[128];	
char tempBuffer3[128];	

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
}

void car::readOffVector( vector3 * v, char ** strPtr )
{
	readOff( tempBuffer3, strPtr, ',');
	v->x = (float)atof(tempBuffer3);
	readOff( tempBuffer3, strPtr, ',');
	v->y = (float)atof(tempBuffer3);
	readOff( tempBuffer3, strPtr, 0);
	v->z = (float)atof(tempBuffer3);
}

void car::setBodyPosition( vector3 * p )
{
	mainObject->setPosition( p );
}

void car::LoadModel(char * arcname, float carscale)
{
	float radiusF, radiusB, oneoverradiusF, oneoverradiusB;

	arcPush(arcname);			// car
	ulong texhandle = arcExtract_txt( "description" );
	

	//
	// Load in the 'decription.txt' file from the archive
	//

	model * FwheelModel, * RwheelModel, * FaxleModel, * RaxleModel, * bodyModel, * driverModel;
	model * FwheelShadowModel, * RwheelShadowModel, * FaxleShadowModel, * RaxleShadowModel, * bodyShadowModel;
	model * driverShadowModel;

	FwheelModel = RwheelModel = FaxleModel = RaxleModel = bodyModel = driverModel = 
	FwheelShadowModel = RwheelShadowModel = FaxleShadowModel = RaxleShadowModel = bodyShadowModel = 
	FBlurMedModel = FBlurHiModel = RBlurMedModel = RBlurHiModel = driverShadowModel = 0;


	bool  FwheelSSnameUsed, RwheelSSnameUsed, driverSSnameUsed,
		  bodySSnameUsed, FaxleSSnameUsed, RaxleSSnameUsed;

	 FwheelSSnameUsed = RwheelSSnameUsed = driverSSnameUsed = 
		  bodySSnameUsed = FaxleSSnameUsed = RaxleSSnameUsed  = false;



	vector3 FwheelPosition, RwheelPosition,  driverPosition;

	driverPosition=vector3(0.0f,0.0f,0.0f);

	bodyShadowObject = driverShadowObject = FwheelShadowObjectLEFT = FwheelShadowObjectRIGHT =
	RwheelShadowObjectLEFT = RwheelShadowObjectRIGHT = FaxleShadowObject = RaxleShadowObject = 0;

#define SHADOWSCALE 1.1f

//	sound = _id % 3;

	if( texhandle ) 
	{

		while(  arcGets( tempBuffer2, 128, texhandle ) )
		{

			char * strPtr = tempBuffer2;

			readOff( tempBuffer3, &strPtr, ':' );


// you really don't want to know.

#define	EMPTYSTRING	NULL

			if( strcmp( tempBuffer3,"FRONT_WHEEL" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FwheelModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
				readOffVector( &FwheelPosition, &strPtr );
				FwheelPosition *= carscale;

			} else
			if( strcmp( tempBuffer3,"REAR_WHEEL" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RwheelModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
				readOffVector( &RwheelPosition, &strPtr );
				RwheelPosition *= carscale;

			} else
			if( strcmp( tempBuffer3,"BODY" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				// now loads as a unique
				bodyModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else
			if( strcmp( tempBuffer3,"FRONT_AXLE" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				// now loads as a unique
				FaxleModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);	
			} else
			if( strcmp( tempBuffer3,"REAR_AXLE" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				// now loads as a unique
				RaxleModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else

			if( strcmp( tempBuffer3,"FWHEEL_BLUR_MED" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				
				FBlurMedModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else

			if( strcmp( tempBuffer3,"FWHEEL_BLUR_HI" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				
				FBlurHiModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else

			if( strcmp( tempBuffer3,"RWHEEL_BLUR_MED" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				
				RBlurMedModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else

			if( strcmp( tempBuffer3,"RWHEEL_BLUR_HI" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				
				RBlurHiModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			} else

			if( strcmp( tempBuffer3,"DRIVER" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				driverModel  = getModelPtr( tempBuffer3, carscale, false, EMPTYSTRING);
			}
#if 0			
			else
			if( load_shadows && strcmp( tempBuffer3,"FRONT_WHEEL_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FwheelShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
				RwheelShadowModel = FwheelShadowModel;
//DBGR.Dprintf("FW SHADOW\n");
			} else
			if( load_shadows && strcmp( tempBuffer3,"REAR_WHEEL_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				//RwheelShadowModel  = .getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
//DBGR.Dprintf("RW SHADOW\n");

			} else
			if( load_shadows && strcmp( tempBuffer3,"BODY_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				bodyShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
				dprintf("numspheres : %lx",bodyShadowModel->meshData->spheres);
			} else		
			if( load_shadows && strcmp( tempBuffer3,"FRONT_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				FaxleShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
			} else		
			if( load_shadows && strcmp( tempBuffer3,"REAR_AXLE_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				RaxleShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
			} else		
			if( load_shadows && strcmp( tempBuffer3,"DRIVER_SHADOW" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, ',' );
				//driverShadowModel  = getModelPtr( tempBuffer3, carscale * SHADOWSCALE, false, EMPTYSTRING);
			}
			else
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
			if( strcmp( tempBuffer3,"SOUND" ) == 0)
			{
				readOff( tempBuffer3, &strPtr, 0 );
				sound = atoi(tempBuffer3);
			}
#endif
		}

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

#if 0
		object * currentObject = FaxleObject;
		if(	bodyShadowModel )
		{
			bodyShadowObject = new object( bodyShadowModel,  0, false  );
			bodyShadowObject->setAngle( &vector3(0, 0, 0 ) );
			bodyShadowObject->setPosition( &vector3(0, 0, 0 ) );
			currentObject->insertSister( bodyShadowObject );
			currentObject = bodyShadowObject;
		}

		if(	FaxleShadowModel )
		{
			FaxleShadowObject = new object( FaxleShadowModel,  0, false  );
			FaxleShadowObject->setAngle( &vector3(0, 0, 0 ) );
			FaxleShadowObject->setPosition( &vector3(0, 0.0f, 0 ) );
			currentObject->insertSister( FaxleShadowObject );
			currentObject = FaxleShadowObject;
		}

		if(	RaxleShadowModel )
		{
			RaxleShadowObject = new object( RaxleShadowModel,  0, false  );
			RaxleShadowObject->setAngle( &vector3(0, 0, 0 ) );
			RaxleShadowObject->setPosition( &vector3(0, 0.0f, 0 ) );
			currentObject->insertSister( RaxleShadowObject );
			currentObject = RaxleShadowObject;
		}

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
		}

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
		}
#endif

		radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		oneoverradiusF = 1.0f / radiusF ;
		oneoverradiusB = 1.0f / radiusB ;

				//
				// --- the wheel radius is taken as half the y-distance of the bounding box since
				//	the model of the wheel is 'standing' 
				//

		//radiusF = ( FwheelModel->boundingBox.hi.y - FwheelModel->boundingBox.lo.y ) * 0.5f;
		//radiusB = ( RwheelModel->boundingBox.hi.y - RwheelModel->boundingBox.lo.y ) * 0.5f;
		//oneoverradiusF = RECIPROCAL( radiusF );
		//oneoverradiusB = RECIPROCAL( radiusB );

		wheel[0] = new carwheel(FwheelObjectRIGHT); 
		wheel[0]->attached = true;
		wheel[0]->radius = radiusF;
  		wheel[0]->oneoverradius = oneoverradiusF;
		wheel[0]->pos = wheel[0]->wheelObject->transform.translation;
		wheel[0]->c = this;

    	wheel[1] = new carwheel(FwheelObjectLEFT); 
		wheel[1]->attached = true;
		wheel[1]->radius = radiusF;
  		wheel[1]->oneoverradius = oneoverradiusF;
		wheel[1]->pos = wheel[1]->wheelObject->transform.translation;
		wheel[1]->c = this;

		wheel[2] = new carwheel(RwheelObjectRIGHT); 
		wheel[2]->attached = true;
		wheel[2]->radius = radiusB;
  		wheel[2]->oneoverradius = oneoverradiusB;
		wheel[2]->pos = wheel[2]->wheelObject->transform.translation;
		wheel[2]->c = this;

		wheel[3] = new carwheel(RwheelObjectLEFT); 
		wheel[3]->attached = true;
		wheel[3]->radius = radiusB;
  		wheel[3]->oneoverradius = oneoverradiusB;
		wheel[3]->pos = wheel[3]->wheelObject->transform.translation;
		wheel[3]->c = this;

#if 0
		wheelShadow[0] =
		wheelShadow[1] =
		wheelShadow[2] =
		wheelShadow[3] = 0;//FwheelObjectLEFT;

		wheelrotateoffset[0]=
		wheelrotateoffset[1]=
		wheelrotateoffset[2]=
		wheelrotateoffset[3]=0.0;
#endif

	    totalWheels = 4;

			
		mainObject->inheritTransformations();		// initially create all concatenatedTransformations
#if 0
		exhaustSpriteFX = exhaustFX;
		exhaustSpriteTimer.setTimer(20);		// Test of 3 times per second (should be prop. to revs
		exhaustMaxSequences = 12;				// Maximum number of exhaust sprite sequences		
		exhaustUsedSequences = 0;
		body->exhaustCloudPoint = vector3(-0.25,-0.1,-2);
#endif			 

		arcDispose( texhandle );

//		CalculatePhysicalProperties();

	}

	arcPop();
}


car::car( char * arcname, float carscale, /*spriteFX * exhaustFX,*/ int _id)
{
	LoadModel(arcname, carscale); 

}
