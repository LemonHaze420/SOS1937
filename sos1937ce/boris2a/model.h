// $Header$

// $Log$

#ifndef _BS_model
#define _BS_model


#define	MAXLODS	8

class model {
	
	friend class engine;
	friend class object;
	friend class car;
	friend class SphereSet;
public:
	model();    // char * filename, float scale );
	~model();
	
	//private:
	
	mesh	  *meshData;			// JCFLOD - pointer to an array of meshes
	int		meshct;					// number of lods
	int		highest_visible_mesh;	// meshes above this lod are invisible
	int		minimum_lod;			// minimum permitted lod (default 0)
	float	meshthresholds[MAXLODS];	// distances below which the corresponding
										// mesh is shown
	
	boundBox  boundingBox;  
	vector3   centroid;				// mid position co-ordinate  (best origin of bounding sphere)
	float     boundingRadius;		// radius of bounding sphere at 'centroid' position.
	
	
	char  * filename;
	model * next;
	model * copynext;
};

#endif	// _BS_model
