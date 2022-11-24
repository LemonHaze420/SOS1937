// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:27:46+00  jjs
// First version that supports the loading of objects.
//

#ifndef _BS_engine
#define _BS_engine


#define	FIRSTLOD	0	// debugging purposes - all instances should be replaced
#define	NOEDGECOLLAPSE	1

struct spriteVertexGroup
{
	vertex	a,b,c,d;
};


class engine
{
	friend class vector3;
	friend class vector;
	friend class car;
	
public:
	
	engine(long ttotal, long vtotal, long ntotal, float physicsTicks);
	~engine();
	
	//	void shadowToGround( float width, float height, matrix3x4 * m, float maxy );
	
				// setting a current camera, display, etc... 
	
	void setCurrentCamera( camera * c );
	void setCurrentDisplay( display * d );
	void setCurrentWorld( world * w )
	{
		currentworld = w;
	}

	
	// disconnect a current ... camera, display, etc...  (leaving non set)
	
	void unsetCurrentCamera()
	{
		currentcamera = 0;
	}

	void unsetCurrentDisplay()
	{
		currentdisplay = 0;
	}

	void unsetCurrentWorld()
	{
		currentworld = 0;
	}
	
	// rendering
	
	void render();			// render the world and all objects to the display surface, return pointer to 
	// the outLists just rendered to
	
	void beginFrame( bool moved, LPARAM heldlParam);
	void insertObject( object * optr );
	void removeObject( object * optr );
	void removeAllObjects();
	
	
	void updateObjectGeometry();		// position all objects correctly (computes transforms needed for physics etc.)
	// and leaves them ready to render!
	
	void objectCollisions( void  callback( object *, ulong wnum ) );
	
	
	
				// - takes filename of texture and returns handle
	
	void __inline rotatePointAroundPoint(float *rx,float *ry,float dx,float dy,float rot)
	{
		float cosa,sina,x,y;
		
#if defined(SH4)
		_SinCosA(&sina, &cosa, rot);
#else
		cosa = (float)cos(rot);
		sina = (float)sin(rot);
#endif
		x = *rx;
		y = *ry;
		
		*rx = dx + sina*(dy-y) + cosa*(x-dx);
		*ry = dy + sina*(x-dx) + cosa*(y-dy);
	}

	void z2Dcalc(float& qz, float zz, float& ww)
	{
		ww = 1.0f/zz;
#if 1
		qz = (1.0f - ww) * zNormalizer;
		if( qz < 0.0f )
			qz = 0.0f;
#else
#define JJSK  3.0f
		if(zz > 1.0f)
			qz = 1.0f - JJSK/(JJSK-1.0f+zz);
		else
			qz = 1.0f  - ww;
#endif
	}
	
	DWORD addTo2Dlist( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		char * filename, ulong flags);
	
				// - takes handle of texture
	void  addTo2Dlist( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		DWORD texHandle);

	void  addTo2Dlistcolor( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		DWORD col);
	
	DWORD addTo2Dlist( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		char * filename, ulong flags, float z);
	
				// - takes handle of texture
	void  addTo2Dlist( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		DWORD texHandle, float z);

	void inline OFFSCREEN(float x3, float y3, float z3, int &mask);
	
	// JCF - rotated versions, single tri version
	
	DWORD addTo2DlistRotated( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		char * filename, ulong flags,float rot,float rx,float ry,float z);
	void  addTo2DlistRotated( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		DWORD texHandle, ulong flags,float rot,float rx,float ry,float z);
	DWORD addTo2DlistRotatedEnvMapped( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		char * filename, ulong flags,float rot,float rx,float ry,float z);
	void  addTo2DlistRotatedEnvMapped( float x, float y, float w, float h,
		float u, float v, float uw, float vh,
		DWORD texHandle, ulong flags,float rot,float rx,float ry,float z);
	
	void addTriTo2Dlist( float x1,float y1,	float u1, float v1,
		float x2,float y2,	float u2, float v2,
		float x3,float y3,	float u3, float v3,
		DWORD texHandle);
	
	
	void lightingOn();
	void lightingOff();
	void shadowOn()
	{
		shadon = true;
	}

	void shadowOff()
	{
		shadon = false;
	}

	void wframeOn()
	{
		wframe=true;
	}

	void wframeOff()
	{
		wframe=false;
	}

	void clipOn()				// enable D3D / Hardware 2D clipping
	{
		clip = true;
	}

	void clipOff()				// disable  .
	{
		clip = false;
	}

	void clipToggle()
	{
		clip = !clip;
	}
	void fogOn();
	void fogOff();
	void setFogColour( ulong fcol )
	{
		fogColour = fcol;
	}
	
	void dumpFrame() 
	{ 
		dumpframe=true;
	}	// dump next frame
	
	
	void startZoom( float distance, float rate );
	float zoompertick;
	float zoomdistance;
	timer zoomtimer;
	
	void driveraidsOn()
	{
		driveraids = true;
	}

	void driveraidsOff()
	{
		driveraids = false;
	}

	bool envmapped2d;

	void envmapped2dOn() { envmapped2d = true; }
	void envmapped2dOff() { envmapped2d = false; }

