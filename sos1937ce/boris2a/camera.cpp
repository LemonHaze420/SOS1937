// $Header$

// $Log$
// Revision 1.4  2000-01-19 14:53:17+00  jjs
// Added getWorldFromScreen.
//
// Revision 1.3  2000-01-18 14:51:07+00  jjs
// Code fixed to remove double.
//
// Revision 1.2  2000-01-18 14:11:57+00  jjs
// Added faceTowardPoint.
//
// Revision 1.1  2000-01-17 12:37:06+00  jjs
// Latest version has corrected Alpha Tested polygons.
//

#include "BS2all.h"

camera::camera( vector3 * p,  float zeyevalue, float zback )
{
	float sn,cs;

#if defined(UNDER_CE)
	_SinCosA(&sn,&cs,0.0f);
#else
	sn = (float)sin(0.0);
	cs = (float)cos(0.0);
#endif

	setCameraPosition( p );

	zeye = zeyevalue;

	direction = vector3(0,0,1);

	RotMatrix.translation = 
	inverseRotMatrix.translation = vector3(0,0,0);

	RotMatrix.set3x3submat( 1.0f, 0.0f, 0.0f,
									   0.0f, cs, sn,
									   0.0f, -sn, cs );			// 3x3 sub matrix ONLY is ever used

	faceTurningTransform.set3x3submat( 1.0f, 0.0f, 0.0f,
									   0.0f, 1.0f, 0.0f,
									   0.0f, 0.0f, 1.0f );			// 3x3 sub matrix ONLY is ever used

	inverseRotMatrix.Transpose3x4(&RotMatrix);

	attached = 0;					// default = no attached object

	minheight = 1.0f;

	prev = next = 0;				// initially not linked
	moved = true;
}

void camera::faceTowardPoint( vector3 * p , bool force)
{
	static vector3 rx, ry, rz, nposition;
	float r;//, dx, dy, dz;
		
	rz.x = p->x - position.x;
	rz.y = p->y - position.y;
	rz.z = p->z - position.z + 0.001f;
	
#if defined(UNDER_CE)
	r = _InvSqrtA( rz.z*rz.z + rz.y*rz.y + rz.x*rz.x );
#else
	r = 1.0f / (float)sqrt( rz.z*rz.z + rz.y*rz.y + rz.x*rz.x );
#endif	
	rz.x *= r;
	rz.y *= r;
	rz.z *= r;
	
	direction = rz;
	
#if defined(UNDER_CE)
	r = _InvSqrtA( rz.z*rz.z + rz.x*rz.x );
#else
	r = 1.0f / (float)sqrt( rz.z*rz.z + rz.x*rz.x );
#endif	
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
