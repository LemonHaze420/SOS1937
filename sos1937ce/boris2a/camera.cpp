//================================================================================================================================
// camera.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"

extern engine * boris2;

//-------------------------------------------------------------------------------------------------
// linkAfter
// ---------
//				link camera AFTER 'this' camera in chain
//-------------------------------------------------------------------------------------------------

void camera::linkAfter( camera * c )
{
	camera * oldnext;

	oldnext = next;
	next = c;
	c->prev = this;
	c->next = oldnext;
	if( oldnext )
		oldnext->prev = c;
}

//-------------------------------------------------------------------------------------------------
//  linkBefore
//  ----------
//				link camera BEFORE 'this' camera in chain
//-------------------------------------------------------------------------------------------------

void camera::linkBefore( camera * c )
{
	camera * oldprev;

	oldprev = prev;
	prev = c;
	c->next = this;
	c->prev = oldprev;
	if( oldprev )
		oldprev->next = c;
}

//-------------------------------------------------------------------------------------------------
//  deLink
//  ------
//				delink camera from chain
//-------------------------------------------------------------------------------------------------

void camera::deLink()
{
	if (next)
		next->prev = prev;
	if (prev)
		prev->next = next;
}


//--------------------------------------------------------------------------------------------------------------
//  findNearest
//  -----------
//				find camera in linked chain closest to point (look in both link directions from 'this' camera
//--------------------------------------------------------------------------------------------------------------

camera * camera::findNearest( vector3 * p )
{
	float dx, dy, dz, dsquared;
	float maxdsquared = -1.0f;
	camera * bestCamera;				// will always have a value (this if nothing else)

	camera * thiscam = this;
	do
	{
		dx = (thiscam->position.x - p->x);
		dy = (thiscam->position.y - p->y);
		dz = (thiscam->position.z - p->z);
		dsquared = dx*dx + dy*dy + dz*dz;
		if( maxdsquared > dsquared )
		{
			maxdsquared = dsquared;
			bestCamera  = thiscam;
		}
	} while( thiscam = thiscam->next );

	thiscam = this->prev;
	while( thiscam )
	{
	 	dx = (thiscam->position.x - p->x);
		dy = (thiscam->position.y - p->y);
		dz = (thiscam->position.z - p->z);
		dsquared = dx*dx + dy*dy + dz*dz;
		if( maxdsquared > dsquared )
		{
			maxdsquared = dsquared;
			bestCamera  = thiscam;
		}
		thiscam = thiscam->prev;
	}

	return(bestCamera);
}

//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------

void camera::transformWorldVector( vector3 * result, vector3 * wvector )
{
	// subtract camera position from world vector and multiply 3x3 sub matrix

	static vector3 tempvector;

	tempvector.x = wvector->x - position.x;
	tempvector.y = wvector->y - position.y;
	tempvector.z = wvector->z - position.z;

	RotMatrix.multiplyV3By3x3mat( result, &tempvector );

}


//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------

void camera::formCameraTransformMatrix()
{
	static vector3 unitvector;
	static matrix3x4 tempMatrix;

	RotX.set3x3submat( 	1, 0, 0,  
						0, cosAngle.x,  sinAngle.x,  
						0,  -sinAngle.x,  cosAngle.x
					 );


	RotY.set3x3submat( 	cosAngle.y, 0, -sinAngle.y,
						0, 1, 0,
						sinAngle.y, 0, cosAngle.y
					 );


	RotZ.set3x3submat( 	cosAngle.z,  sinAngle.z, 0,
						-sinAngle.z, cosAngle.z, 0,
						0, 0, 1
					 );

			// order of rot. .. Y,Z,X 

	tempMatrix.multiply3x3submats( &RotZ, &RotY );
	RotMatrix.multiply3x3submats( &RotX, &tempMatrix);


			//
			// Define the inverse of the Rotation matrix (used when moving camera)
			//			note that  -sin(x) = sin(-x)  is used here.
			//						cos(x) = cos(-x)  is used here
			// also note that the inverse of any one component matrix is the same
			// as using the non-inverted matrix with 'minus' the angle
			//

	inverseRotMatrix.Transpose3x4( &RotMatrix );

			// now lets create the direction vector and the up vector


	RotMatrix.multiplyV3By3x3mat( &direction, &vector3( 0.0f,  0.0f, 1.0f) );
	direction.Normalise();
}

void camera::setZbackplane( float zback )
{
	if(zback > 10.0f)
		zbackplane = zback;
	else
		zbackplane = 10.0f;
}