	//private:
	bool clip;
	bool wframe;
	bool fog;
	bool skyon;
	bool lightSwitch;
	bool envon;									// true = enable environment mapping,  false = disable
	bool deton;
	bool dumpframe;
	bool runsprites;
	DWORD shadow_2d;		// shading value for 2d polys. Initially 0xffffff
	
	uchar mask;
	ulong fogColour;
	
	void add2dvertex( float x, float y, float u, float v, float z,DWORD col=0x00ffffff );
	void add2dvertex( float x, float y, float u, float v,DWORD col=0x00ffffff );
	
	void add2dvertexEnv( float x, float y, float u1, float v1, float u2, float v2, float z,DWORD col=0x00ffffff );
	void add2dvertexEnv( float x, float y, float u1, float v1, float u2, float v2,DWORD col=0x00ffffff);
	
	void  start2Dlist();
	void  start2DlistEnv();
	
	D3DTLVERTEX2 * vertex2dlistEnv;
	D3DTLVERTEX2 * current2dvertexListEnvPtr;

	BorisMaterial * vertex2dmatListEnv;
	BorisMaterial *	currentvertex2dmatlistEnvPtr ;

	D3DTLVERTEX * vertex2dlist;
	D3DTLVERTEX * current2dvertexListPtr;

	BorisMaterial * vertex2dmatList;
	BorisMaterial *	currentvertex2dmatlistPtr ;
	
	display		* currentdisplay;
	camera		* currentcamera;
	world		* currentworld;
//	sky			* currentSky;
	
	object		* objectList;
	
	float randomScale;
	
	void  renderstart();
	
	void  render2Dlist();
	void  render2DlistEnv();
	void  renderSky();
	
	// material and colour values currently set
	
	material * currentSelectedMaterial;
	bool nulltexture;
	
	bool environmentMappedMaterial, detailMappedMaterial;
	
	colourComponents diffuseColour, ambientColour;
	ulong UdiffuseColour;
	
	//
//	void project( vector2P * v2,  vector3 * v, float l , float sl);	// project a 3d coordinate to 2d (with 3d z component)
//	void project( vector2P * v2,  float vx, float vy, float vz, float l , float sl );
//	void zLimitedproject( vector2P * v2,  vector3 * v, float l, float sl );
//	void zLimitedproject( vector2P * v2,  float vx, float vy, float vz, float l , float sl);
	
	void updateDCdata();		// local method called for updating data dependent on both display and camera
	
	// projection data ... dependent upon both the current camera and current display
	
	float projcoeffW, projcoeffH;
	
	// scaling co-efficients for 2D Hud overlays
	
	float coeff2dW, coeff2dH;
	
	// projection data ... dependent upon just the display
	
	float displayWd2deltaX, displayHd2deltaY;
	float displayW, displayH;
	float displayWd2, displayHd2;
	float displayDeltaX, displayDeltaY;
	float displayWM1deltaX, displayHM1deltaY;
	float displayWM1, displayHM1;
	float zNormalizer;				// for turning 3d 'z' into range [0..1)
	float zNormalizer1;
	float displayWd2divprojcoeffW, 	displayHd2divprojcoeffH; 
	
	
	
	// outLists		( lists of triangles to render )
	
//	slong olCount;					// Number of Outlists 
//	outLists ** outListListPtr;		// array of outlists
//	outLists * currentOutListPtr;	// points to outlist being used now
//	slong olCountdown;				// counts down from olCount to 0 then back so that we can advance 'currentOutListPtr'
	
	// globalpctr is incremented when stuff is being rendered to distinguish from previous renderings
	
	ulong globalpctr;
	
	
#if 0	
	// internal routinez needed
	
	void splitEdgeZ( pointdata2d * r, pointdata3d * a, pointdata3d * b, float splitz );
	void splitEdgeX2D( pointdata2d * r, pointdata2d * a, pointdata2d * b, float splitx );
	void splitEdgeY2D( pointdata2d * r, pointdata2d * a, pointdata2d * b, float splitx );
	void doubleSplitEdgeX2D(  pointdata2d * r0,  pointdata2d * r1,
		pointdata2d * a, pointdata2d * b, 
		float splitx0, float splitx1 )
	{
		splitEdgeX2D( r0, a, b, splitx0);
		splitEdgeX2D( r1, a, b ,splitx1);
	}
	void doubleSplitEdgeY2D(  pointdata2d * r0,  pointdata2d * r1,
		pointdata2d * a, pointdata2d * b, 
		float splity0, float splity1 )
	{
		splitEdgeY2D( r0, a, b, splity0);
		splitEdgeY2D( r1, a, b ,splity1);
	}
	void doubleSplitEdgeZ( pointdata2d * r0,  pointdata2d * r1,
		pointdata3d * a, pointdata3d * b, 
		float splitz0, float splitz1)
	{
		splitEdgeZ( r0, a, b, splitz0 );
		splitEdgeZ( r1, a, b, splitz1 );
	}
	void process2DTriangle( pointdata2d * p, pointdata2d * q, pointdata2d * r);	
	void process2DTriangleY( pointdata2d * p, pointdata2d * q, pointdata2d * r);
	void outputTriangle( pointdata2d * p, pointdata2d * q, pointdata2d * r);
	//void processTriangle( triangle * triPtr );
#endif
	void processObjectLit( object * objPtr );
	void processObjectUnlit( object * objPtr );
	void processObjectShadow( object * objPtr );
	
