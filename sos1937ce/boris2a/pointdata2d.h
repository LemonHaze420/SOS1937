//================================================================================================================================
// pointdata2d.h
// -------------
//			
//================================================================================================================================

#ifndef _BS_pointdata2d
#define _BS_pointdata2d

class pointdata2d
{
	friend class engine;

	public:
		pointdata2d() {};
		~pointdata2d() {};
	//private:
		vector2P  *v;			
		nongeodata *nv;			// Base texture u v's
		nongeodata nv1;		// Environment mapping u,v s	
};


#endif	// _BS_pointdata3d

//================================================================================================================================
//END OF FILE
//================================================================================================================================
