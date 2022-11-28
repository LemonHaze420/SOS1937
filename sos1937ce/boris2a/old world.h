//================================================================================================================================
// world.h
// -------
//		Note that the methods:
//
//						findClosestIntersectedTriangle
//				and		findClosestTriangle
//
//		have analogies in the class 'hunk'
//
//================================================================================================================================

#ifndef _BS_world
#define _BS_world


class world
{
	public:
		world( char * filename, float scale,
			   float hunksizex, float hunksizez);	// loads world from file
		~world();

		bool	 findClosestIntersectedTriangle( vertex * p, vertex * d, triangle * t, float * distance);
		void	 findClosestTriangle( vertex * p, float * distance );
	
		bool	 getY( vector3 * v, float * yresult, triangle ** resulttriPtr, bool wallsallowed );     // Assumes v->x, v->z inrange (above world)
		bool	 getShadowCoord( vector3 * v, vector3 * dirn, vector3 * shadowCoord, vector3 * triNormal );

		void	  setSun( vector3 * lightDirection,
						  uchar  ambientR, uchar ambientG, uchar ambientB,
						  uchar  incidentR, uchar incidentG, uchar incidentB);

		
		void setSunDirection( vector3 * d );
		void turnSunOn();
		void turnSunOff();

		bool isPointUnderwater( vector3 * v, float * depth, triangle ** resulttriPtr );

//	private:

		//mesh      meshData;			// contains list of triangles, vertices and stuff to describe the mesh.
		model     * worldModel;

		hunk      * hunkList;
		slong     xhunks, zhunks;		// Number of hunks in each direction
		slong     totalhunks;
		float     xhunksize,  zhunksize;			// dimensions of hunks
		float     oneoverxhunksize, oneoverzhunksize;		// used in calculating the viewcone

		racingLine		 * rLine;	// racingLine


		bool      scanTrianglesForXZ( triangle **thistlist, ulong total, vector3 * v, float * yresult, triangle ** resulttriPtr);

		void	 updateLforvertex( vertex * v);

		void updateWorldForSun();

				//	SUN

		vector3			     sunLightDirection;
		colourComponents     sunAmbientColour;
		colourComponents     sunIncidentColour;
		vector3				 lastSunLightDirection;
		bool				 sunOn;
	
};

#endif	// _BS_world

//================================================================================================================================
//END OF FILE
//================================================================================================================================
