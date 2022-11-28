//================================================================================================================================
// vector3.h
// ---------
//			3d vector
//================================================================================================================================

#ifndef _BS_vector3
#define _BS_vector3

#include "bs2utils.h"

class vector3
{
	friend class engine;
	friend class world;
	friend class triangle;
	friend class camera;
	friend class matrix3x4;

public:
	vector3(){}
	vector3( float xx, float yy, float zz )
	{
		x = xx;
		y = yy;
		z = zz;
	}
	~vector3(){}

	uchar SCmask( vector3 * min,  vector3 * max );		// returns SCmask for a point within a bounding box
															// described by two vectors
	void multiplyByMatrix3x3( vector3 * v, matrix3x4 * m );
	void multiplyByMatrix3x4( vector3 * v, matrix3x4 * m );
	void multiplyByMatrixTranspose3x4( vector3 * v, matrix3x4 * m );

	void rationaliseAngleVector();							// only used for Euler angle vectors to put them in range
	void  Normalise()
	{
		float r = RECIPROCAL( (float)sqrt(x*x + y*y + z*z) );
		x *= r;
		y *= r;
		z *= r;
	}
	void  NormaliseWithCheckAndMag( float * mag )
	{
		float m;
		float r = x*x + y*y + z*z;
		if(r > 0.00001f)
		{
			m = (float)sqrt(r);
			*mag = m;
			r = RECIPROCAL( m );
			x *= r;
			y *= r;
			z *= r;
		}
	}
	float DotProduct( vector3 * r)
	{
		return ( (float)(  (x * r->x) + (y * r->y) + (z * r->z) ) );
	}

	bool  IsNull()
	{
		return( (x == 0.0f) && (y == 0.0f) && ( z== 0.0f) );
	}

	float distanceSquared( vector3 * v )
	{
		float dx = v->x - x;
		float dy = v->y - y;
		float dz = v->z - z;

		return( dx*dx + dy*dy + dz*dz );
	}

	float FastInvMag()
	{
		return( (float)fisqrt(x*x + y*y + z*z) );
	}
	float FastMag()
	{
		return( (float)fsqrt(x*x + y*y + z*z) );
	}
	float Mag()
	{
		return( (float)sqrt(x*x + y*y + z*z) );
	}
	float MagSquared()
	{
		return( (float)(x*x + y*y + z*z) );
	}
	void  FastNormalise()
	{
		float r = (float)fisqrt(x*x + y*y + z*z);
		x *= r;
		y *= r;
		z *= r;
	}
	void  FastNormalise(vector3 * s)
	{
		float r = (float)fisqrt(s->x*s->x + s->y*s->y + s->z*s->z);
		x = s->x * r;
		y = s->y * r;
		z = s->z * r;
	}

	void negate( vector3 * v )
	{
		x = -v->x;
		y = -v->y;
		z = -v->z;
	}
	
	vector3& operator *= (const float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	vector3& operator /= (const float f)
	{
		float Temp = RECIPROCAL(f);
		x *= Temp;
		y *= Temp;
		z *= Temp;
		return *this;
	}

	vector3& operator += (vector3& r)
	{
		x += r.x;
		y += r.y;
		z += r.z;
		return *this;
	}

	vector3& operator -= (vector3& r)
	{
		x -= r.x;
		y -= r.y;
		z -= r.z;
		return *this;
	}
		
	friend bool operator != (vector3& a, vector3& b)
	{
		return ( (a.x != b.x) ||
					    (a.y != b.y) ||
						(a.z != b.z));
	}

	friend vector3 operator * (vector3& a, vector3& b)
	{
		return vector3(a.y * b.z - a.z * b.y,
					   a.z * b.x - a.x * b.z,
				       a.x * b.y - a.y * b.x);
	}

	friend vector3 operator * (vector3& a, float b)
	{
		return vector3(a.x * b,
					   a.y * b,
				       a.z * b);
	}

	friend vector3 operator + (vector3& a, vector3& b)
	{
		return vector3(a.x + b.x,
						   a.y + b.y,
						   a.z + b.z);
	}

	friend vector3 operator - (vector3& a, vector3& b)
	{
		return vector3(a.x - b.x,
					   a.y - b.y,
					   a.z - b.z);
	}
	

	bool isCoordOnXZtriangle( triangle * triPtr );
	bool isCoordOnXYtriangle( triangle * triPtr );
	bool isCoordOnYZtriangle( triangle * triPtr );
	bool isCoordNearlyOnXZtriangle( triangle * triPtr );
	bool isCoordNearlyOnXYtriangle( triangle * triPtr );
	bool isCoordNearlyOnYZtriangle( triangle * triPtr );
	
	float x, y, z;

};			

#endif	// _BS_vector3

//================================================================================================================================
//END OF FILE
//================================================================================================================================