	hunk * thisHunk;
	
	void renderthisHunk( triangle **thistlist, ulong total);			// process the world hunk pointed to by 'thisHunk'
	void renderWorld();
	void renderObjects();
	void performRender();
	
	bool checkRboundboxVisibility(RboundBox * r);		
	
	void zsplitting();
//	void Skyzsplitting();
//	void skysplitEdgeZ( pointdata2d * r, pointdata3d * a, pointdata3d * b, float splitz );
	
	
	triangle * triPtr;
//	pointdata2d projectedp, projectedq, projectedr;
	
				// SpriteFX List					
	
//	spriteFXentry * spriteFXlist;
//	void  renderSpriteFXlist();							// update and render spriteFX's
//	spriteFXentry *  appendSpriteFX( spriteFX * sfx, vector3 * position, ulong * counter, bool fU, bool fV );		// add sprite to engine list
				// sky
	
//	void setCurrentSky( sky * s )
//	{
//		if( (currentSky = s) != NULL )
//			skyon = true;
//	}
	// environment map of sky = an alpha texture
	
	void setCurrentSkyEnvironmentMap( char * filename );
	
	BorisMaterial  skyEnvironment;				// holds texturehandle and flags 
	BorisMaterial  detailMap;
	BorisMaterial	BumpEnv;
	
				// envmapping = flag used during rendering to know whether to calculate a second textures u,v s for
				// environment mapping.
	
	bool envmapping;
	bool detmapping;
	//void getEnvmapUVs(nongeodata * nv1, vertex * v);
	void getEnvmapUVs(nongeodata * nv1, vertex * v, vector3 * p);
	void getDetmapUVs( nongeodata * nv1, vertex * v)
	{
		nv1->u = v->nv1.u * 0.125f;
		nv1->v = v->nv1.v * 0.125f;
	}
	void getDetmapUVs( nongeodata * nv1, nongeodata * v)
	{
		nv1->u = v->u * 0.125f;
		nv1->v = v->v * 0.125f;
	}
	void getDetmapUVs(nongeodata * nv1, vertex * v, vector3 * p)
	{
		nv1->u = v->nv1.u * 0.125f;
		nv1->v = v->nv1.v * 0.125f;
	}
	
	void loadInSpriteList(float worldscale);
	
	//		ulong formHwareKeyAlpha( ulong k,  float f );
	//		ulong formHwareKeyNonAlpha( ulong k,  float f );
	
	ulong formHwareKeyAlpha( ulong k,  float f0, float f1, float f2 );
	ulong formHwareKeyNonAlpha( ulong k,  float f0, float f1, float f2 );

	void enableEnvironmentMapping()
	{
		envon = true;
	}

	void disableEnvironmentMapping()
	{
		envon = false;
	}
	void enableDetailMap()
	{
		deton = true;
	}

	void disableDetailMap()
	{
		deton = false;
	}
	void setCurrentDetailMap( char * filename );
	
	void enableSky()
	{
		skyon = true;
	}

	void disableSky()
	{
		skyon = false;
	}

	
	float oneoverphysicsTicks;
	timer physicsTimer;			// Created at constructor for physics ticks
	
	float zoomz;
	float zoomproportiondone;
	float oneovertotalzoomticks;
	long  zoomtickselapsed;
	bool getZoomStatus()
	{
		return(zoomdone);
	}

	bool zoomdone;
	
	bool shadon;
	bool useRacingline;
	
	ulong uniqueTid;
	
#ifdef FEATURE_ROLLOVERNUMBER
	rollOverNumber * rollOverList;
#endif
				
	bool driveraids;
	bool voodoo;
				
	float cameraLoft;
				
	// display pixel width and height stored as longs
				
	long displayHlong,displayWlong;

	float cameraangle;
	
	void RotatePoints(vertex *pvPtr,vertex *qvPtr,vertex *rvPtr,float x,float y);

	void wheelDirection( long wheelNumber, float angle );
	void showmagnitude( long num, float min, float max, float value );
	void showmagdir( long num, float max, vector3 vect);
				
//	bool doesSpriteFXentryExist( spriteFXentry * e );
				
	int GetLod(class model *m,float z);

	float lod_threshold_scale;

	DWORD Caps;

//	Pool<spriteFXentry> *spritePool;
//	Pool<spriteVertexGroup> *vertexPool;

	void DrawLine(float x1,float y1,float x2,float y2,DWORD col);

	// the material must be unique to each particle in the frame; don't initialise it, it's
	// just used as storage.

//	void DrawParticle(vector3 *v,DWORD col,float psize,material *mat); 
	void renderTriangle(triangle *triPtr);
	void getWorldFromScreen(vector3 *v,float x,float y,float z);
};




#endif	// _BS_engine
