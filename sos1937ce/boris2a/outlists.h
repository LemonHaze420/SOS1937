//================================================================================================================================
// outLists.h
// ----------
//			- output lists which collect the final projected 2D triangles prior to rendering
//================================================================================================================================

#ifndef _BS_outLists
#define _BS_outLists

class outLists
{
	friend class engine;

	public:
			outLists(slong trianglecount, slong vector2Pcount, slong nonGeocount);
			~outLists();
//	private:

			outTriangle * triangleList;			// array of outTriangles
			vector2P    * vector2PList;			// array of 2D projected vectors referenced by outTriangle
			nongeodata	* nongeoList;

			
			sortlistentry * sortlistPtr;			// used for sorting ... 

												// LOCAL SOFTWARE RENDERER .. triangle 'z' s
												// HARDWARE RENDERER       .. materials
												//
			slong tmax, vmax, nmax;

		
};


#endif	// _BS_outLists

//================================================================================================================================
//END OF FILE
//================================================================================================================================
