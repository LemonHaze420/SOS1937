//================================================================================================================================
// object.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"


object::object( model * modelPtr, uchar cflags, bool defincam )
{
	modelData = modelPtr;

	collisionFlags = cflags;

	definedInCameraSpace = defincam;

	transform.translation.x = transform.translation.y = transform.translation.z =
	concatenatedTransform.translation.x = concatenatedTransform.translation.y = concatenatedTransform.translation.z = 0.0f;
	attachmentPoint.x = attachmentPoint.y = attachmentPoint.z =
	transformedattachmentPoint.x = transformedattachmentPoint.y = transformedattachmentPoint.z = 0.0f;

	attached = 0;		// when created, no camera is attached

	sister = NULL;
	child  = NULL;
	prevsister = NULL;
	parent = NULL;
	prev =  NULL;		// initially not attached to list
	next = NULL;
	transformedSpheres = NULL;
	avoidOcclusion = false;

	if(modelPtr && modelPtr->meshData)
	{
		mesh *lodmesh = modelPtr ->meshData + FIRSTLOD;	// only spheres in the zeroth mesh are used for collision

		if( lodmesh->spheres )	
		{
			transformedSpheres = new SphereSet;
	dprintf("num %lx",lodmesh->spheres->num);
			transformedSpheres->data = new Sphere[lodmesh->spheres->num];
		}

		turningPoint = vector3(0,0,0);
	}
}

//-----------------------------------------------------
// deleteObjectTree	- INTERNAL ROUTINE SINCE DOES NOT
// -----------------  DISCONNECT FROM ENGINE CHAIN 
//
//-----------------------------------------------------

void object::deleteObjectTree()
{
	object * oldoptr;
	object * optr = this;
	while( optr )
	{
		if( optr->child )
			optr->child->deleteObjectTree();
		
		oldoptr = optr;
		
		delete optr->modelData;		// destroy model
		
		optr = optr->sister;
		
		delete oldoptr;
	}
}


//-----------------------------------------------------
// deleteObjectTreeAndDisconnect
// -----------------------------
//		Delete the object tree nodes and disconnect the
//	parent node from the engine chain if it is connected
//
//-----------------------------------------------------

void object::deleteObjectTreeAndDisconnect()
{
	object * oldoptr;
	object * optr = this;
	while( optr )
	{

				// must disconnect from engine chain IF RELEVANT
	
		 boris2->removeObject( optr );	// disconnects, does NOT delete it!
		 
		 oldoptr = optr;

	     optr->deleteObjectTree();

	     optr = oldoptr->sister;
	}
}

//-----------------------------------------------------
// destroy object node
//-----------------------------------------------------

object::~object()
{
};

//-----------------------------------------------------
//setAllAngles - internal routine
//----------------------------------------------------

void object::setAllAngles( matrix3x4 * resultmatrix )
{

	angle.rationaliseAngleVector();

	cosAngle.x = mySineAndCosine( angle.x, &sinAngle.x );
	cosAngle.y = mySineAndCosine( angle.y, &sinAngle.y );
	cosAngle.z = mySineAndCosine( angle.z, &sinAngle.z );
	
	RotX.set3x3submat(	1,   0,   0,  
						0,  cosAngle.x,  sinAngle.x,  
						0,  -sinAngle.x,  cosAngle.x  
					 );

	RotY.set3x3submat(	cosAngle.y, 0, -sinAngle.y,
						0,  1, 0, 
						sinAngle.y, 0, cosAngle.y
					 );

	RotZ.set3x3submat( 	cosAngle.z,  sinAngle.z, 0, 
						-sinAngle.z, cosAngle.z, 0, 
						0, 0, 1
					  );

			// order of rot...  Z,X,Y

	RotXRotZ.multiply3x3submats( &RotX, &RotZ );
	resultmatrix->multiply3x3submats( &RotY,  &RotXRotZ);			
}

//-----------------------------------------------------------------------------------------------------------------
// setAngle
// --------
//				set every local angle of an object.
//-----------------------------------------------------------------------------------------------------------------

void object::setAngle( vector3 * a )
{
	static vector3 tempvector;

	tempvector = (*a) - angle;
	angle = *a;
	setAllAngles( &transform );

	changed = true;
}

void object::setAngleNOCAMERA( vector3 * a )
{
	static vector3 tempvector;

	tempvector = (*a) - angle;
	angle = *a;
	setAllAngles( &transform );

	changed = true;
}

void object::setAngle_X( float a )
{
}

void object::setAngle_Y( float a )
{
}

void object::setAngle_Z( float a )
{
}

void object::moveAngleFromLocalPosition( vector3 * a , vector3 * p )
{
}

//-----------------------------------------------------------------------------------------------------------------
// inheritTransformationsT
// -----------------------
//
//-----------------------------------------------------------------------------------------------------------------

