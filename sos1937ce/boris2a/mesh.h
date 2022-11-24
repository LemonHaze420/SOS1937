// $Header$

// $Log$

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
		long     tcount;			// total triangles in mesh
		long     vcount;			// total vertices in mesh
	
		material  * mlistPtr;		// all materials in the mesh
		long	  mcount;			// total materialse in the mesh

		SphereSet * spheres;		// holds collision bounding information IFF it exists.

		vector3 * geoDataPtr;

		void copyfrom(mesh *m);

		LPD3DVERTEXC vertBuf;
		int vbcount;

		LPWORD *indices;
		WORD *indcount;

private:
	
};

#endif	// _BS_mesh
