//===============================================================================================================================
// triangle.h
// ----------
//
//================================================================================================================================

#ifndef _BS_triangle
#define _BS_triangle

class triangle
{
	friend class world;
	friend class engine;
	friend class edge;

	public:
		triangle();
		~triangle();

		void getBounds( vector3 * min, vector3 * max );		// getting rectangle around triangle
		pointdata3d		p, q, r;

		material * mtl;		// pointer to material 
		
		vector3 faceNormal;			

		float   planeD;
		float   ABC;				// holds 1 / (A*A + B*B + C*C)
		float   minusOneOverB;		// holds -1 / B		WHEN  faceNormal.y != 0 !!  (ie. not vertical face)

		vector3 minLimit;
		vector3 maxLimit;

		ulong   pcnt;				// initially o, loadded with globalpcnt when 'used' to prevent it being reused.

		float A, B, C;
		schar   faceNormalOctant;	// code representing which octant the face normal lies within

		float DistanceSquared( vector3 * v );

		edge *edgea, *edgeb, *edgec;


		vector3 centroid;
		float boundingRadius, boundingRadiusSquared;


	private:
	
};

#endif	// _BS_triangle

//================================================================================================================================
//END OF FILE
//================================================================================================================================
