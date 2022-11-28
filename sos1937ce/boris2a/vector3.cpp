//================================================================================================================================
// vector3.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"

uchar vector3::SCmask( vector3 * min,  vector3 * max )
{
	return(
		(   (x >= min->x) ? 1  : ( ( x < max->x ) ?  2 : 0 )   )  |
		(   (y >= min->y) ? 4  : ( ( y < max->y ) ?  8 : 0 )   )  |
		(   (z >= min->z) ? 16 : ( ( z < max->z ) ? 32 : 0 )   ) 
		   );
}


void vector3::multiplyByMatrix3x3( vector3 * v, matrix3x4 * m )
{
	x = m->E00 * v->x + m->E01 * v->y + m->E02 * v->z ;
	y = m->E10 * v->x + m->E11 * v->y + m->E12 * v->z ;  
	z = m->E20 * v->x + m->E21 * v->y + m->E22 * v->z ;  
}
void vector3::multiplyByMatrix3x4( vector3 * v, matrix3x4 * m )
{
	x = m->E00 * v->x + m->E01 * v->y + m->E02 * v->z + m->translation.x ;   // v->? == 1
	y = m->E10 * v->x + m->E11 * v->y + m->E12 * v->z + m->translation.y ;  
	z = m->E20 * v->x + m->E21 * v->y + m->E22 * v->z + m->translation.z ;  
}
void vector3::multiplyByMatrixTranspose3x4( vector3 * v, matrix3x4 * m )
{
	x = m->E00 * v->x + m->E10 * v->y + m->E20 * v->z - m->translation.x ;   // v->? == 1
	y = m->E01 * v->x + m->E11 * v->y + m->E21 * v->z - m->translation.y ;  
	z = m->E02 * v->x + m->E12 * v->y + m->E22 * v->z - m->translation.z ;  
}

void vector3::rationaliseAngleVector()
{
return;
        while (x >= TWOPI)
                 x -= (float)TWOPI;
        while (y >= TWOPI)
                 y -= (float)TWOPI;
        while (z >= TWOPI)
                 z -= (float)TWOPI;

        while (x < 0)
                 x += (float)TWOPI;
        while (y < 0)
                 y += (float)TWOPI;
        while (z < 0)
                 z += (float)TWOPI;
}

//---------------------------------------------------------------------------------------------
// isCoordOnXZtriangle
// -------------------
//		is this coordinate (XZ part) in world space inside the XZ part of the triangle ?
//---------------------------------------------------------------------------------------------

bool vector3::isCoordOnXZtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;		// geoCoord = transformed for world object
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Xtp = pcoord->x - x;
	float Ztp = pcoord->z - z;
	float Xup = qcoord->x - x;
	float Zup = qcoord->z - z;
	float Xvp = rcoord->x - x;
	float Zvp = rcoord->z - z;
			
	if( Xtp*Zup  >  Xup*Ztp )
	{
			return ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
	} else
	{
			return ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
	}
}


//---------------------------------------------------------------------------------------------
// isCoordOnXYtriangle
// -------------------
//		is this coordinate (XY part) in world space inside the XY part of the triangle ?
//---------------------------------------------------------------------------------------------

bool vector3::isCoordOnXYtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Xtp = pcoord->x - x;
	float Ytp = pcoord->y - y;
	float Xup = qcoord->x - x;
	float Yup = qcoord->y - y;
	float Xvp = rcoord->x - x;
	float Yvp = rcoord->y - y;
			

	if( Xtp*Yup  >  Xup*Ytp )
	{
			return ( ( Xup*Yvp >= Xvp*Yup) &&  ( Xvp*Ytp >= Xtp*Yvp) );
	} else
	{
			return ( ( Xup*Yvp <= Xvp*Yup) &&  ( Xvp*Ytp <= Xtp*Yvp) );
	}
}

//---------------------------------------------------------------------------------------------
// isCoordOnYZtriangle
// -------------------
//		is this coordinate (YZ part) in world space inside the YZ part of the triangle ?
//---------------------------------------------------------------------------------------------

bool vector3::isCoordOnYZtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Ytp = pcoord->y - y;
	float Ztp = pcoord->z - z;
	float Yup = qcoord->y - y;
	float Zup = qcoord->z - z;
	float Yvp = rcoord->y - y;
	float Zvp = rcoord->z - z;
			
	if( Ytp*Zup  >  Yup*Ztp )
	{
			return ( ( Yup*Zvp >= Yvp*Zup) &&  ( Yvp*Ztp >= Ytp*Zvp) );
	} else
	{
			return ( ( Yup*Zvp <= Yvp*Zup) &&  ( Yvp*Ztp <= Ytp*Zvp) );
	}
}







#define CONTAINDIFF 0.1f
//0.0000001f

//---------------------------------------------------------------------------------------------
// isCoordOnXZtriangle
// -------------------
//		is this coordinate (XZ part) in world space inside the XZ part of the triangle ?
//---------------------------------------------------------------------------------------------

bool vector3::isCoordNearlyOnXZtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;		// geoCoord = transformed for world object
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Xtp = pcoord->x - x;
	float Ztp = pcoord->z - z;
	float Xup = qcoord->x - x;
	float Zup = qcoord->z - z;
	float Xvp = rcoord->x - x;
	float Zvp = rcoord->z - z;
			
	if( Xtp*Zup  >  (Xup*Ztp - CONTAINDIFF) )
	{
			return ( ( Xup*Zvp >= (Xvp*Zup - CONTAINDIFF)) &&  ( Xvp*Ztp >= (Xtp*Zvp - CONTAINDIFF)) );
	} else
	{
			return ( ( Xup*Zvp <= (Xvp*Zup - CONTAINDIFF)) &&  ( Xvp*Ztp <= (Xtp*Zvp - CONTAINDIFF)) );
	}
}

bool vector3::isCoordNearlyOnXYtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Xtp = pcoord->x - x;
	float Ytp = pcoord->y - y;
	float Xup = qcoord->x - x;
	float Yup = qcoord->y - y;
	float Xvp = rcoord->x - x;
	float Yvp = rcoord->y - y;
			

	if( Xtp*Yup  >  (Xup*Ytp - CONTAINDIFF) )
	{
			return ( ( Xup*Yvp >= (Xvp*Yup  - CONTAINDIFF)) &&  ( Xvp*Ytp >= (Xtp*Yvp  - CONTAINDIFF)) );
	} else
	{
			return ( ( Xup*Yvp <= (Xvp*Yup - CONTAINDIFF)) &&  ( Xvp*Ytp <= (Xtp*Yvp - CONTAINDIFF)) );
	}
}

bool vector3::isCoordNearlyOnYZtriangle( triangle * triPtr )
{
	vector3 * pcoord = triPtr->p.v->geoCoord;
	vector3 * qcoord = triPtr->q.v->geoCoord;
	vector3 * rcoord = triPtr->r.v->geoCoord;

	float Ytp = pcoord->y - y;
	float Ztp = pcoord->z - z;
	float Yup = qcoord->y - y;
	float Zup = qcoord->z - z;
	float Yvp = rcoord->y - y;
	float Zvp = rcoord->z - z;
			
	if( Ytp*Zup  >  (Yup*Ztp - CONTAINDIFF) )
	{
			return ( ( Yup*Zvp >= (Yvp*Zup - CONTAINDIFF)) &&  ( Yvp*Ztp >= (Ytp*Zvp - CONTAINDIFF)) );
	} else
	{
			return ( ( Yup*Zvp <= (Yvp*Zup - CONTAINDIFF)) &&  ( Yvp*Ztp <= (Ytp*Zvp - CONTAINDIFF)) );
	}
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
