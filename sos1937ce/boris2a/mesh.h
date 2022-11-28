//================================================================================================================================
// mesh.h
// ------
//
//================================================================================================================================

#ifndef _BS_mesh
#define _BS_mesh


class mesh {

	friend class engine;
	friend class world;
	friend class edge;

public:
		mesh();
		~mesh();

		float MinCoordX, MaxCoordX;
		float MinCoordY, MaxCoordY;
		float MinCoordZ, MaxCoordZ;

		triangle  * tlistPtr;		// all triangles in the mesh
		vertex    * vlistPtr;		// all vertices in the mesh
		slong     tcount;			// total triangles in mesh
		slong     vcount;			// total vertices in mesh
	
		material  * mlistPtr;		// all materials in the mesh
		slong	  mcount;			// total materialse in the mesh

		SphereSet * spheres;		// holds collision bounding information IFF it exists.

		vector3 * geoDataPtr;

		void copyfrom(mesh *m);

private:
	
};

#endif	// _BS_mesh

//================================================================================================================================
//END OF FILE
//================================================================================================================================
