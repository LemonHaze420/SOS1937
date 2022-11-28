//================================================================================================================================
// collide.cpp
// -----------
//
//================================================================================================================================

#include "BS2all.h"

extern engine * boris2;

extern vector3 collpoint;

//================================================================================================================================
// EdgeIntersectSphere
// -------------------
//	Returns 0 if the line intersects the sphere, otherwise returns which side of the
//	sphere are are on (1 or -1)
//
//================================================================================================================================

int EdgeIntersectSphere( Sphere *s, vector3 *v1, vector3 *v2)
{
	// get some stuff into temporaries, then
	// get the line parameters for the line equations
	//	x = x1 + ft
	//	y = y1 + gt
	//	z = z1 + ht
	// parameter t goes from 0 at v1 to 1 at v2.

	float f,g,h,
		x,y,z,
		lx,ly,lz,
		px,py,pz,
		t;


	px=s->v.x;
	py=s->v.y;
	pz=s->v.z;

	lx=v1->x;
	ly=v1->y;
	lz=v1->z;

	f = v2->x - lx;
	g = v2->y - ly;
	h = v2->z - lz;

	// now get the value of t as the nearest point to the centre of the sphere

	float numerator = 	f*(px - lx) + g*(py - ly) +	h*(pz - lz);
	float denominator = (f*f + g*g +h*h);

	bool  nless = ( numerator < 0.0f );
	bool  dless = ( denominator < 0.0f);
	
	if( nless != dless )
		t = 0;					// since this implies that n/d < 0
	else
	{
			// both numerator and denominator have the same sign

		if( nless )
		{
			if( numerator < denominator )		//   eg. -6 / -5
				t = 1;
			else
				t = numerator / denominator;
		}
		else
		{
			if( numerator > denominator )		//   eg   6 / 5
				t = 1;
			else
				t = numerator / denominator;
		}
	}

	// plug this value of t back into the line equations to get new x,y,z. These are the coordinates
	// of the closest point. Get the deltas between this and our centre point.

	x = px - (lx + f*t);
	y = py - (ly + g*t);
	z = pz - (lz + h*t);

	// now get the distance(squared) between x,y,z and px,py,pz. We'll call it f.

	f = x*x + y*y + z*z;

	if(f<s->radsquared)
	{
		// return 0 (intersect) if the distance < radius of sphere
		// we assume the collision point is the nearest point on the line to the
		// centre of the sphere. We just undo the subtractions done above.

		collpoint.x = px + x;
		collpoint.y = py + y;
		collpoint.z = pz + z;
		return 0;
	}

	// OK, we now know that all the points are outside the triangle, the plane intersects
	// the sphere, and none of the edges intersect the sphere.

	static vector3 pv,qv,rv;

	// so we work out which side we're on. If all calls to this routine return non-zero,
	// then the calling routine checks to see if the sphere's centre is on the same side
	// of all the edges. If so, we're inside the triangle.

	pv = *v2 - *v1;
	qv.x = px - lx;
	qv.y = py - ly;
	qv.z = pz - lz;

	rv = pv*qv;


	return rv.y < 0 ? 1 : -1;
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