void   object::inheritTransformationsT(  matrix3x4 * parentTransform )
{
	object * optr = this;
	
	while( optr )
	{
		optr->changed = false;	
		
		optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedattachmentPoint, &optr->attachmentPoint );
		if(optr->modelData)
		{
			optr->concatenatedTransform.multiply3x4mats( parentTransform, &optr->transform );
			
			optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
			optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedCentroid, &optr->modelData->centroid );
			
			// only the zeroth mesh has collision spheres
			if( optr->modelData->meshData->spheres )
				optr->modelData->meshData->spheres->transformSpheres( optr->transformedSpheres, &optr->concatenatedTransform ); 
			
		}
		if( optr->child)
			optr->child->inheritTransformationsT( &optr->concatenatedTransform );
		
		optr = optr->sister;
	}
}

//-----------------------------------------------------------------------------------------------------------------
// inheritTransformations
// ----------------------
//
//-----------------------------------------------------------------------------------------------------------------


void   object::inheritTransformations(  matrix3x4 * parentTransform )
{
	object * optr = this;
	
	while( optr )
	{
		if( optr->changed )	// if changed at this node then from here on we must ALWAYS concatenate transforms
		{
			optr->changed = false;
			
			optr->concatenatedTransform.multiply3x4mats( parentTransform, &optr->transform );
			
			optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedattachmentPoint, &optr->attachmentPoint );
			if(optr->modelData)
			{
				optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
				optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedCentroid, &optr->modelData->centroid );
				
				if( optr->modelData->meshData->spheres )
					optr->modelData->meshData->spheres->transformSpheres( optr->transformedSpheres, &optr->concatenatedTransform); 
				
			}
			if( optr->child )
				optr->child->inheritTransformationsT( &optr->concatenatedTransform );
		}	
		else
		{
			if( optr->child )
				optr->child->inheritTransformations( &optr->concatenatedTransform );
		}
		
		optr = optr->sister;
	}
}

//-----------------------------------------------------------------------------------------------------------------
// inheritTransformations
// ----------------------
//
//-----------------------------------------------------------------------------------------------------------------

void   object::inheritTransformations()
{
	object * optr = this;
	
	while( optr )
	{
		if( optr->changed)	// if changed at this node then from here on we must ALWAYS concatenate transforms
		{
			optr->changed = false;
			optr->concatenatedTransform = optr->transform;
			optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedattachmentPoint, &optr->attachmentPoint );
			
			if(optr->modelData)
			{
				optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
				optr->concatenatedTransform.multiplyV3By3x4mat(&optr->transformedCentroid, &optr->modelData->centroid );
				
				if( optr->modelData->meshData->spheres )
				{
					optr->modelData->meshData->spheres->transformSpheres( optr->transformedSpheres, &optr->concatenatedTransform ); 
				}
				
			}
			if( optr->child )
				optr->child->inheritTransformationsT( &optr->concatenatedTransform );
		}	
		else
		{
			if( optr->child )
				optr->child->inheritTransformations( &optr->concatenatedTransform );
		}
		
		optr = optr->sister;
	}
}


void object::insertChild( object * childObject )
{
	childObject->child = child;
	if( child )
		child->parent = childObject;
	childObject->parent = this;
	child = childObject;
}

void object::insertSister( object * sisterObject )
{
	sisterObject->sister = sister;
	if( sister )
		sister->prevsister = sisterObject;
	sisterObject->prevsister = this;
	sister = sisterObject;
}

void object::detachSister()
{
	if( prevsister )
		prevsister->sister = sister;		
	if( sister )
		sister->prevsister = prevsister;
}

void object::detachChild()
{
	if( parent )
		parent->child = child;					
	if( child )
		child->parent = parent;
}

//-----------------------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------------------

void object::tiltToNewUpVector( vector3 * upVector )
{
	// The default UP vector is (0,1,0)
	// so we assume this has moved to 'upVector'
	
	static vector3 t;
	static vector3 tz;
	static vector3 tx;
	static matrix3x4 newMatrix;
	static matrix3x4 newMatrix1;
				
	upVector->Normalise();
				
	t.x = 0.0f; 
	t.y = 1.0f; 
	t.z = 0.0f;
				
	tz.x = upVector->y * t.z - upVector->z * t.y;
	tz.y = upVector->z * t.x - upVector->x * t.z;
	tz.z = upVector->x * t.y - upVector->y * t.x;
				
	tz.Normalise();
				
	tx.x = upVector->y * tz.z - upVector->z * tz.y;
	tx.y = upVector->z * tz.x - upVector->x * tz.z;
	tx.z = upVector->x * tz.y - upVector->y * tz.x;
				
	tx.Normalise();
				
	newMatrix.set3x3submat( tx.x, upVector->x, tz.x,
				tx.y, upVector->y, tz.y,
				tx.z, upVector->z, tz.z );
				
	newMatrix1.multiply3x4mats( &transform, &newMatrix );
				
	transform = newMatrix1;
				
	changed = true;
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
