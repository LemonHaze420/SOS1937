//================================================================================================================================
// hunk.h
// ------
//			A hunk is a XZ 2d-box containing a list of triangles which occur within this list.
//================================================================================================================================

#ifndef _BS_hunk
#define _BS_hunk

class hunk
{
	friend class world;
	friend class engine;
	friend class SphereSet;
	friend class Sphere;

	public:
		hunk();
		~hunk();

	private:
		triangle ** tlistPtr_N,  ** nextTriangle_N,  ** tlistPtr_W, ** nextTriangle_W;		
		slong tcount_N, tcount_W;
};

#endif	// _BS_hunk

//================================================================================================================================
//END OF FILE
//================================================================================================================================
