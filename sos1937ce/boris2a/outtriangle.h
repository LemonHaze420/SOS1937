//================================================================================================================================
// outTriangle.h
// -------------
//			triangle for the output lists
//================================================================================================================================

#ifndef _BS_outTriangle
#define _BS_outTriangle

class outTriangle
{
	friend class engine;

	public:
		outTriangle(){};
		~outTriangle(){};
	//private:
		pointdata2d		p, q, r;
		material * 	    mtl;			// index of material to use

		bool oneEdge;
		long minx, minx_y, maxx, maxx_y, other_x, other_y;


};


#endif	// _BS_outTriangle

//================================================================================================================================
//END OF FILE
//================================================================================================================================
