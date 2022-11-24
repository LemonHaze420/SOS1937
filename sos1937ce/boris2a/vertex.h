// $Header$

// $Log$

#ifndef _BS_vertex
#define _BS_vertex


class vertex
{
	friend class world;
	friend class engine;
	friend class triangle;
	friend class linkToTriangle;

public:

	vector3	 geoCoord;		// geometric 3d co-ordinate

//	vector3     transformed;	// holds the transformed geoCoord (in world space)

	vector3	vertexNormal;	// 
//	vector3     transformedvertexNormal;

//	vector2P    * projectedPtr;	// ptr to projection of transformed co-ordinate

//	ulong		tcnt;			// counter for transformation validity (set from globalpcnt)
//	ulong		pcnt;			// counter for projection validity (set from globalpcnt)
//	uchar       SCmaskInZ;		// Holding the vertex Z-plane SuthCohen component value for frontback plane clipping

//	float		l;				// vertex light power
//	float       specularl;			// specular light power

	nongeodata  nv1;

private:

};


#endif	// _BS_vertex
