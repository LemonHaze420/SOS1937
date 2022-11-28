//================================================================================================================================
// outLists.cpp
// ------------
//				lists of data for 2D triangles prior to rendering:
//						(i)			List of output triangles with 2D coords.
//						(ii)		List of projected vertices (vector2P) referenced from output triangles.
//================================================================================================================================

#include "BS2all.h"

outLists::outLists(slong trianglecount, slong vector2Pcount, slong nonGeocount)
{
	triangleList = new outTriangle[trianglecount];
	vector2PList = new vector2P[vector2Pcount];
	nongeoList	 = new nongeodata[nonGeocount];

	sortlistPtr  = new sortlistentry[ trianglecount ];

	tmax = trianglecount;
	vmax = vector2Pcount;
	nmax = nonGeocount;
}

outLists::~outLists()
{
	delete[] triangleList;
	delete[] vector2PList;
	delete[] nongeoList;

	delete[] sortlistPtr;
}
			
//================================================================================================================================
// END OF FILE
//================================================================================================================================

