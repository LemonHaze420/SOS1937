//================================================================================================================================
// quaternion.cpp
// --------------
//
//================================================================================================================================

#include "BS2all.h"

void  quaternion::quatToMat( matrix3x4 * m )			// return result matrix in 'm' (only 3x3 part affected)
{
	double s,xs,ys,zs,wx,wy,wz,xx,xy,xz,yy,yz,zz;

	s = 2.0f * RECIPROCAL( x*x + y*y + z*z + w*w );		// we should be able to use 2.0f if the quaternion is correct

	xs = x * s;   ys = y * s;   zs = z * s;
	wx = w * xs;  wy = w * ys;  wz = w * zs;
	xx = x * xs;  xy = x * ys;  xz = x * zs;
	yy = y * ys;  yz = y * zs;  zz = z * zs;

	m->E00 = 1.0f - (float)(yy + zz);
	m->E10 = (float)(xy + wz);
	m->E20 = (float)(xz - wy);

	m->E01 = (float)(xy - wz);
	m->E11 = 1.0f - (float)(xx + zz);
	m->E21 = (float)(yz + wx);

	m->E02 = (float)(xz + wy);
	m->E12 = (float)(yz - wx);
	m->E22 = 1.0f - (float)(xx + yy);

}

void  quaternion::quatFromMat( matrix3x4  * m )		// return quaternion from parameter matrix
{
	double tr, s;

	tr = m->E00 + m->E11 + m->E22;
	if( tr > 0.0f )
	{
		s = sqrt( tr + 1.0f );

		w = (float)s * 0.5f;

		s = 0.5f * RECIPROCAL(s);

		x = ( m->E21 - m->E12 ) * (float)s;
		y = ( m->E02 - m->E20 ) * (float)s;
		z = ( m->E10 - m->E01 ) * (float)s;

	} 
	else
	{
	}
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