void camera::setZbackplane( float zback, world * curworld)
{
	float clipz=zback;
	float x1,y1,x2,y2;
	float nx,ny;
	float cx,cy;
	bool intersect=false;
	float t,u;


	nx=direction.x;
	ny=direction.z;
	cx=position.x;
	cy=position.z;

	if(curworld->numcliplines!=0)
	{
		for(int i=0;i<curworld->numcliplines;i++)
		{
			x1=curworld->clipline[i].x1;
			y1=curworld->clipline[i].y1;
			x2=curworld->clipline[i].x2;
			y2=curworld->clipline[i].y2;


			t = (ny*(x1-cx) - nx*(y1-cy)) / (nx*(y2-y1) - ny*(x2-x1));
			u = (y1 - cy + t * (y2-y1))/ny;

			if(t<1.0 && t>0.0 && u>0.0)
			{
				if(u<clipz) clipz=u;
				intersect=true;

			}
		}
		if(intersect) zbackplane=clipz; else zbackplane = zback;
	}
	else
	{
		if(zback > 10.0f)
			zbackplane = zback;
		else
			zbackplane = 10.0f;
	}
}


//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------

camera::camera( vector3 * p,  float zeyevalue, float zback )
{
	setCameraPosition( p );

	zeye = zeyevalue;
	setZbackplane( zback );

	direction = vector3(0,0,1);
	Octant = normalOctantCode( &direction );		// we need this for easy-culling purposes
	RotMatrix.translation = 
	inverseRotMatrix.translation = vector3(0,0,0);

	faceTurningTransform.set3x3submat( 1.0f, 0.0f, 0.0f,
									   0.0f, 1.0f, 0.0f,
									   0.0f, 0.0f, 1.0f );			// 3x3 sub matrix ONLY is ever used

	attached = 0;					// default = no attached object

	minheight = 1.0f;

	prev = next = 0;				// initially not linked
	moved = true;
}

//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------

camera::~camera()
{

	if (attached != 0)
		attached->attached = 0;		// tell any attached object that the camera has gone.

	if( boris2->currentcamera == this)
		boris2->currentcamera = NULL;
}

//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------


void camera::checkUnderWater()
{
	float depthunderwater;
	if( boris2->currentworld)
		underwater = boris2->currentworld->isPointUnderwater( &position, &depthunderwater,
									&underWaterTrianglePtr );
	else
		underwater = false;
}

//-------------------------------------------------------------------------------------------------
// 
//-------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------
// moveCameraPositionLAGGED
// ------------------------
//		- useful for a fixed camera when tracking a passing vehicle.
//		  (if the lagVector components are '0' then we have no movement of the camera position.
//		   if the lagVector components are small then we a correspondingly small amount of movement)
//		   
//---------------------------------------------------------------------------------------------------

void camera::moveCameraPositionLAGGED( vector3 * deltap, vector3 * lagVector)
{
 
	position.x += deltap->x * lagVector->x;
	position.y += deltap->y * lagVector->y;
	position.z += deltap->z * lagVector->z;
}

void camera::moveCameraPositionInDirection( vector3 * deltap )
{
	vector3 temp;
	inverseRotMatrix.multiplyV3By3x3mat( &temp, deltap ); 
    position += temp;
	moved = true;
}

void camera::setCameraAngle(     vector3 * a )
{
	angle = *a;
      
    angle.rationaliseAngleVector();

	cosAngle.x = mySineAndCosine( angle.x, &sinAngle.x );
	cosAngle.y = mySineAndCosine( angle.y, &sinAngle.y );
	cosAngle.z = mySineAndCosine( angle.z, &sinAngle.z );

	formCameraTransformMatrix();
}

void camera::moveCameraAngle(     vector3 * a )
{
	angle += *a;
      
    angle.rationaliseAngleVector();

	cosAngle.x = mySineAndCosine( angle.x, &sinAngle.x );
	cosAngle.y = mySineAndCosine( angle.y, &sinAngle.y );
	cosAngle.z = mySineAndCosine( angle.z, &sinAngle.z );

	formCameraTransformMatrix();
}

void camera::attach( object * optr, uchar aValue, vector3 * attachPoint)
{
	if(attached)
		attached->attached = 0;		// if any object was already attached then disconnect it

	attached = optr;
	attached->attached = this;		// connect camera to object and vice versa

	attached->attachedValue = aValue;

	attached->attachmentPoint = * attachPoint;
}

void camera::detach()
{
	if(attached)
	{
		attached->attached = 0;
		attached = 0;
	}
}

camera::camera()
{
};

//
//  Rotate the camera through an angle about a point ( keep it facing in the same direction )
//  - THIS REALLY NEEDS OPTIMISING ONCE WE KNOW IT IS STABLE
//

