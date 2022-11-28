//================================================================================================================================
// sphere.h 
// ---------
//				Interface for the sphereset class.
//================================================================================================================================

#ifndef __BS_SPHERESET 
#define __BS_SPHERESET

typedef enum { SPHERE,LINE,CIRCLE } SphereType;
 
class Sphere	// could be a line!
{
	friend class hunk;

public:
	Sphere(){};
	~Sphere(){};

	SphereType type;

	vector3 v;
    vector3 v2;	// other point if line
    float rad,radsquared;	// radius and radius-squared
	float offset;		// 'd' in plane equation for circles
	float angle;		// used in CIRCLE in transformed SS's - it's the Y angle of the object
						// and is used for wheels.

	int    edgeIntersectSphere( vector3 * p, vector3 * q, vector3 * fv);
	bool	sphereCollidesWithWorld( vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime );
	void    lineProtrusion( vector3 * v0, vector3 * deltaVector	);
	bool	getTimeFromBestt(vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime );
};


class SphereSet  
{
	friend class hunk;
	friend class Sphere;

public:
	SphereSet(char *filename, float scale);
	SphereSet()
	{
		data=NULL;
		num=0;
	}
	virtual ~SphereSet();

	int num;
	Sphere *data;

	void transformSpheres( SphereSet * rawSpheres, matrix3x4 * transf );	// from local to world space
	bool isTriColliding( triangle *tri ); //, BroadVertex *verts)
	ulong isCollidingWithWorld( vector3 * v,  float d);

	bool	collidesWithWorld( vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime );
	
};

#endif //  __BS_SPHERESET 

//================================================================================================================================
// END OF FILE
//================================================================================================================================
