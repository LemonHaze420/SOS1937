// $Header$

// $Log$
// Revision 1.4  2000-02-07 12:20:49+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.3  2000-01-18 15:27:17+00  jjs
// Added detach routines.
//
// Revision 1.2  2000-01-17 12:37:07+00  jjs
// Latest version has corrected Alpha Tested polygons.
//
// Revision 1.1  2000-01-13 17:27:06+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

#define	FIRSTLOD 0

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
			transformedSpheres->data = new Sphere[lodmesh->spheres->num];
		}

		turningPoint = vector3(0,0,0);
	}
}

void object::setAllAngles( matrix3x4 * resultmatrix )
{

	angle.rationaliseAngleVector();

#if defined(UNDER_CE)
	_SinCosA(&sinAngle.x, &cosAngle.x, angle.x);
	_SinCosA(&sinAngle.y, &cosAngle.y, angle.y);
	_SinCosA(&sinAngle.z, &cosAngle.z, angle.z);
#else
	cosAngle.x = (float)cos(angle.x);
	sinAngle.x = (float)sin(angle.x);
	cosAngle.y = (float)cos(angle.y);
	sinAngle.y = (float)sin(angle.y);
	cosAngle.z = (float)cos(angle.z);
	sinAngle.z = (float)sin(angle.z);
#endif	
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

void object::setAngle_Y( float a )
{
	float sn,cs;

#if defined(UNDER_CE)
	_SinCosA(&sn,&cs,a);
#else
	sn = (float)sin(a);
	cs = (float)cos(a);
#endif
	transform.set3x3submat(	cs, 0, -sn,
						0,  1, 0, 
						sn, 0, cs
					 );

	changed = true;
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

	// Two sisters must surely have the same parent.....
	sisterObject->parent = parent;
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
			
//			optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
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
//				optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
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
//				optr->transformedBoundingBox.transformBoundBoxM3x4( &optr->modelData->boundingBox, &optr->concatenatedTransform);
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

