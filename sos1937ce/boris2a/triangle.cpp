//================================================================================================================================
// triangle.cpp
// ------------
//
//================================================================================================================================

#include "BS2all.h"


triangle::triangle()
{}

triangle::~triangle()
{}

//================================================================================================================================
// getBounds
// ---------
//
// get the bounding rectangle around a triangle, expressed as a 'minimum' vector and a maximum vector.
//================================================================================================================================

void triangle::getBounds( vector3 * min, vector3 * max )
{


	minmax3( p.v->geoCoord->x, q.v->geoCoord->x, r.v->geoCoord->x, &min->x, &max->x );
	minmax3( p.v->geoCoord->y, q.v->geoCoord->y, r.v->geoCoord->y, &min->x, &max->x );
	minmax3( p.v->geoCoord->z, q.v->geoCoord->z, r.v->geoCoord->z, &min->x, &max->x );

};

float triangle::DistanceSquared( vector3 * v )
{
	
	double num = A * v->x + B * v->y + C * v->z + planeD;
	
			// ABC = 1 /( A^2 + B^2 + C^2)

	return( (float)( num * num * ABC ) );
		

};

//================================================================================================================================
// END OF FILE
//================================================================================================================================