void camera::rotateAboutPoint( vector3 * a, vector3 * p )
{
	static vector3 tempvector;
	float sinx, cosx, siny, cosy, sinz, cosz;
	static matrix3x4 RX;
	static matrix3x4 RY;
	static matrix3x4 RZ;
	static matrix3x4 RXRZ;
	static matrix3x4 rmatrix;

	tempvector.x = position.x - p->x;
	tempvector.y = position.y - p->y;
	tempvector.z = position.z - p->z;

	a->rationaliseAngleVector();

	cosx = mySineAndCosine( a->x, &sinx );
	cosy = mySineAndCosine( a->y, &siny );
	cosz = mySineAndCosine( a->z, &sinz );
	
	RX.set3x3submat(	1,   0,   0,  
						0,  cosx,   sinx,  
						0,  -sinx,  cosx  
					 );

	RY.set3x3submat(	cosy, 0, -siny,
						0,  1, 0, 
						siny, 0, cosy
					 );

	RZ.set3x3submat( 	cosz,  sinz, 0, 
						-sinz, cosz, 0, 
						0, 0, 1
				    );
	
			// order of rot...  Z,X,Y

	RXRZ.multiply3x3submats( &RX, &RZ );
	rmatrix.multiply3x3submats( &RY,  &RXRZ);		

	rmatrix.multiplyV3By3x3mat( &position, &tempvector );

	position.x += p->x;
	position.y += p->y;
	position.z += p->z;
	moved = true;
}

void camera::rotateAboutPoint_ACCURATE( vector3 * a, vector3 * p )
{
	static vector3 tempvector;
	float sinx, cosx, siny, cosy, sinz, cosz;
	static matrix3x4 RX;
	static matrix3x4 RY;
	static matrix3x4 RZ;
	static matrix3x4 RXRZ;
	static matrix3x4 rmatrix;

	tempvector.x = position.x - p->x;
	tempvector.y = position.y - p->y;
	tempvector.z = position.z - p->z;

	a->rationaliseAngleVector();

	cosx = (float)cos( a->x); 
	sinx = (float)sin( a->x );
	cosy = (float)cos( a->y); 
	siny = (float)sin( a->y );
	cosz = (float)cos( a->z); 
	sinz = (float)sin( a->z );

	RX.set3x3submat(	1,   0,   0,  
						0,  cosx,   sinx,  
						0,  -sinx,  cosx  
					 );

	RY.set3x3submat(	cosy, 0, -siny,
						0,  1, 0, 
						siny, 0, cosy
					 );

	RZ.set3x3submat( 	cosz,  sinz, 0, 
						-sinz, cosz, 0, 
						0, 0, 1
					  );

					  
			// order of rot...  Z,X,Y

	RXRZ.multiply3x3submats( &RX, &RZ );
	rmatrix.multiply3x3submats( &RY,  &RXRZ);		

	rmatrix.multiplyV3By3x3mat( &position, &tempvector );

	position.x += p->x;
	position.y += p->y;
	position.z += p->z;
	moved = true;
}

//
//  make the camera face a particular point in the world
//

void camera::faceTowardPoint( vector3 * p , bool force)
{
	static vector3 rx, ry, rz, nposition;
	float r;//, dx, dy, dz;
	

	{
		float yvalue;
		static triangle * resulttriPtr;
		
		boris2->currentworld->getY( &position, &yvalue, &resulttriPtr, true );
		yvalue += minheight;
		if (!force)
		{
			if( position.y < yvalue )
			{
				position.y = yvalue;
				moved = true;
			}
		}
	}
	
	rz.x = p->x - position.x;
	rz.y = p->y - position.y;
	rz.z = p->z - position.z + 0.001f;
	
	r = RECIPROCAL( (float)sqrt( rz.z*rz.z + rz.y*rz.y + rz.x*rz.x ) );
	
	rz.x *= r;
	rz.y *= r;
	rz.z *= r;
	
	direction = rz;
	
	r = RECIPROCAL( (float)sqrt( rz.z*rz.z + rz.x*rz.x ) );
	
	rx.x = rz.z * r;
	rx.z = -rz.x * r;
	
	ry.x = rz.y * rx.z;
	ry.y = rz.z * rx.x - rz.x * rx.z;
	ry.z = - rz.y * rx.x;
	
	static matrix3x4 t;
	
	t.set3x3submat( rx.x, ry.x, rz.x,
		0   , ry.y, rz.y,
		rx.z, ry.z, rz.z );
	

	
	RotMatrix.multiply3x3submats(&faceTurningTransform, &t );		// faceTurningTransform - final camera rotation after face
																	// toward object!!
		
	inverseRotMatrix.Transpose3x4(&RotMatrix);
	direction.Normalise();	
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================

