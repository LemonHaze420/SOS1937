//================================================================================================================================
// engine.cpp
// ----------
//
//================================================================================================================================

#define DETSCALE 8.0f		// scale of detail mapping

#include "BS2all.h"

	//
	// CULLINGEPSILON is the amount needed to compensate for approximate trig. resulting in the transformed normal SEEMING to
	// face backwards when it really faces forwards, and hence culling is inacurate!!!
	//

#define CULLINGEPSILON 1.0f

#define ZFRONTPLANE 1.0f

//
// ttotal = number of triangles in OUTPUT lists
// vtotal = number of vertices in VERTEX lists
//

//extern debugger DBGR;

engine  * boris2;			// The global engine

long occlusionColumn[1280];
long stepTable[3290];

__int64 timing_basecounter;
float   timing_oneOverFrequency;
__int64 timing_frequency;


#define BATCHSIZE 80000
D3DTLVERTEX vertexListD3D[BATCHSIZE];
D3DTLVERTEX2 vertexListD3D2[BATCHSIZE];
#define BATCHSIZEM3 ( BATCHSIZE - 3 )

#ifdef EDITOR
	extern vector3 * treestore;
	extern vector3 * treestorePtr;
	extern ulong   treecount;
	extern object * treeobject;
#endif

extern ulong polygoncounter;

extern void dprintf(char *f, ...);

engine::engine(  bool useSoftware,  slong outListCount, slong ttotal, slong vtotal, slong ntotal, float physicsTicks)
{
	slong i;

	current2dvertexListPtr = NULL;
	shadow_2d = 0xffffff;

	runsprites=true;

	lod_threshold_scale=1.0;

	dumpframe=false;

	cameraangle = 0;
	cameraLoft = 0;

	uniqueTid = 0;

	initialiseArcHandles();

	outLists ** myolPtr;

	currentdisplay    = NULL;
    currentcamera     = NULL;
    currentworld      = NULL;
	objectList		  = NULL;
    spriteFXlist	  = NULL; 

	lightSwitch = false;
	skyon = false;
	envon = false;
	deton = true;
	shadon = false;
	wframe = false;
	fog = false;
	clip = false;
	envmapped2d = false;
							
	zoomz = 0;
	zoomdone = true;

	vertex2dlist	= new D3DTLVERTEX[ MAX2DENTRIES * 6 ];
	vertex2dmatList = new BorisMaterial[ MAX2DENTRIES * 3 ];

	vertex2dlistEnv	= new D3DTLVERTEX2[ MAX2DENTRIES * 6 ];
	vertex2dmatListEnv = new BorisMaterial[ MAX2DENTRIES * 3 ];

	randomScale =  (RECIPROCAL( ((float)(RAND_MAX))  )) ;		// for scaling random float values

			// Create a list of outLists

	olCount = outListCount;

	myolPtr = outListListPtr = new outLists *[ olCount ];

	for(i = olCount; i--; )
		(*myolPtr++) =  new outLists( ttotal, vtotal, ntotal );

	currentOutListPtr =  * outListListPtr;

	olCountdown = olCount;
	
	// set timing

	QueryPerformanceCounter((LARGE_INTEGER *)&timing_basecounter);

	QueryPerformanceFrequency((LARGE_INTEGER *)&timing_frequency);

	if(timing_frequency == 0)
		fatalError("QueryPerformanceFrequency returned zero!");

	timing_oneOverFrequency = RECIPROCAL( (float)(timing_frequency) );

	//
	// set global up counter (used to see 'what' is processed on a particular part-frame - cheaper than
	// initialising whole arrays of flags per frame!
	//

	globalpctr = 0;
		
	//
	// set up fast square root and inverse square root tables.
	//

	BuildSqrtTable();

	//
	// Create physics timer
	//

	physicsTimer.setTimer(physicsTicks);
	oneoverphysicsTicks = RECIPROCAL( physicsTicks);

	//
	// initialise the pointer to any rollover list
	//
	fogColour = 0x0FFFFFF;

	//
	//
	//

	#ifdef FEATURE_ROLLOVERNUMBER
		rollOverList = NULL;
	#endif

	driveraidsOff();

	for(i = 1; i < 3280; i++)
		stepTable[i] = -((1 << 16) / i);



	dprintf("allocating pools");

	spritePool = new Pool<spriteFXentry> (1024);
	vertexPool = new Pool<spriteVertexGroup> (1024);

	dprintf("pools allocated");
}

engine::~engine()
{
	slong i;
	outLists ** myolPtr;

	// remove outLists 

	myolPtr = outListListPtr;

	for( i = olCount; i--; myolPtr++)
		delete *myolPtr;
	
	delete[] outListListPtr;
	delete[] vertex2dlist;
	delete[] vertex2dmatList;
	delete[] vertex2dlistEnv;
	delete[] vertex2dmatListEnv;

	delete spritePool;
	delete vertexPool;
}

void engine::fogOn()
{
	renderFog(true);
	SetFogColor( fogColour );
	fog = true;
}

void engine::fogOff()
{
	renderFog(false);
	fog = false;
}

void engine::startZoom( float distance, float timeofzoom )
{
	#define ZOOMTICKS 10000
	#define ZOOMTICKSF 10000.0f

	if(zoomdone)
	{
		zoomdistance = distance;
		oneovertotalzoomticks = RECIPROCAL( ZOOMTICKSF * timeofzoom );
		zoomtickselapsed = 0;
		zoomproportiondone = 0;

		zoompertick  = (distance - zoomz) * oneovertotalzoomticks;
		zoomtimer.setTimer( ZOOMTICKS );
		zoomdone = false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// setCurrentSkyEnvironmentMap
// ---------------------------
//
// Load up a texture used for the sky environment mapping
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void engine::setCurrentSkyEnvironmentMap( char * filename )
{
	skyEnvironment.textureHandle =(getTexturePtr(filename, true ))->textureHandle;
	skyEnvironment.flags = 0;
	envon = true;
}

void engine::setCurrentDetailMap( char * filename )
{
	detailMap.textureHandle =(getTexturePtr(filename, true ))->textureHandle;
	detailMap.flags = 0;
	deton = true;
}

//- - - - - - - - - - - - - - - - - 
// updateDCdata
// ------------
//		- local method used to update information dependent on both the camera and display characteristics
//- - - - - - - - - - - - - - - - -

void engine::updateDCdata()
{
	if( (currentcamera!=0)  && (currentdisplay!=0) )
	{
		//
		// A camera and display both exist ... so update DC data.
		//

		projcoeffW = currentcamera->zeye * ( coeff2dW = displayW * CONST_RECIPROCAL(BASERES_WIDTH));
		projcoeffH = currentcamera->zeye * ( coeff2dH = displayH * CONST_RECIPROCAL(BASERES_HEIGHT));
	
		displayWd2divprojcoeffW = displayWd2 * RECIPROCAL( projcoeffW );
		displayHd2divprojcoeffH = displayHd2 * RECIPROCAL( projcoeffH );
	}
}

//- - - - - - - - - - - - - - - - - 
//- - - - - - - - - - - - - - - - - 

void engine::updateObjectGeometry()
{

	object * objPtr = objectList;
	while (objPtr != 0 )		// for each object in the list
	{
		objPtr->inheritTransformations();// &id3x4 );
		objPtr = objPtr->next;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// find each object collision (returns TRUE if found, FALSE = end operation, non-found)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define MINMAX( v )   \
	if ( v.x < xl ) xl = v.x;  \
	else  \
		if (v.x > xh) xh = v.x; \
	if ( v.y < yl ) yl = v.y; \
	else  \
		if (v.y > yh) yh = v.y;\
	if ( v.z < zl ) zl = v.z; \
	else  \
		if (v.z > zh) zh = v.z;



void engine::objectCollisions( void callback( object *, ulong wnum ))
{
	object * nextCollisionObject = objectList;

	while( nextCollisionObject )
	{
		if((nextCollisionObject->collisionFlags & COLLIDE_COLLIDABLE))
		{
			//
			//- establish hunk limits of object under current transform
			//

			RboundBox * bboxPtr = &nextCollisionObject->transformedBoundingBox;
			float xl,xh, yl,yh, zl,zh;
			float hx, hz;
			slong loHunkX, hiHunkX, loHunkZ, hiHunkZ, xx, zz;

			xl = xh = bboxPtr->v000.x;
			yl = yh = bboxPtr->v000.y;
			zl = zh = bboxPtr->v000.z;
			MINMAX( bboxPtr->v001 );
			MINMAX( bboxPtr->v010 );
			MINMAX( bboxPtr->v011 );
			MINMAX( bboxPtr->v100 );
			MINMAX( bboxPtr->v101 );
			MINMAX( bboxPtr->v110 );
			MINMAX( bboxPtr->v111 );

			// collision handling for the world, use LOD 0
			mesh * meshData = currentworld->worldModel->meshData+0;
		
			FASTFTOL(&loHunkX , (xl - meshData->MinCoordX) * currentworld->oneoverxhunksize - 0.5f) ;
			FASTFTOL(&hiHunkX , (xh - meshData->MinCoordX) * currentworld->oneoverxhunksize + 0.5f) ;

			FASTFTOL(&loHunkZ , (zl - meshData->MinCoordZ) * currentworld->oneoverzhunksize - 0.5f) ;
			FASTFTOL(&hiHunkZ , (zh - meshData->MinCoordZ) * currentworld->oneoverzhunksize + 0.5f) ;
		
			if( hiHunkX >= currentworld->xhunks) 
				hiHunkX = currentworld->xhunks-1;
			if( hiHunkZ >= currentworld->zhunks) 
				hiHunkZ = currentworld->zhunks-1;

			float dx = (xh - xl)*0.5f;
			float dy = (yh - yl)*0.5f;
			float dz = (zh - zl)*0.5f;
			float dsqr = dx*dx + dy*dy + dz*dz;
			
			//
			// Now examine each hunk within this range
			//
  
			dx += xl;
			dy += yl;
			dz += zl;

			hunk * hunkPtrX, * hunkPtrZ;

			hx = loHunkX * currentworld->xhunksize + meshData->MinCoordX ;
			hunkPtrX = currentworld->hunkList + loHunkX * currentworld->zhunks;

			bool cont = true;
	
			xx = 1 + hiHunkX - loHunkX;
			while( (xx!=0) && cont)
			{
				xx--;

				hz = loHunkZ * currentworld->zhunksize + meshData->MinCoordZ; //minz;
				hunkPtrZ = hunkPtrX + loHunkZ;
				zz = 1 + hiHunkZ - loHunkZ;

				while( (zz!=0) && cont)
				{
					zz--;
					//
					// hunkPtrZ points at hunk to examine wall triangles
					//

					slong i;
					triangle ** tlistPtr = hunkPtrZ->tlistPtr_W;
					triangle * triPtr;
					i = hunkPtrZ->tcount_W;
					while( (i!=0) && cont)
					{
						i--;
						triPtr = * tlistPtr;
								
						//
						// For this triangle, for every bounding box corner determine which side of the triangle
						// the corner lies upon - as soon as any two signs are different then we know that the
						// triangle PLANE is inside the bounding box. At that point we then need to determine
						// if the plane

						vector3 * l = triPtr->p.v->geoCoord;
						vector3 * m = triPtr->q.v->geoCoord;
						vector3 * n = triPtr->r.v->geoCoord;

						//
						// We must now see if any part of this triangle is inside the transformed bounding box
						// - this means a collision has occured!!
						//  
						// FIRST TEST - 'bounding box OF bounding box' ie. does triangle fall inside the aligned box
						//				 which encompasses the bounding box? This test is a simple Sutherland Cohen
						//				 test.
						//


						uchar lmask = (( l->x < xl ) ? 1  : (( l->x > xh ) ? 2 : 0)) |
									  	  (( l->z < zl ) ? 4  : (( l->z > zh ) ? 8 : 0)) |
										  (( l->y < yl ) ? 16 : (( l->y > yh ) ? 32 : 0));

						uchar mmask = (( m->x < xl ) ? 1  : (( m->x > xh ) ? 2 : 0)) |
									  	  (( m->z < zl ) ? 4  : (( m->z > zh ) ? 8 : 0)) |
										  (( m->y < yl ) ? 16 : (( m->y > yh ) ? 32 : 0));

						uchar nmask = (( n->x < xl ) ? 1  : (( n->x > xh ) ? 2 : 0)) |
									  	  (( n->z < zl ) ? 4  : (( n->z > zh ) ? 8 : 0)) |
										  (( n->y < yl ) ? 16 : (( n->y > yh ) ? 32 : 0));

						if ((lmask & mmask & nmask)==0)
						{
							//
							// Now lets try a similar test BUT we make a box around the
							// triangle and test the transformedBoundingBox co-ordinates
							// against it!
							//

							xl = triPtr->minLimit.x;
							xh = triPtr->maxLimit.x;

							yl = triPtr->minLimit.y;
							yh = triPtr->maxLimit.y;
						
							zl = triPtr->minLimit.z;
							zh = triPtr->maxLimit.z;

						
#define SCOHENMASK( v ) \
	((v.x < xl ) ? 1 : (( v.x > xh ) ? 2 : 0)) |\
	((v.y < yl ) ? 4 : (( v.y > yh ) ? 8 : 0)) |\
	((v.z < zl ) ? 16: (( v.z > zh ) ? 32 : 0))

							uchar Kmask;

							if( Kmask = SCOHENMASK( bboxPtr->v000 ) )
								if( Kmask &= SCOHENMASK( bboxPtr->v001 ) )
									if( Kmask &= SCOHENMASK( bboxPtr->v010 ) )
										if( Kmask &= SCOHENMASK( bboxPtr->v011 ) )
											if( Kmask &= SCOHENMASK( bboxPtr->v100 ) )
												if( Kmask &= SCOHENMASK( bboxPtr->v101 ) )
													if( Kmask &= SCOHENMASK( bboxPtr->v110 ) )
														Kmask &= SCOHENMASK( bboxPtr->v111 );


							if (Kmask == 0)
							{
								float rsqr = dx * triPtr->A + 
										  dy * triPtr->B + 	
									      dz * triPtr->C +
										  triPtr->planeD;

								rsqr *= rsqr * triPtr->ABC;

								if (rsqr < dsqr * 0.2)
								{
// TEMP.
// Find closest corner of bounding box around object and return a pointer to the co-ordinate
//

									float mindistance = 99999999.0f;
									ulong wnum;


									rsqr = bboxPtr->v000.x * triPtr->A + 
										bboxPtr->v000.y * triPtr->B + 	
										bboxPtr->v000.z * triPtr->C + 
										triPtr->planeD; 
									if (rsqr < mindistance ) 
									{
										mindistance = rsqr;
										wnum = 0;
									}
									rsqr = bboxPtr->v001.x * triPtr->A + 
										bboxPtr->v001.y * triPtr->B + 	
										bboxPtr->v001.z * triPtr->C + 
										triPtr->planeD; 
									if (rsqr < mindistance ) 
									{
										mindistance = rsqr;
										wnum = 2;
									}
	
									rsqr = bboxPtr->v100.x * triPtr->A + 
										bboxPtr->v100.y * triPtr->B + 	
										bboxPtr->v100.z * triPtr->C + 
										triPtr->planeD; 
									if (rsqr < mindistance ) 
									{
										mindistance = rsqr;
										wnum = 1;
									}
									rsqr = bboxPtr->v101.x * triPtr->A + 
										bboxPtr->v101.y * triPtr->B + 	
										bboxPtr->v101.z * triPtr->C + 
										triPtr->planeD; 
									if (rsqr < mindistance ) 
									{
										mindistance = rsqr;
										wnum = 3;
									}

									callback( nextCollisionObject, wnum);															
									cont = false;
								}
							}
						}

						tlistPtr++;
					}

					hz += currentworld->zhunksize;
					hunkPtrZ++;
				}

				hx += currentworld->xhunksize;
				hunkPtrX += currentworld->zhunks;
			}
		}
		nextCollisionObject = nextCollisionObject->next;
	}
}

//
//   F( 1 - D / z  ) / ( F - D )
//


//================================================================================================================================
// produce a projected vertex from the 3d co-oridnate vertex.
//		- automatically produces the SutherlandCohen mask against the current display since this will always be used.
//
//================================================================================================================================

void engine::project( vector2P * v2,  vector3 * v, float l, float sl )
{

	z2Dcalc(v2->z, v->z, v2->w);		
	
	float vw = RECIPROCAL( v->z );

	v2->x	= v->x * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - v->y * vw * projcoeffH;

	v2->v3 = *v;
	v2->l   = l;
	v2->specularl = sl;

	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

}

void engine::project( vector2P * v2,  float vx, float vy, float vz, float l, float sl )
{

	z2Dcalc( v2->z, vz, v2->w);

	float vw = RECIPROCAL( vz );

	v2->x	= vx * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - vy * vw * projcoeffH;

	v2->v3.x = vx;
	v2->v3.y = vy;
	v2->v3.z = vz;

	v2->l = l;
	v2->specularl = sl;

	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
}

//----
//
//----

void engine::zLimitedproject( vector2P * v2,  vector3 * v, float l, float sl )
{

	z2Dcalc(v2->z, v->z, v2->w);				

	float vw = RECIPROCAL( v->z );

	v2->x	= v->x * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - v->y * vw * projcoeffH;

	v2->v3 = *v;
	v2->l   = l;
	v2->specularl = sl;
	
	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

}

void engine::zLimitedproject( vector2P * v2,  float vx, float vy, float vz, float l, float sl )
{

	z2Dcalc(v2->z, vz, v2->w);

	float vw = RECIPROCAL( vz );

	v2->x	= vx * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - vy * vw * projcoeffH;

	v2->v3.x = vx;
	v2->v3.y = vy;
	v2->v3.z = vz;

	v2->l = l;
	v2->specularl = sl;
		
	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
}

//================================================================================================================================
// setCurrentCamera
// ----------------
//		- define the camera being used by the engine.
//================================================================================================================================

void engine::setCurrentCamera( camera * c )
{
	currentcamera = c;

#ifdef MAXIMUMREARZ
	zNormalizer1 = RECIPROCAL( MAXIMUMREARZ - ZFRONTPLANE) ;			
	zNormalizer =  MAXIMUMREARZ * zNormalizer1 ;  
	zNormalizer1 *= 8191.0f;
#else
	zNormalizer1 = RECIPROCAL( currentcamera->zbackplane -  ZFRONTPLANE) ;
	zNormalizer =  currentcamera->zbackplane * zNormalizer1 ; 
	zNormalizer1 *= 8191.0f;
#endif

	updateDCdata();
}

//================================================================================================================================
// setCurrentDisplay
// -----------------
//		- define the display being used by the camera
//================================================================================================================================

void engine::setCurrentDisplay( display * d )
{
	currentdisplay = d;
	
	displayDeltaX = (float)(d->deltaX );
	displayDeltaY = (float)(d->deltaY );
	displayHlong = d->pixelheight;
	displayWlong = d->pixelwidth;
	displayH =  (float)d->pixelheight;
	displayW =	(float)d->pixelwidth;
	displayWM1 = (displayW-0.001f);			// Maximum value is very close too but less than the actual screen width.
	displayHM1 = (displayH-0.001f);			// ditto for screen height.
	displayHM1deltaY = displayHM1 + d->deltaY;
	displayWM1deltaX = displayWM1 + d->deltaX;
	displayWd2 = (float)( d->pixelwidth  >> 1);
	displayHd2 = (float)( d->pixelheight >> 1);
	displayHd2deltaY = displayHd2 + d->deltaY;
	displayWd2deltaX = displayWd2 + d->deltaX;
	
	updateDCdata();
}


outTriangle * outTriangleNextPtr;
vector2P    * outNextVector2Pptr;
nongeodata  * outNextNonGeoptr;
material * currentmtl;

#define TRIMOFF( t ) \
	if ( t < 0.0f ) t = 0.0f; \
	if ( t > 1.0f ) t = 1.0f; 

//================================================================================================================================
// splitEdgeZ
// -----------
//		split a 3D edge at a z-plane and return the 2D PROJECTED data.   
//		- This will create a 'vector2P' and a 'nongeodata' for 'r'
//================================================================================================================================

void engine::splitEdgeZ( pointdata2d * r, pointdata3d * a, pointdata3d * b, float splitz )
{

	float t;
	
	vector3 * avg = &a->v->transformed;
	vector3 * bvg = &b->v->transformed;

	// calculate parametric co-efficient of line at split (and set z at split )

	t = (float)(( splitz - avg->z) * RECIPROCAL( bvg->z - avg->z ));

	//	calculate x and y at split point

	project(  outNextVector2Pptr, (avg->x + (float)(t * ( bvg->x - avg->x ))),
								  (avg->y + (float)(t * ( bvg->y - avg->y ))), 
								  splitz,
								  a->v->l + (float)(t * ( b->v->l - a->v->l )),
								  a->v->specularl + (float)(t * ( b->v->specularl - a->v->specularl )));

	// now split non-goemetric data to create  r->nv...  (we must create a new entry of nongeodata)

	outNextNonGeoptr->u = a->nv.u + (float)(t * ( b->nv.u - a->nv.u ));
	outNextNonGeoptr->v = a->nv.v + (float)(t * ( b->nv.v - a->nv.v ));

   	r->v = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
	if( envmapping )
		getEnvmapUVs( &r->nv1, a->v,  &r->v->v3);	 	
	if( detmapping )
		getDetmapUVs( &r->nv1, r->nv);
}

//================================================================================================================================
// skysplitEdgeZ
// -----------
//	
//================================================================================================================================

void engine::skysplitEdgeZ( pointdata2d * r, pointdata3d * a, pointdata3d * b, float splitz )
{

	float t;
	
	vector3 * avg = &a->v->transformed;
	vector3 * bvg = &b->v->transformed;

	// calculate parametric co-efficient of line at split (and set z at split )

	t = (float)(( splitz - avg->z) * RECIPROCAL( bvg->z - avg->z ));

	//	calculate x and y at split point

	zLimitedproject(  outNextVector2Pptr, (avg->x + (float)(t * ( bvg->x - avg->x ))),
								  (avg->y + (float)(t * ( bvg->y - avg->y ))), 
								  splitz,
								  a->v->l + (float)(t * ( b->v->l - a->v->l )),
								  a->v->specularl + (float)(t * ( b->v->specularl - a->v->specularl )));

	// now split non-goemetric data to create  r->nv...  (we must create a new entry of nongeodata)

	outNextNonGeoptr->u = a->nv.u + (float)(t * ( b->nv.u - a->nv.u ));
	outNextNonGeoptr->v = a->nv.v + (float)(t * ( b->nv.v - a->nv.v ));

	r->v = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
}

			
//================================================================================================================================
// splitEdgeX2D
// ------------
//		split a 2D edge at at x-plane  
//		- This will create a 'vector2P' and a 'nongeodata' for 'r'
//================================================================================================================================

void engine::splitEdgeX2D( pointdata2d * r, pointdata2d * a, pointdata2d * b, float splitx)
{

	float t;
	nongeodata * rnv1, * anv1, * bnv1;
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;

	// calculate parametric co-efficient of line at split

	t = (float)(((outNextVector2Pptr->x = splitx) - av->x) * RECIPROCAL( bv->x - av->x ));

	outNextVector2Pptr->y =  av->y + (float)(t * ( bv->y - av->y ));

	//
	// calculate the 3d parametric co-efficient
	// and split 3d values  (z, u,v,l )
	//		( we must maintain the 3d x,y values because we shall split on Y next
	//

	float Rw = (float)(projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y ));
	float Rh = (float)(projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX ));

	t = (float)(( Rh * av->v3.y - Rw * av->v3.x ) * 
		RECIPROCAL( (bv->v3.x - av->v3.x) * Rw - (bv->v3.y - av->v3.y) * Rh ));

	outNextVector2Pptr->v3.x = av->v3.x + (float)(t * ( bv->v3.x - av->v3.x ));	
	outNextVector2Pptr->v3.y = av->v3.y + (float)(t * ( bv->v3.y - av->v3.y ));	
	outNextVector2Pptr->v3.z = av->v3.z + (float)(t * ( bv->v3.z - av->v3.z ));	
		
	z2Dcalc(outNextVector2Pptr->z,outNextVector2Pptr->v3.z, outNextVector2Pptr->w ) ;	

	outNextNonGeoptr->u = anv->u + (float)(t * ( bnv->u - anv->u ));
	outNextNonGeoptr->v = anv->v + (float)(t * ( bnv->v - anv->v ));
	outNextVector2Pptr->l = av->l + (float)(t * ( bv->l - av->l ));
		
	// We actually DO need to calculate the partial SC mask in Y since Y-clipping is done AFTER x-clipping
	// (we dont need to do the X mask)

	outNextVector2Pptr->SCmaskY = (( outNextVector2Pptr->y < displayDeltaY ) ? 1 : ( (outNextVector2Pptr->y > displayHM1deltaY) ? 2 : 0 ));

	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
    if( envmapping || detmapping )
	{
		rnv1 = &r->nv1;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		rnv1->u =  anv1->u + (float)(t * ( bnv1->u - anv1->u ));
		rnv1->v =  anv1->v + (float)(t * ( bnv1->v - anv1->v ));
	}
}


//================================================================================================================================
// splitEdgeY2D
// ------------
//		split a 2D edge at at y-plane  
//		- This will create a 'vector2P' and a 'nongeodata' for 'r'
//================================================================================================================================

void engine::splitEdgeY2D( pointdata2d * r, pointdata2d * a, pointdata2d * b, float splity )
{
	
	float t;
	nongeodata * rnv1, * anv1, * bnv1;
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;

	// calculate parametric co-efficient of line at split

	t = (float)((  (outNextVector2Pptr->y = splity) - av->y) * RECIPROCAL( bv->y - av->y ));

	//	calculate x at split point

	outNextVector2Pptr->x =  av->x + (float)(t * ( bv->x - av->x ));

	//
	// calculate the 3d parametric co-efficient
	// and split 3d values  (z, u,v,l )
	//		( we must maintain the 3d x,y values because we shall split on Y next
	//

	float Rw = (float)(projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y ));
	float Rh = (float)(projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX ));

	t = (float)(( Rh * av->v3.y - Rw * av->v3.x ) *
		RECIPROCAL( (bv->v3.x - av->v3.x) * Rw - (bv->v3.y - av->v3.y) * Rh ));

	outNextVector2Pptr->v3.x = av->v3.x + (float)(t * ( bv->v3.x - av->v3.x ));	
	outNextVector2Pptr->v3.y = av->v3.y + (float)(t * ( bv->v3.y - av->v3.y ));	
	outNextVector2Pptr->v3.z = av->v3.z + (float)(t * ( bv->v3.z - av->v3.z ));	
		
	z2Dcalc(outNextVector2Pptr->z, outNextVector2Pptr->v3.z, outNextVector2Pptr->w );	

	outNextNonGeoptr->u = anv->u + (float)(t * ( bnv->u - anv->u ));
	outNextNonGeoptr->v = anv->v + (float)(t * ( bnv->v - anv->v ));
	outNextVector2Pptr->l = av->l + (float)(t * ( bv->l - av->l ));

	// NOTE that we have no need for setting local SCmaskX or SCmaskY here..

	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;

	if( envmapping || detmapping )
	{
		rnv1 = &r->nv1;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		rnv1->u =  anv1->u + (float)(t * ( bnv1->u - anv1->u ));
		rnv1->v =  anv1->v + (float)(t * ( bnv1->v - anv1->v ));
	}
}

inline bool windingOrder( vector2P *p, vector2P *q, vector2P * r)
{
	return( ((q->x - p->x) * (r->y - p->y))  >=  ((r->x - p->x) * (q->y - p->y)) );
}

//================================================================================================================================
//outputTriangle
//--------------
//				Append a completed 2D triangle to the output list. This triangle is entrirely contained in screen space.
//
//================================================================================================================================

	
void engine::outputTriangle( pointdata2d * p, pointdata2d * q, pointdata2d * r)		
{
	outTriangleNextPtr->p = *p;			// copy both 'v' and 'nv' POINTERS across.
	outTriangleNextPtr->q = *q;
	outTriangleNextPtr->r = *r;

	outTriangleNextPtr->mtl = currentmtl;	// set curentmtl to that  of the triangle

	outTriangleNextPtr++;			// advance to next triangle 'output' triangle
}

//================================================================================================================================
//process2DTriangleY
//------------------
//
//	Y clip a triangle in 2D to produce the final 2D triangles, which are then appended to the output list with the routine:
//  'outputTriangle'.
//
//================================================================================================================================

#define SY4_A(a,b, c,d )\
	doubleSplitEdgeY2D( &Ysplit0, &Ysplit1,  c, d, displayDeltaY, displayHM1deltaY );\
	doubleSplitEdgeY2D( &Ysplit2, &Ysplit3,  a, b, displayHM1deltaY, displayDeltaY );\
	outputTriangle( &Ysplit3, &Ysplit2, &Ysplit0);\
	outputTriangle( &Ysplit1, &Ysplit0, &Ysplit2);
#define SY4_B(a,b, c,d )\
	doubleSplitEdgeY2D( &Ysplit0, &Ysplit1,  c, d, displayHM1deltaY, displayDeltaY );\
	doubleSplitEdgeY2D( &Ysplit2, &Ysplit3,  a, b, displayDeltaY, displayHM1deltaY );\
	outputTriangle( &Ysplit3, &Ysplit2, &Ysplit0);\
	outputTriangle( &Ysplit1, &Ysplit0, &Ysplit2);

void engine::process2DTriangleY( pointdata2d * p, pointdata2d * q, pointdata2d * r)		
{
	static pointdata2d Ysplit0, Ysplit1, Ysplit2, Ysplit3;

	switch( (p->v->SCmaskY << 4) |  (q->v->SCmaskY << 2) | (r->v->SCmaskY ) )
	{
	   
		case 0:	// 00 00 00 .... 
		    outputTriangle( p, q, r );
 		    break;

		case 21:		// 01 01 01 = completely off left - IGNORE
		case 42:		// 10 10 10 = completely off right - IGNORE
			break;

		case 1: // 00 00 01 ...  'r' is off top (y<0)
			splitEdgeY2D( &Ysplit0,  q, r, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayDeltaY);										 
			outputTriangle( p, q, &Ysplit1 );
			outputTriangle( q, &Ysplit0, &Ysplit1 );
			break;

		case 2: // 00 00 10 ...  'r' is off bottom (y>displayHM1deltaY)
			splitEdgeY2D( &Ysplit0,  q, r, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayHM1deltaY);										 
			outputTriangle( p, q, &Ysplit1 );
			outputTriangle( q, &Ysplit0, &Ysplit1 );
			break;
		
		case  4:		// 00 01 00	=  
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayDeltaY);										 
			outputTriangle( r, p, &Ysplit0);
			outputTriangle( r, &Ysplit0, &Ysplit1 );
			break;

		case  8:		// 00 10 00 =   
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY );
			splitEdgeY2D( &Ysplit1,  q, r, displayHM1deltaY);										 
			outputTriangle( r, p, &Ysplit0);
			outputTriangle( r, &Ysplit0, &Ysplit1 );
			break;

		case 16:		// 01 00 00	=   
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayDeltaY);										 
			outputTriangle( q, r, &Ysplit0);
			outputTriangle( r, &Ysplit1, &Ysplit0 );
			break;

		case 32:		// 10 00 00	=	
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayHM1deltaY);										 
			outputTriangle( q, r, &Ysplit0);
			outputTriangle( r, &Ysplit1, &Ysplit0 );
			break;

		case  5:		// 00 01 01 
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  p, r, displayDeltaY);										 
			outputTriangle( p, &Ysplit0, &Ysplit1);
			break;

		case 17:		// 01 00 01 
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayDeltaY);										 
			outputTriangle( q, &Ysplit1, &Ysplit0);
			break;

		case 20:		// 01 01 00 
			splitEdgeY2D( &Ysplit0,  r, p, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayDeltaY);										 
			outputTriangle( r, &Ysplit0, &Ysplit1);
			break;


		case 10:		// 00 10 10	
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  p, r, displayHM1deltaY);										 
			outputTriangle( p, &Ysplit0, &Ysplit1);
			break;

		case 34:		// 10 00 10 =  'p' and 'r' infront of z==1
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayHM1deltaY);										 
			outputTriangle( q, &Ysplit1, &Ysplit0);
			break;

		case 40:		// 10 10 00 =  'p' and 'q' infront of z==1
			splitEdgeY2D( &Ysplit0,  r, p, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayHM1deltaY);										 
			outputTriangle( r, &Ysplit0, &Ysplit1);
			break;

		case  6:		// 00 01 10 =  'q' beyond back plane,  'r' in front of z==1
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayHM1deltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, q, r, displayHM1deltaY,  displayDeltaY);
			outputTriangle( p, &Ysplit0, &Ysplit3);
			outputTriangle( p, &Ysplit3, &Ysplit2);
			outputTriangle( p, &Ysplit2, &Ysplit1);
			break;

		case  9:        // 00 10 01	= 'q' in front of z==1,		'r' beyond back plane
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  r, p, displayDeltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, q, r,  displayDeltaY, displayHM1deltaY);
			outputTriangle( p, &Ysplit0, &Ysplit3);
			outputTriangle( p, &Ysplit3, &Ysplit2);
			outputTriangle( p, &Ysplit2, &Ysplit1);
			break;

		case 18:		// 01 00 10 =  'p' beyond backplane,  'r' in front of z==1
			splitEdgeY2D( &Ysplit0,  p, q, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayHM1deltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, r, p, displayHM1deltaY,  displayDeltaY);
			outputTriangle( q, &Ysplit1, &Ysplit2);
			outputTriangle( q, &Ysplit2, &Ysplit3);
			outputTriangle( q, &Ysplit3, &Ysplit0);
			break;

		case 33:		// 10 00 01	=  'p' in front of z==1,  'r' beyond backplane
			splitEdgeY2D( &Ysplit0,  p, q, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayDeltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, r, p,  displayDeltaY, displayHM1deltaY);
			outputTriangle( q, &Ysplit1, &Ysplit2);
			outputTriangle( q, &Ysplit2, &Ysplit3);
			outputTriangle( q, &Ysplit3, &Ysplit0);
			break;

		case 24:		// 01 10 00 =  'p' beyond backplane,  'q' in front of z==1
			splitEdgeY2D( &Ysplit0,  r, p, displayDeltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayHM1deltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, p, q, displayHM1deltaY,  displayDeltaY);
			outputTriangle( r, &Ysplit0, &Ysplit3);
			outputTriangle( r, &Ysplit3, &Ysplit2);
			outputTriangle( r, &Ysplit2, &Ysplit1);
			break;

		case 36:		// 10 01 00 =  'p' in front of z==1,  'q' beyond backplane
			splitEdgeY2D( &Ysplit0,  r, p, displayHM1deltaY);
			splitEdgeY2D( &Ysplit1,  q, r, displayDeltaY);
			doubleSplitEdgeY2D( &Ysplit2, &Ysplit3, p, q,  displayDeltaY, displayHM1deltaY);
			outputTriangle( r, &Ysplit0, &Ysplit3);
			outputTriangle( r, &Ysplit3, &Ysplit2);
			outputTriangle( r, &Ysplit2, &Ysplit1);
			break;

		case 22:		// 01 01 10 
			SY4_A( r,p,   q,r);
			break;
		case 41:		// 10 10 01	
			SY4_B( r,p,   q,r);
			break;
		case 25:		// 01 10 01	
			SY4_A( q,r,   p,q);
			break;
 		case 38:		// 10 01 10 	
			SY4_B( q,r,   p,q);
			break;
		case 26:		// 01 10 10  
			SY4_A( r,p,   p,q);
			break;
		case 37:		// 10 01 01 
			SY4_B( r,p,   p,q);
			break;

	}
}



#define wSZ4_A(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, ZFRONTPLANE, currentcamera->zbackplane);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, currentcamera->zbackplane, ZFRONTPLANE);\
	Wprocess2DTriangle( &split3, &split2, &split0);\
	Wprocess2DTriangle( &split1, &split0, &split2);
#define wSZ4_B(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, currentcamera->zbackplane, ZFRONTPLANE);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, ZFRONTPLANE, currentcamera->zbackplane);\
	Wprocess2DTriangle( &split3, &split2, &split0);\
	Wprocess2DTriangle( &split1, &split0, &split2);


#define SZ4_A(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, ZFRONTPLANE, currentcamera->zbackplane);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, currentcamera->zbackplane, ZFRONTPLANE);\
	process2DTriangle( &split3, &split2, &split0);\
	process2DTriangle( &split1, &split0, &split2);
#define SZ4_B(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, currentcamera->zbackplane, ZFRONTPLANE);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, ZFRONTPLANE, currentcamera->zbackplane);\
	process2DTriangle( &split3, &split2, &split0);\
	process2DTriangle( &split1, &split0, &split2);


//================================================================================================================================
// Macro allowing 2D winding order culling
//================================================================================================================================

#define Wprocess2DTriangle( p, q, r ) \
        if(currentmtl->bm.flags & MAT_SHADOW)\
        { \
                process2DTriangle( p , q, r ); \
        } else \
        {       \
                if (windingOrder( (p)->v, (q)->v, (r)->v )) \
                        process2DTriangle( p , q, r ); \
        }


void engine::zsplitting()
{

	static   pointdata2d split0, split1, split2, split3;

	if( currentmtl->bm.flags & MAT_DOUBLESIDED )
	{
		switch( mask ) // MUST ZCLIP!!!   cant do ... (clip)?0:mask)
		{
			case 0:			// 000000 = completely between z-planes: NO SPLIT
				process2DTriangle(&projectedp, &projectedq, &projectedr);
				break;

			case 21:		// 01 01 01 = completely beyond back plane - IGNORE
			case 42:		// 10 10 10 = completely infront of front plane - IGNORE
				break;
			case  1:		// 00 00 01 =   'r' is beyobnd back plane - split into 2 triangles
				splitEdgeZ( &split0,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);										 
				process2DTriangle( &projectedp, &projectedq, &split1 );
				process2DTriangle( &projectedq, &split0, &split1 );
				break;
							
			case  2:		// 00 00 10  =   'r' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
				process2DTriangle( &projectedp, &projectedq, &split1 );
				process2DTriangle( &projectedq, &split0, &split1 );
				break;

			case  4:		// 00 01 00	=   'q' is beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				process2DTriangle( &projectedr, &projectedp, &split0);
				process2DTriangle( &projectedr, &split0, &split1 );
				break;

			case  8:		// 00 10 00 =   'q' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE );
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE );										 
				process2DTriangle( &projectedr, &projectedp, &split0);
				process2DTriangle( &projectedr, &split0, &split1 );
				break;

			case 16:		// 01 00 00	=   'p' is beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);										 
				process2DTriangle( &projectedq, &projectedr, &split0);
				process2DTriangle( &projectedr, &split1, &split0 );
				break;

			case 32:		// 10 00 00	=	'p' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
				process2DTriangle( &projectedq, &projectedr, &split0);
				process2DTriangle( &projectedr, &split1, &split0 );
				break;

			case  5:		// 00 01 01  =  'q' and 'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->p, &triPtr->r, currentcamera->zbackplane);										 
				process2DTriangle( &projectedp, &split0, &split1);
				break;

			case 17:		// 01 00 01 =  'p' and 'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				process2DTriangle( &projectedq, &split1, &split0);
				break;

			case 20:		// 01 01 00 =  'p' and 'q' beyond back plane
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				process2DTriangle( &projectedr, &split0, &split1);
				break;

			case 10:		// 00 10 10	=  'q' and 'r' infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->p, &triPtr->r, ZFRONTPLANE);										 
				process2DTriangle( &projectedp, &split0, &split1);
				break;

			case 34:		// 10 00 10 =  'p' and 'r' infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
				process2DTriangle( &projectedq, &split1, &split0);
				break;

			case 40:		// 10 10 00 =  'p' and 'q' infront of z==1
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
				process2DTriangle( &projectedr, &split0, &split1);
				break;

			case  6:		// 00 01 10 =  'q' beyond back plane,  'r' in front of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->q, &triPtr->r, ZFRONTPLANE,  currentcamera->zbackplane);
				process2DTriangle( &projectedp, &split0, &split3);
				process2DTriangle( &projectedp, &split3, &split2);
				process2DTriangle( &projectedp, &split2, &split1);
				break;

			case  9:        // 00 10 01	= 'q' in front of z==1,		'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->q, &triPtr->r,  currentcamera->zbackplane, ZFRONTPLANE);
				process2DTriangle( &projectedp, &split0, &split3);
				process2DTriangle( &projectedp, &split3, &split2);
				process2DTriangle( &projectedp, &split2, &split1);
				break;

			case 18:		// 01 00 10 =  'p' beyond backplane,  'r' in front of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->r, &triPtr->p,  ZFRONTPLANE, currentcamera->zbackplane);
				process2DTriangle( &projectedq, &split1, &split2);
				process2DTriangle( &projectedq, &split2, &split3);
				process2DTriangle( &projectedq, &split3, &split0);
				break;

			case 33:		// 10 00 01	=  'p' in front of z==1,  'r' beyond backplane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->r, &triPtr->p, currentcamera->zbackplane,  ZFRONTPLANE);
				process2DTriangle( &projectedq, &split1, &split2);
				process2DTriangle( &projectedq, &split2, &split3);
				process2DTriangle( &projectedq, &split3, &split0);
				break;

			case 24:		// 01 10 00 =  'p' beyond backplane,  'q' in front of z==1
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->p, &triPtr->q, ZFRONTPLANE, currentcamera->zbackplane);
				process2DTriangle( &projectedr, &split0, &split3);
				process2DTriangle( &projectedr, &split3, &split2);
				process2DTriangle( &projectedr, &split2, &split1);
				break;

			case 36:		// 10 01 00 =  'p' in front of z==1,  'q' beyond backplane
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->p, &triPtr->q, currentcamera->zbackplane, ZFRONTPLANE);
				process2DTriangle( &projectedr, &split0, &split3);
				process2DTriangle( &projectedr, &split3, &split2);
				process2DTriangle( &projectedr, &split2, &split1);
				break;

			case 22:		// 01 01 10 
				SZ4_A( &triPtr->r,&triPtr->p,   &triPtr->q,&triPtr->r);
				break;

			case 41:		// 10 10 01	
				SZ4_B( &triPtr->r,&triPtr->p,   &triPtr->q,&triPtr->r);
				break;
			
			case 25:		// 01 10 01	
				SZ4_A( &triPtr->q,&triPtr->r,  &triPtr->p,&triPtr->q );
				break;
 			
			case 38:		// 10 01 10 	
				SZ4_B( &triPtr->q,&triPtr->r,  &triPtr->p,&triPtr->q );
				break;
			
			case 26:		// 01 10 10  
				SZ4_A( &triPtr->r,&triPtr->p,   &triPtr->p,&triPtr->q);
				break;
			
			case 37:		// 10 01 01 
				SZ4_B( &triPtr->r,&triPtr->p,   &triPtr->p,&triPtr->q);
				break;

		}
	} 
	else
	{
		switch( mask ) //Must z-clip cannot (clip)?0:mask)
		{
			case 0:			// 000000 = completely between z-planes: NO SPLIT
				Wprocess2DTriangle(&projectedp, &projectedq, &projectedr);
				break;

			case 21:		// 01 01 01 = completely beyond back plane - IGNORE
			case 42:		// 10 10 10 = completely infront of front plane - IGNORE
				break;
	
			case  1:		// 00 00 01 =   'r' is beyobnd back plane - split into 2 triangles
				splitEdgeZ( &split0,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedp, &projectedq, &split1 );
				Wprocess2DTriangle( &projectedq, &split0, &split1 );
				break;
							
			case  2:		// 00 00 10  =   'r' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
				Wprocess2DTriangle( &projectedp, &projectedq, &split1 );
				Wprocess2DTriangle( &projectedq, &split0, &split1 );
				break;

			case  4:		// 00 01 00	=   'q' is beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedr, &projectedp, &split0);
				Wprocess2DTriangle( &projectedr, &split0, &split1 );
				break;

			case  8:		// 00 10 00 =   'q' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE );
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE );										 
				Wprocess2DTriangle( &projectedr, &projectedp, &split0);
				Wprocess2DTriangle( &projectedr, &split0, &split1 );
				break;

			case 16:		// 01 00 00	=   'p' is beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedq, &projectedr, &split0);
				Wprocess2DTriangle( &projectedr, &split1, &split0 );
				break;

			case 32:		// 10 00 00	=	'p' is infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
				Wprocess2DTriangle( &projectedq, &projectedr, &split0);
				Wprocess2DTriangle( &projectedr, &split1, &split0 );
				break;

			case  5:		// 00 01 01  =  'q' and 'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->p, &triPtr->r, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedp, &split0, &split1);
				break;

			case 17:		// 01 00 01 =  'p' and 'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedq, &split1, &split0);
				break;

			case 20:		// 01 01 00 =  'p' and 'q' beyond back plane
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);										 
				Wprocess2DTriangle( &projectedr, &split0, &split1);
				break;

			case 10:		// 00 10 10	=  'q' and 'r' infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->p, &triPtr->r, ZFRONTPLANE);										 
				Wprocess2DTriangle( &projectedp, &split0, &split1);
				break;

			case 34:		// 10 00 10 =  'p' and 'r' infront of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
				Wprocess2DTriangle( &projectedq, &split1, &split0);
				break;

			case 40:		// 10 10 00 =  'p' and 'q' infront of z==1
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
				Wprocess2DTriangle( &projectedr, &split0, &split1);
				break;

			case  6:		// 00 01 10 =  'q' beyond back plane,  'r' in front of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->q, &triPtr->r, ZFRONTPLANE,  currentcamera->zbackplane);
				Wprocess2DTriangle( &projectedp, &split0, &split3);
				Wprocess2DTriangle( &projectedp, &split3, &split2);
				Wprocess2DTriangle( &projectedp, &split2, &split1);
				break;

			case  9:        // 00 10 01	= 'q' in front of z==1,		'r' beyond back plane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->q, &triPtr->r,  currentcamera->zbackplane, ZFRONTPLANE);
				Wprocess2DTriangle( &projectedp, &split0, &split3);
				Wprocess2DTriangle( &projectedp, &split3, &split2);
				Wprocess2DTriangle( &projectedp, &split2, &split1);
				break;

			case 18:		// 01 00 10 =  'p' beyond backplane,  'r' in front of z==1
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, currentcamera->zbackplane);
 				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->r, &triPtr->p,  ZFRONTPLANE, currentcamera->zbackplane);
				Wprocess2DTriangle( &projectedq, &split1, &split2);
				Wprocess2DTriangle( &projectedq, &split2, &split3);
				Wprocess2DTriangle( &projectedq, &split3, &split0);
				break;

			case 33:		// 10 00 01	=  'p' in front of z==1,  'r' beyond backplane
				splitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->r, &triPtr->p, currentcamera->zbackplane,  ZFRONTPLANE);
				Wprocess2DTriangle( &projectedq, &split1, &split2);
				Wprocess2DTriangle( &projectedq, &split2, &split3);
				Wprocess2DTriangle( &projectedq, &split3, &split0);
				break;

			case 24:		// 01 10 00 =  'p' beyond backplane,  'q' in front of z==1
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, currentcamera->zbackplane);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->p, &triPtr->q, ZFRONTPLANE, currentcamera->zbackplane);
				Wprocess2DTriangle( &projectedr, &split0, &split3);
				Wprocess2DTriangle( &projectedr, &split3, &split2);
				Wprocess2DTriangle( &projectedr, &split2, &split1);
				break;

			case 36:		// 10 01 00 =  'p' in front of z==1,  'q' beyond backplane
				splitEdgeZ( &split0,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
				splitEdgeZ( &split1,  &triPtr->q, &triPtr->r, currentcamera->zbackplane);
				doubleSplitEdgeZ( &split2, &split3, &triPtr->p, &triPtr->q, currentcamera->zbackplane, ZFRONTPLANE);
				Wprocess2DTriangle( &projectedr, &split0, &split3);
				Wprocess2DTriangle( &projectedr, &split3, &split2);
				Wprocess2DTriangle( &projectedr, &split2, &split1);
				break;

			case 22:		// 01 01 10 
				wSZ4_A( &triPtr->r,&triPtr->p,   &triPtr->q,&triPtr->r);
				break;

			case 41:		// 10 10 01	
				wSZ4_B( &triPtr->r,&triPtr->p,   &triPtr->q,&triPtr->r);
				break;
			
			case 25:		// 01 10 01	
				wSZ4_A( &triPtr->q,&triPtr->r,  &triPtr->p,&triPtr->q );
				break;
 			
			case 38:		// 10 01 10 	
				wSZ4_B( &triPtr->q,&triPtr->r,  &triPtr->p,&triPtr->q );
				break;
			
			case 26:		// 01 10 10  
				wSZ4_A( &triPtr->r,&triPtr->p,   &triPtr->p,&triPtr->q);
				break;
			
			case 37:		// 10 01 01 
				wSZ4_B( &triPtr->r,&triPtr->p,   &triPtr->p,&triPtr->q);
				break;

				
		}
	}
}

void engine::Skyzsplitting()
{
	static   pointdata2d split0, split1, split2, split3;

	switch( mask ) // (clip)?0:mask)
	{
		case 0:			// 000000 = completely between z-planes: NO SPLIT
			process2DTriangle(&projectedp, &projectedq, &projectedr);
			break;
			
		case 42:		// 10 10 10 = completely infront of front plane - IGNORE
			break;
								
		case  2:		// 00 00 10  =   'r' is infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->q, &triPtr->r, ZFRONTPLANE);
			skysplitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
			process2DTriangle( &projectedp, &projectedq, &split1 );
			process2DTriangle( &projectedq, &split0, &split1 );
			break;
								
		case  8:		// 00 10 00 =   'q' is infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE );
			skysplitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE );										 
			process2DTriangle( &projectedr, &projectedp, &split0);
			process2DTriangle( &projectedr, &split0, &split1 );
			break;
										
		case 32:		// 10 00 00	=	'p' is infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
			skysplitEdgeZ( &split1,  &triPtr->r, &triPtr->p, ZFRONTPLANE);										 
			process2DTriangle( &projectedq, &projectedr, &split0);
			process2DTriangle( &projectedr, &split1, &split0 );
			break;
								
		case 10:		// 00 10 10	=  'q' and 'r' infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
			skysplitEdgeZ( &split1,  &triPtr->p, &triPtr->r, ZFRONTPLANE);										 
			process2DTriangle( &projectedp, &split0, &split1);
			break;
								
		case 34:		// 10 00 10 =  'p' and 'r' infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->p, &triPtr->q, ZFRONTPLANE);
			skysplitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
			process2DTriangle( &projectedq, &split1, &split0);
			break;
								
		case 40:		// 10 10 00 =  'p' and 'q' infront of z==1
			skysplitEdgeZ( &split0,  &triPtr->r, &triPtr->p, ZFRONTPLANE);
			skysplitEdgeZ( &split1,  &triPtr->q, &triPtr->r, ZFRONTPLANE);										 
			process2DTriangle( &projectedr, &split0, &split1);
			break;
	}
}

void engine::getEnvmapUVs(nongeodata * nv1, vertex * v, vector3 * p)
{
	float dp =  2.0f * v->vertexNormal.DotProduct(&currentcamera->direction);

	float rx = v->vertexNormal.x - dp*currentcamera->direction.x;
	float ry = v->vertexNormal.y - dp*currentcamera->direction.y;
	float rz = 1 - (v->vertexNormal.z - dp*currentcamera->direction.z);

	float rm = 5.0f / ( fsqrt( rx * rx + ry * ry + rz * rz ) );
	nv1->u = rx * rm + 0.5f + p->x * 0.09f;
	nv1->v = ry * rm + 0.5f + p->z * 0.09f;
}

//================================================================================================================================
// renderSky
// ---------
//
//		render the current sky dome.
//================================================================================================================================
void engine::renderSky()
{

	static matrix3x4 WCtransform;
	ulong t;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;

	mesh * meshData = currentSky->skyModel->meshData;	// only one LOD

	WCtransform.multiply3x3submats( &currentcamera->RotMatrix, &currentSky->RotMatrix );
	triPtr = meshData->tlistPtr;

	globalpctr++;

	envmapping = false;				// NO ENVIRONMENT MAPPING ON SKY!!!
	detmapping = false;

	for( t = meshData->tcount; t--; )
	{

		currentmtl = triPtr->mtl;

		//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					 
		pvPtr = triPtr->p.v;
		qvPtr = triPtr->q.v;
		rvPtr = triPtr->r.v;

		//
		// Note that if we form SCmaskInZ without the backplane then 'zsplitting' will still work ok. with no
		// z backplane being considered!!
		//

#define SUBTEND 400.0f

		if( pvPtr->tcnt != globalpctr )		// not yet transformed 
		{
			pvPtr->tcnt = globalpctr;
			
			pvPtr->geoCoord->y -= SUBTEND;
			WCtransform.multiplyV3By3x3mat( &pvPtr->transformed, pvPtr->geoCoord);
			pvPtr->geoCoord->y += SUBTEND;
			pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
		}
		if( qvPtr->tcnt != globalpctr )	
		{
			qvPtr->tcnt = globalpctr;
			qvPtr->geoCoord->y -= SUBTEND;
			WCtransform.multiplyV3By3x3mat( &qvPtr->transformed, qvPtr->geoCoord );
			qvPtr->geoCoord->y += SUBTEND;
			qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
		}
		if( rvPtr->tcnt != globalpctr )		
		{
			rvPtr->tcnt = globalpctr;
			rvPtr->geoCoord->y -= SUBTEND;
			WCtransform.multiplyV3By3x3mat( &rvPtr->transformed, rvPtr->geoCoord);
			rvPtr->geoCoord->y += SUBTEND;
			rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
		}

		//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
		//  is no need).
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					
		mask = (pvPtr->SCmaskInZ<<4) | (qvPtr->SCmaskInZ<<2) | rvPtr->SCmaskInZ;
						
		if( (mask & 48) == 0 )				// if vertex is between z-planes
		{
			projectedp.nv = &triPtr->p.nv;
			if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
			{
				pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
				zLimitedproject( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l, pvPtr->specularl );
			}
			projectedp.v = pvPtr->projectedPtr;		// use already projected value
		}
		
		if( (mask & 12) == 0 )
		{
			projectedq.nv = &triPtr->q.nv;
			if( qvPtr->pcnt != globalpctr )		// not yet projected 
			{
				qvPtr->pcnt = globalpctr;
				zLimitedproject( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l,  qvPtr->specularl  );
			}
			projectedq.v = qvPtr->projectedPtr;
		}
		
		if( (mask & 3) == 0 )
		{
			projectedr.nv = &triPtr->r.nv;
			if( rvPtr->pcnt != globalpctr )		// not yet projected 
			{	
				rvPtr->pcnt = globalpctr;
				zLimitedproject( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l,  rvPtr->specularl );
			}
			projectedr.v = rvPtr->projectedPtr;
		}
		
		// temp. stop lighting if on
		
		pvPtr->l = qvPtr->l = rvPtr->l = 1.0f;
		
		float zbp = currentcamera->zbackplane;
		currentcamera->zbackplane = 9999999.0f;
		Skyzsplitting();
		currentcamera->zbackplane = zbp;
		triPtr++;				// next triangle in sky	
	}
}

//================================================================================================================================
// - process a hunk which has been found to be at least partially visible
//================================================================================================================================
void engine::renderthisHunk( triangle **thistlist, ulong total)
{
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	long kk;
	bool potentiallyVisible;

	for (kk = total; kk--; thistlist++)
	{
	    triPtr = *thistlist;
		if( triPtr->pcnt != globalpctr )		// if not already examined (another hunk)
		{
			triPtr->pcnt = globalpctr;			// flag the triangle as 'used' so we won't reuse it from another hunk
			currentmtl = triPtr->mtl;

			if( currentmtl->bm.flags & MAT_NOCLIP || currentmtl->bm.flags & (MAT_MAXZM1 | MAT_MAXZ))
			{
				potentiallyVisible = true;
				if ( currentmtl->bm.type == MAT_DRIVINGAIDS ) 
				{
					// Material is a driver aid so only allow it if driveraids is on
					potentiallyVisible = driveraids;
				}

				if ( currentmtl->bm.flags & MAT_INVISIBLE ) 
				{
					// Material is a driver aid so only allow it if driveraids is on
					potentiallyVisible = false;
				}


				if( potentiallyVisible )
				{
					if( !( potentiallyVisible = ((currentmtl->bm.flags & MAT_DOUBLESIDED )? true : false)) )
					{
						potentiallyVisible = ( (triPtr->faceNormal.x * currentcamera->RotMatrix.E20 + 
											    triPtr->faceNormal.y * currentcamera->RotMatrix.E21 + 
									             triPtr->faceNormal.z * currentcamera->RotMatrix.E22 )<= CULLINGEPSILON ) ;
					}	
					
					if( potentiallyVisible )
					{
						envmapping = false;
						detmapping = deton && (( currentmtl->bm.flags & MAT_DETAILMAP ) != 0);


						//
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//

						pvPtr = triPtr->p.v;
						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &pvPtr->transformed, pvPtr->geoCoord );
							pvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &pvPtr->nv1, pvPtr);
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;
						}

						qvPtr = triPtr->q.v;
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &qvPtr->transformed, qvPtr->geoCoord );
							qvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &qvPtr->nv1, qvPtr);

							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;
						}

						rvPtr = triPtr->r.v;
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &rvPtr->transformed, rvPtr->geoCoord );
							rvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &rvPtr->nv1, rvPtr);

							rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 :  0;
						}

						//
						//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
						//  is no need).
						//
					
						mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);

						if( (mask & 48) == 0 )				// if vertex is between z-planes
						{
							projectedp.nv = &triPtr->p.nv;
							projectedp.nv1 = pvPtr->nv1;
							
							if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
							{
								pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
								zLimitedproject( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l, pvPtr->specularl );
							}
							projectedp.v = pvPtr->projectedPtr;		// use already projected value
						}
			
						if( (mask & 12) == 0 )
						{
							projectedq.nv = &triPtr->q.nv;
							projectedq.nv1 = qvPtr->nv1;
							
							
							if( qvPtr->pcnt != globalpctr )		// not yet projected 
							{
								qvPtr->pcnt = globalpctr;
								zLimitedproject( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l, qvPtr->specularl  );
							}
							projectedq.v = qvPtr->projectedPtr;
						}

						if( (mask & 3) == 0 )
						{
							projectedr.nv = &triPtr->r.nv;
							projectedr.nv1 = rvPtr->nv1;
							
							if( rvPtr->pcnt != globalpctr )		// not yet projected 
							{	
								rvPtr->pcnt = globalpctr;
								zLimitedproject( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l, rvPtr->specularl );
							};
							projectedr.v = rvPtr->projectedPtr;
						}
				
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// According to which front/back planes are crossed (if any) by this triangle, split if necessary
						// (creating new 2D vertices) and process resulting 2D triangles
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - - 

						polygoncounter++;
			
						float zbp = currentcamera->zbackplane;
						currentcamera->zbackplane =9999999.0f;  //MAXIMUMREARZ;//

						Skyzsplitting();
						currentcamera->zbackplane = zbp;
					}
				}
			}
			else			// not 'NO_CLIP' flag
			{
				potentiallyVisible = true;
				if ( currentmtl->bm.flags & MAT_DRIVINGAIDS ) 
				{
					// Material is a driver aid so only allow it if driveraids is on
					potentiallyVisible = driveraids;
				}


				if ( currentmtl->bm.flags & MAT_INVISIBLE ) 
				{
					// Material is a driver aid so only allow it if driveraids is on
					potentiallyVisible = false;
				}

				if( potentiallyVisible )
				{
					if( !( potentiallyVisible = ((currentmtl->bm.flags & MAT_DOUBLESIDED )? true : false)) )
					{
						potentiallyVisible = ( (triPtr->faceNormal.x * currentcamera->RotMatrix.E20 + 
							triPtr->faceNormal.y * currentcamera->RotMatrix.E21 + 
							triPtr->faceNormal.z * currentcamera->RotMatrix.E22 )<= CULLINGEPSILON ) ;
					}	
					
					if( potentiallyVisible )
					{
						
						envmapping = false;
						detmapping = deton && (( currentmtl->bm.flags & MAT_DETAILMAP ) != 0);
						
						//
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//
						
						pvPtr = triPtr->p.v;
						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &pvPtr->transformed, pvPtr->geoCoord );
							pvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &pvPtr->nv1, pvPtr);
							
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						}
						
						qvPtr = triPtr->q.v;
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &qvPtr->transformed, qvPtr->geoCoord );
							qvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &qvPtr->nv1, qvPtr);
							
							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						}
						
						rvPtr = triPtr->r.v;
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							currentcamera->transformWorldVector( &rvPtr->transformed, rvPtr->geoCoord );
							rvPtr->transformed.z -= zoomz;
							if( deton )
								getDetmapUVs( &rvPtr->nv1, rvPtr);
							
							rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						}
						
						//
						//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
						//  is no need).
						//
						
						mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);
						
						if( (mask & 48) == 0 )				// if vertex is between z-planes
						{
							projectedp.nv = &triPtr->p.nv;
							projectedp.nv1 = pvPtr->nv1;
							
							
							if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
							{
								pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
								project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l, pvPtr->specularl );
							}
							projectedp.v = pvPtr->projectedPtr;		// use already projected value
						}
						
						if( (mask & 12) == 0 )
						{
							projectedq.nv = &triPtr->q.nv;
							projectedq.nv1 = qvPtr->nv1;
							
							if( qvPtr->pcnt != globalpctr )		// not yet projected 
							{
								qvPtr->pcnt = globalpctr;
								project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l, qvPtr->specularl  );
							}
							projectedq.v = qvPtr->projectedPtr;
						}
						
						if( (mask & 3) == 0 )
						{
							projectedr.nv = &triPtr->r.nv;
							projectedr.nv1 = rvPtr->nv1;
							
							if( rvPtr->pcnt != globalpctr )		// not yet projected 
							{	
								rvPtr->pcnt = globalpctr;
								project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l, rvPtr->specularl );
							}
							projectedr.v = rvPtr->projectedPtr;
						}
						
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// According to which front/back planes are crossed (if any) by this triangle, split if necessary
						// (creating new 2D vertices) and process resulting 2D triangles
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - - 
						
						polygoncounter++;
						
						
						zsplitting();
					}	
				}
			}
//			triPtr++;

		}
		triPtr++;
	} 
}


int engine::GetLod(model *m,float z)
{
	int qq;

	z *= lod_threshold_scale;

	for(int i=0;i<m->meshct;i++)
		if(z<m->meshthresholds[i])
		{
			if(i<m->minimum_lod)i=m->minimum_lod;
			return i;
		}

	i--;
//	if(i<m->minimum_lod)i=m->minimum_lod;
	return i-1;
}


//================================================================================================================================
// Macro adapted from ::project for rejection of 3d coords against 2D volume where we DONT need the actual projected coord.
//
//						Note that we need no divisions for this code
//
// THIS MACRO IS EXPLICIT TO THE ONE ROUTINE, ITS PARAMETERS WILL ALWAYS BE EXPRESSIONS SO WE HAVE 'l3 = z3'
//================================================================================================================================

//	if (( lz >= ZFRONTPLANE ) && ( lz <= currentcamera->zbackplane) )  

#define OFFSCREEN( x3, y3, z3 )  \
{ \
	float lz; \
	int tmask = 0; \
	lz = z3;\
 \
	if(mask & 3) \
	{ \
		float px; \
		px = displayWd2divprojcoeffW*lz; \
		tmask = (( x3 < -px ) ? 1 : ( (x3 > px) ? 2 : 0 )); \
	} \
	if(mask & 12) \
	{ \
		float py; \
		py = displayHd2divprojcoeffH*lz; \
		tmask |= (( y3 < -py ) ? 4 : ( (y3 > py) ? 8 : 0 )); \
	} \
	mask &= tmask; \
}

//	if(mask & 48) \
//		tmask |= (( lz < ZFRONTPLANE ) ? 16 : ( (lz > currentcamera->zbackplane) ? 32 : 0 )); \

//----------------------------------------------------------------------------------------------------------------------------
// processObjectLit
// ----------------
//			RECURSIVE processing of the object tree  (LIGHTING ON)
//----------------------------------------------------------------------------------------------------------------------------

//ulong bumf;


void engine::processObjectLit( object * objPtr )
{
	static vector3 transformedFnormal;
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	ulong t;
	bool    potentiallyVisible;
	bool    shadowTri;
	
	myObject = objPtr;
	while( myObject)		// for object and each of its sister objects
	{
		if( myObject->child != 0)
			processObjectLit(myObject->child);
		
		if(myObject->visible)
		{
			if( myObject->definedInCameraSpace )
			{
				myObject->transform.translation.z += zoomz;
				catMat = &myObject->transform;
				WCtransform = *catMat;
			}
			else
			{
				catMat = &myObject->concatenatedTransform;
				catMat->translation -= currentcamera->position;
				WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
				catMat->translation += currentcamera->position;
			}
			
			if( myObject->modelData != 0)		// ie not a NULL model 
			{
				// check bounding sphere is on screen
				if( checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{
					// calculate which LOD to use
					
					vector3 opos,rpos;
					
					opos = myObject->concatenatedTransform.translation - currentcamera->position;
					currentcamera->RotMatrix.multiplyV3By3x3mat(&rpos,&opos);
					
					int lod = GetLod(myObject->modelData,rpos.z);
					if(lod>myObject->modelData->highest_visible_mesh)
						goto donotrender;
					mesh *lodmesh = myObject->modelData->meshData+lod;
					triPtr = lodmesh->tlistPtr;
					
					globalpctr++;
					
					for( t = lodmesh->tcount; t--; )
					{
						currentmtl = triPtr->mtl;
						
						if( ! ( potentiallyVisible = ( ( currentmtl->bm.flags & MAT_DOUBLESIDED ) != 0 ) ) )
						{
							
							potentiallyVisible = ( (triPtr->faceNormal.x * WCtransform.E20 + 
								triPtr->faceNormal.y * WCtransform.E21 + 
								triPtr->faceNormal.z * WCtransform.E22 ) <= CULLINGEPSILON ) ;
						}
						
						if( potentiallyVisible )
						{
							envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
							detmapping = deton && (( currentmtl->bm.flags & MAT_DETAILMAP ) != 0);
							
							shadowTri = ( currentmtl->bm.flags & MAT_SHADOW )? true : false;
							
							//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
							// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
							//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
							
							pvPtr = triPtr->p.v;
							qvPtr = triPtr->q.v;
							rvPtr = triPtr->r.v;
							
							
							if(shadowTri)
							{
								catMat->multiplyV3By3x3mat(	&transformedFnormal, &triPtr->faceNormal );
								float dp = boris2->currentworld->sunLightDirection.DotProduct( &transformedFnormal );
								potentiallyVisible = true;
							}
							if( potentiallyVisible)
							{
								if( pvPtr->tcnt != globalpctr )		// not yet transformed 
								{
									pvPtr->tcnt = globalpctr;

									if( shadowTri )
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, pvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										//tempv.y += 100;
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, &(*pvPtr->geoCoord - myObject->turningPoint) );
										catMat->translation += currentcamera->position;
										tempv += myObject->turningPoint;
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );
									}
								
									pvPtr->transformed.z -= zoomz;
									catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal, &pvPtr->vertexNormal);
									
									pvPtr->specularl = currentcamera->direction.DotProduct( &currentworld->sunLightDirection );
									if( pvPtr->specularl < 0 ) 
										pvPtr->specularl = 0;
									
									pvPtr->l = pvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
									if( pvPtr->l < 0 ) 
										pvPtr->l = 0;
									
									pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
									if( envon )
										getEnvmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
									if( deton )
										getDetmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
								}
								if( qvPtr->tcnt != globalpctr )	
								{
									qvPtr->tcnt = globalpctr;

									if( shadowTri )
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, qvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv,  &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, &(*qvPtr->geoCoord - myObject->turningPoint) );
										catMat->translation += currentcamera->position;
										tempv += myObject->turningPoint;
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );
									}
									
									qvPtr->transformed.z -= zoomz;
									catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
									
									qvPtr->specularl = currentcamera->direction.DotProduct( &currentworld->sunLightDirection );
									if( qvPtr->specularl < 0 ) 
										qvPtr->specularl = 0;
									
									qvPtr->l = qvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
									if( qvPtr->l < 0 ) 
										qvPtr->l = 0;
									
									qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
									if( envon )
										getEnvmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
									if( deton )
										getDetmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
								}
								if( rvPtr->tcnt != globalpctr )		
								{
									rvPtr->tcnt = globalpctr;
									
									if( shadowTri )
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, rvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv,  &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, &(*rvPtr->geoCoord - myObject->turningPoint) );
										catMat->translation += currentcamera->position;
										tempv += myObject->turningPoint;
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );
									}
									rvPtr->transformed.z -= zoomz;
									catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
									
									rvPtr->specularl = currentcamera->direction.DotProduct( &currentworld->sunLightDirection );
									if( rvPtr->specularl < 0 ) 
										rvPtr->specularl = 0;
									
									rvPtr->l = rvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
									if( rvPtr->l < 0 ) 
										rvPtr->l = 0;
									
									rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
									if( envon )
										getEnvmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
									if( deton )
										getDetmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
								}
								
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
								//  is no need).
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								
								mask = (pvPtr->SCmaskInZ<<4) | (qvPtr->SCmaskInZ<<2) | rvPtr->SCmaskInZ;
								
								if( (mask & 48) == 0 )				// if vertex is between z-planes
								{
									projectedp.nv = &triPtr->p.nv;
									projectedp.nv1 = pvPtr->nv1;
									
									if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
									{
										pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
										project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l, pvPtr->specularl );
									}
									projectedp.v = pvPtr->projectedPtr;		// use already projected value
								}
								
								if( (mask & 12) == 0 )
								{
									projectedq.nv = &triPtr->q.nv;
									projectedq.nv1 = qvPtr->nv1;
									
									if( qvPtr->pcnt != globalpctr )		// not yet projected 
									{
										qvPtr->pcnt = globalpctr;
										project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l, qvPtr->specularl  );
									}
									projectedq.v = qvPtr->projectedPtr;
								}
								
								if( (mask & 3) == 0 )
								{
									projectedr.nv = &triPtr->r.nv;
									projectedr.nv1 = rvPtr->nv1;
									
									if( rvPtr->pcnt != globalpctr )		// not yet projected 
									{	
										rvPtr->pcnt = globalpctr;
										project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l, rvPtr->specularl );
									}
									projectedr.v = rvPtr->projectedPtr;
								}
								zsplitting();
							}
						}
					}				// if (potentially visible
					triPtr++;
							
				}	// if (ok)
					
			}  // if(objPtr->modeldata != 0)
			
donotrender:
			
			if( myObject->definedInCameraSpace )
				myObject->transform.translation.z -= zoomz;
		}			
		myObject = myObject->sister;
	}  // while( myObject)
}

//----------------------------------------------------------------------------------------------------------------------------
// processObjectUnlit
// ------------------
//			RECURSIVE processing of the object tree  (LIGHTING OFF)
//----------------------------------------------------------------------------------------------------------------------------

void engine::processObjectUnlit( object * objPtr )
{
	static vector3 transformedFnormal;
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	ulong t;

	static vector3 tempvector;
	bool potentiallyVisible;
	bool shadowTri;
		
	// - examine the model (if not 0) and transform every point by the local transform matrix
	//   and add on the position in the world!
			
			
	myObject = objPtr;
	while( myObject)		// for object and each of its sister objects
	{
		
		if( myObject->child != 0)
			processObjectUnlit(myObject->child);
		
		if(myObject->visible)
		{
			if( myObject->definedInCameraSpace )
			{		
				myObject->transform.translation.z += zoomz;
				catMat = &myObject->transform;
				WCtransform = *catMat;
			} 
			else
			{
				catMat = &myObject->concatenatedTransform;
				catMat->translation -= currentcamera->position;
				WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
				catMat->translation += currentcamera->position;
			}
			
			if( myObject->modelData != 0)		// ie not a NULL model 
			{
				// check bounding box is onscreen ....
				
				if(checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{
					// calculate which LOD to use
					
					vector3 opos,rpos;
					
					opos = myObject->concatenatedTransform.translation - currentcamera->position;
					currentcamera->RotMatrix.multiplyV3By3x3mat(&rpos,&opos);
					
					int lod = GetLod(myObject->modelData,rpos.z);
					
					if(lod>myObject->modelData->highest_visible_mesh)
						goto donotrender;
					
					mesh *lodmesh = myObject->modelData->meshData+lod;
					
					triPtr = lodmesh->tlistPtr;
					
					globalpctr++;
					
					for( t = lodmesh->tcount; t--; )
					{
						currentmtl = triPtr->mtl;
						
						if (!( potentiallyVisible = ((currentmtl->bm.flags & MAT_DOUBLESIDED) != 0) ) )
						{
							
							potentiallyVisible = ( (triPtr->faceNormal.x * WCtransform.E20 + 
								triPtr->faceNormal.y * WCtransform.E21 + 
								triPtr->faceNormal.z * WCtransform.E22 ) <= CULLINGEPSILON ) ;
						}
						
						if( potentiallyVisible )
						{
							envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
							detmapping = deton && (( currentmtl->bm.flags & MAT_DETAILMAP ) != 0);
							
							shadowTri = ( currentmtl->bm.flags & MAT_SHADOW )? true : false;
							
							pvPtr = triPtr->p.v;
							qvPtr = triPtr->q.v;
							rvPtr = triPtr->r.v;
							
							if(shadowTri)
							{
								catMat->multiplyV3By3x3mat(	&transformedFnormal, &triPtr->faceNormal );
								float dp = boris2->currentworld->sunLightDirection.DotProduct( &transformedFnormal );
								potentiallyVisible = true;
							}
							if(potentiallyVisible || shadowTri)
							{
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								
								if( pvPtr->tcnt != globalpctr )		// not yet transformed 
								{
									pvPtr->tcnt = globalpctr;
									
									if( shadowTri )
									{
										static vector3 tempv;
										
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, pvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );				
									}
									else
									{
										
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*pvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );
										
									}
									
									pvPtr->transformed.z -= zoomz;
									if( envon)
									{
										catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
										getEnvmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton)
									{
										catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
										getDetmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
									}
									pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								if( qvPtr->tcnt != globalpctr )	
								{
									qvPtr->tcnt = globalpctr;
									
									if( shadowTri )
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, qvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*qvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );
									}
  
									qvPtr->transformed.z -= zoomz;
									if( envon)
									{
										catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
										getEnvmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton)
									{
										catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
										getDetmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
									}
									qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								if( rvPtr->tcnt != globalpctr )		
								{
									rvPtr->tcnt = globalpctr;
									if ( shadowTri )						
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, rvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal);
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*rvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );
									}
									
									rvPtr->transformed.z -= zoomz;
									if( envon )
									{
										catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
										getEnvmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton )
									{
										catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
										getDetmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
									}
									rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								
								
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
								//  is no need).
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								
								mask = (pvPtr->SCmaskInZ<<4) | (qvPtr->SCmaskInZ<<2) | rvPtr->SCmaskInZ;
								
								if( (mask & 48) == 0 )				// if vertex is between z-planes
								{
									projectedp.nv = &triPtr->p.nv;
									if( envon || deton )
										projectedp.nv1 = pvPtr->nv1;
									
									
									if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
									{
										pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
										project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l, pvPtr->specularl );
									}
									projectedp.v = pvPtr->projectedPtr;		// use already projected value
								}
								
								if( (mask & 12) == 0 )
								{
									projectedq.nv = &triPtr->q.nv;
									if( envon || deton)
										projectedq.nv1 = qvPtr->nv1;
									
									if( qvPtr->pcnt != globalpctr )		// not yet projected 
									{
										qvPtr->pcnt = globalpctr;
										project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l, qvPtr->specularl  );
									}
									projectedq.v = qvPtr->projectedPtr;
								}
								
								if( (mask & 3) == 0 )
								{
									projectedr.nv = &triPtr->r.nv;
									if( envon || deton )
										projectedr.nv1 = rvPtr->nv1;
									
									if( rvPtr->pcnt != globalpctr )		// not yet projected 
									{	
										rvPtr->pcnt = globalpctr;
										project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l, rvPtr->specularl );
									}
									projectedr.v = rvPtr->projectedPtr;
								}
								
								zsplitting();
								}
							}
							
							
							triPtr++;
							
					}
					
				}	// if (ok)
				
			} // if (checkRboundboxVisibility
				
donotrender:
			
			if( myObject->definedInCameraSpace )
				myObject->transform.translation.z -= zoomz;
		}		  
		myObject = myObject->sister;
	}  // while( myObject)
}

void engine::processObjectShadow( object * objPtr )
{
	static vector3 transformedFnormal;
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	ulong t;

	static vector3 tempvector;
	bool potentiallyVisible;
	bool shadowTri;
		
	// - examine the model (if not 0) and transform every point by the local transform matrix
	//   and add on the position in the world!
			
			
	myObject = objPtr;
	while( myObject)		// for object and each of its sister objects
	{
		
		if( myObject->child != 0)
			processObjectShadow(myObject->child);
		
		if(myObject->visible)
		{
			if( myObject->definedInCameraSpace )
			{		
				myObject->transform.translation.z += zoomz;
				catMat = &myObject->transform;
				WCtransform = *catMat;
			} 
			else
			{
				catMat = &myObject->concatenatedTransform;
				catMat->translation -= currentcamera->position;
				WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
				catMat->translation += currentcamera->position;
			}
			
			if( myObject->modelData != 0)		// ie not a NULL model 
			{
				// check bounding box is onscreen ....
				
				if(checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{
					// calculate which LOD to use
					
					vector3 opos,rpos;
					
					opos = myObject->concatenatedTransform.translation - currentcamera->position;
					currentcamera->RotMatrix.multiplyV3By3x3mat(&rpos,&opos);
					
					int lod = GetLod(myObject->modelData,rpos.z);
					
					if(lod>myObject->modelData->highest_visible_mesh)
						goto donotrender;
					
					mesh *lodmesh = myObject->modelData->meshData+lod;
					
					triPtr = lodmesh->tlistPtr;
					
					globalpctr++;
					
					for( t = lodmesh->tcount; t--; )
					{
						currentmtl = triPtr->mtl;
						
						if (!( potentiallyVisible = ((currentmtl->bm.flags & MAT_DOUBLESIDED) != 0) ) )
						{
							
							potentiallyVisible = ( (triPtr->faceNormal.x * WCtransform.E20 + 
								triPtr->faceNormal.y * WCtransform.E21 + 
								triPtr->faceNormal.z * WCtransform.E22 ) <= CULLINGEPSILON ) ;
						}
						
						if( potentiallyVisible )
						{
							envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
							detmapping = deton && (( currentmtl->bm.flags & MAT_DETAILMAP ) != 0);
							
							shadowTri = ( currentmtl->bm.flags & MAT_SHADOW )? true : false;
							
							pvPtr = triPtr->p.v;
							qvPtr = triPtr->q.v;
							rvPtr = triPtr->r.v;
							
							if(shadowTri)
							{
								catMat->multiplyV3By3x3mat(	&transformedFnormal, &triPtr->faceNormal );
								float dp = boris2->currentworld->sunLightDirection.DotProduct( &transformedFnormal );
								potentiallyVisible = true;
							}
							if(potentiallyVisible || shadowTri)
							{
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								
								if( pvPtr->tcnt != globalpctr )		// not yet transformed 
								{
									pvPtr->tcnt = globalpctr;
									
									if( shadowTri )
									{
										static vector3 tempv;
										
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, pvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );				
									}
									else
									{
										
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*pvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );
										
									}
									
									pvPtr->transformed.z -= zoomz;
									if( envon)
									{
										catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
										getEnvmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton)
									{
										catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
										getDetmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
									}
									pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								if( qvPtr->tcnt != globalpctr )	
								{
									qvPtr->tcnt = globalpctr;
									
									if( shadowTri )
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, qvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*qvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );
									}
  
									qvPtr->transformed.z -= zoomz;
									if( envon)
									{
										catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
										getEnvmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton)
									{
										catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
										getDetmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
									}
									qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								if( rvPtr->tcnt != globalpctr )		
								{
									rvPtr->tcnt = globalpctr;
									if ( shadowTri )						
									{
										static vector3 tempv;
										catMat->translation -= currentcamera->position;
										catMat->multiplyV3By3x4mat( &tempv, rvPtr->geoCoord );
										catMat->translation += currentcamera->position;
										
										tempv += currentcamera->position;
										boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal);
										tempv -= currentcamera->position;
										
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );				
									} 
									else
									{
										static vector3 tempv;
										catMat->multiplyV3By3x3mat( &tempv, &(*rvPtr->geoCoord - myObject->turningPoint) );
										tempv += (myObject->turningPoint + catMat->translation - currentcamera->position);
										currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );
									}
									
									rvPtr->transformed.z -= zoomz;
									if( envon )
									{
										catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
										getEnvmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
									}
									if( deton )
									{
										catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
										getDetmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
									}
									rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
								}
								
								
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
								//  is no need).
								//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
								
								mask = (pvPtr->SCmaskInZ<<4) | (qvPtr->SCmaskInZ<<2) | rvPtr->SCmaskInZ;
								
								if( (mask & 48) == 0 )				// if vertex is between z-planes
								{
									projectedp.nv = &triPtr->p.nv;
									if( envon || deton )
										projectedp.nv1 = pvPtr->nv1;
									
									
									if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
									{
										pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
										project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, 100000.0f, pvPtr->specularl );
									}
									projectedp.v = pvPtr->projectedPtr;		// use already projected value
								}
								
								if( (mask & 12) == 0 )
								{
									projectedq.nv = &triPtr->q.nv;
									if( envon || deton)
										projectedq.nv1 = qvPtr->nv1;
									
									if( qvPtr->pcnt != globalpctr )		// not yet projected 
									{
										qvPtr->pcnt = globalpctr;
										project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, 100000.0f, qvPtr->specularl  );
									}
									projectedq.v = qvPtr->projectedPtr;
								}
								
								if( (mask & 3) == 0 )
								{
									projectedr.nv = &triPtr->r.nv;
									if( envon || deton )
										projectedr.nv1 = rvPtr->nv1;
									
									if( rvPtr->pcnt != globalpctr )		// not yet projected 
									{	
										rvPtr->pcnt = globalpctr;
										project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 100000.0f, rvPtr->specularl );
									}
									projectedr.v = rvPtr->projectedPtr;
								}
								
								zsplitting();
								}
							}
							
							
							triPtr++;
							
					}
					
				}	// if (ok)
				
			} // if (checkRboundboxVisibility
				
donotrender:
			
			if( myObject->definedInCameraSpace )
				myObject->transform.translation.z -= zoomz;
		}		  
		myObject = myObject->sister;
	}  // while( myObject)
}

void engine::renderObjects()
{

	object * objPtr;
	if( lightSwitch )
	{
		objPtr = objectList;
		while (objPtr != 0 )		// for each object in the list
		{
			processObjectLit( objPtr);		// begin recursion at this object node
			objPtr = objPtr->next;
		}
	}
	else
	{
		objPtr = objectList;
		while (objPtr != 0 )		// for each object in the list
		{
			if(objPtr->collisionFlags & COLLIDE_SHADOW)
				processObjectShadow( objPtr);		// begin recursion at this object node
			else
				processObjectUnlit( objPtr);		// begin recursion at this object node
			objPtr = objPtr->next;
		}
	}

#ifdef EDITOR
	ulong i;
//DBGR.Dprintf("......");
	if(treecount)
	{
		treeobject->angle.x = treeobject->angle.y = treeobject->angle.z = 0;;
		treeobject->setAllAngles( &treeobject->transform );
		treeobject->concatenatedTransform = treeobject->transform;
		treeobject->visible = true;

		treestorePtr = treestore;
		for( i = treecount; i--; )
		{
			
			treeobject->setPosition(treestorePtr);
			treeobject->concatenatedTransform.translation = *treestorePtr;

			treeobject->transformedBoundingBox.transformBoundBoxM3x4( &treeobject->modelData->boundingBox, &treeobject->concatenatedTransform);

			processObjectUnlit(treeobject);

			treestorePtr++;
		}
	}
#endif

}

//----------------------------------------------------------------------------------------------------------------------------
// renderWorld
// -----------
//				render the world (asuming that one exists)
//----------------------------------------------------------------------------------------------------------------------------

hunk * listOfHunkPtrs[8192];
extern int jjsHack5;
void engine::renderWorld()
{
	long xx, zz;

	float y0m01, y1m01, y0m11, y1m11, y0m21, y1m21;
	float x0m00, x1m00, x0m10, x1m10, x0m20, x1m20;
	float z0m02, z1m02, z0m12, z1m12, z0m22, z1m22; 
	float z0m02s, z0m12s, z0m22s;
	float A, B, C;
	float xhunkcoord, zhunkcoord;
	float px, py, lz;
	slong zhunkcount;
	uchar mask;
	int i;

	polygoncounter = 0;

	slong firstXhunk = 0;
	slong lastXhunk = currentworld->xhunks-1; 
	slong firstZhunk = 0;
	slong lastZhunk = currentworld->zhunks-1; 

	zhunkcount= 1+lastZhunk - firstZhunk;

	hunk * thisHunkX = currentworld->hunkList + firstXhunk * currentworld->zhunks;		

	mesh * meshData = currentworld->worldModel->meshData+FIRSTLOD;

	float MinY = meshData->MinCoordY - currentcamera->position.y;
	float MaxY = meshData->MaxCoordY - currentcamera->position.y;

	float Me00xsize = currentcamera->RotMatrix.E00 * currentworld->xhunksize;
	float Me10xsize = currentcamera->RotMatrix.E10 * currentworld->xhunksize;
	float Me20xsize = currentcamera->RotMatrix.E20 * currentworld->xhunksize;

	float Me02zsize = currentcamera->RotMatrix.E02 * currentworld->zhunksize;
	float Me12zsize = currentcamera->RotMatrix.E12 * currentworld->zhunksize;
	float Me22zsize = currentcamera->RotMatrix.E22 * currentworld->zhunksize;

	y0m01 = MinY * currentcamera->RotMatrix.E01; 
	y1m01 = MaxY * currentcamera->RotMatrix.E01;
 	y0m11 = MinY * currentcamera->RotMatrix.E11;
	y1m11 = MaxY * currentcamera->RotMatrix.E11;
	y0m21 = MinY * currentcamera->RotMatrix.E21;
	y1m21 = MaxY * currentcamera->RotMatrix.E21;
	
    xhunkcoord = meshData->MinCoordX  -  currentcamera->position.x;
    zhunkcoord = meshData->MinCoordZ  -  currentcamera->position.z;

	x0m00 = xhunkcoord * currentcamera->RotMatrix.E00;
	x0m10 = xhunkcoord * currentcamera->RotMatrix.E10;
	x0m20 = xhunkcoord * currentcamera->RotMatrix.E20;

	z0m02s = zhunkcoord * currentcamera->RotMatrix.E02;
 	z0m12s = zhunkcoord * currentcamera->RotMatrix.E12;
	z0m22s = zhunkcoord * currentcamera->RotMatrix.E22;

	hunk **hunkListPtr = listOfHunkPtrs;
	long hunkListCount = 0;
	float znear = -(currentworld->xhunksize);
	float zfar = currentcamera->zbackplane + (currentworld->xhunksize);

	for( xx = 1+lastXhunk-firstXhunk ; xx--;)
	{

	    thisHunk = thisHunkX + firstZhunk;

		z0m02 = z0m02s;
 		z0m12 = z0m12s;
		z0m22 = z0m22s;

		x1m00  = x0m00;
		x0m00 += Me00xsize;
		x1m10  = x0m10;
		x0m10 += Me10xsize; 
		x1m20  = x0m20;
		x0m20 += Me20xsize; 

		for( zz = zhunkcount; zz--;)
		{
			z1m02  = z0m02;
			z0m02 += Me02zsize;
			z1m12  = z0m12;
			z0m12 += Me12zsize;
			z1m22  = z0m22;
			z0m22 += Me22zsize;

			if( thisHunk->tcount_N )		// hunk not empty
			{
				C = x0m20 + y0m21 ;

				if((C+z0m22 < znear) || (C+z0m22 > zfar))goto wibble;

				A = x0m00 + y0m01 ;
				B = x0m10 + y0m11 ;

				  
				mask = 0x0F; 
				OFFSCREEN( A + z0m02, B + z0m12, C + z0m22 )
				if( mask )
				{
					OFFSCREEN( A + z1m02, B + z1m12, C + z1m22 )
					if( mask )
					{
						A = x0m00 + y1m01;
						B = x0m10 + y1m11;
						C = x0m20 + y1m21;
						OFFSCREEN( A + z0m02, B + z0m12, C + z0m22  )
						if( mask )
						{
							OFFSCREEN( A + z1m02, B + z1m12, C + z1m22  )
							if( mask )
							{
								A = x1m00 + y0m01;
								B = x1m10 + y0m11;
								C = x1m20 + y0m21;
								OFFSCREEN( A + z0m02, B + z0m12, C + z0m22  ) 
								if( mask )
								{
									OFFSCREEN( A + z1m02, B + z1m12, C + z1m22  )
									if( mask )
									{
										A = x1m00 + y1m01;
										B = x1m10 + y1m11;
										C = x1m20 + y1m21;
										OFFSCREEN( A + z0m02, B + z0m12, C + z0m22  )
										if( mask )
											OFFSCREEN( A + z1m02, B + z1m12, C + z1m22  ); 
									}
								}
							}
						}
					}
				}
				if( mask == 0)
				{
					*hunkListPtr++ = thisHunk;
					hunkListCount++;
				}
wibble:	;
			}
			thisHunk++;
		}	
		thisHunkX += currentworld->zhunks;   
	}

	*hunkListPtr = currentworld->mountainHunk;
	hunkListCount++;
	
	hunkListPtr = listOfHunkPtrs;
	for(i = hunkListCount;i--;)
	{
		thisHunk = *hunkListPtr++;
		renderthisHunk( thisHunk->tlistPtr_N, thisHunk->tcount_N) ;			// Non walls
		renderthisHunk( thisHunk->tlistPtr_W, thisHunk->tcount_W) ;			// Walls

	}  
}



#define SX4_A(a,b, c,d )\
	doubleSplitEdgeX2D( &Xsplit0, &Xsplit1,  c, d, displayDeltaX, displayWM1deltaX );\
	doubleSplitEdgeX2D( &Xsplit2, &Xsplit3,  a, b, displayWM1deltaX, displayDeltaX );\
	process2DTriangleY( &Xsplit3, &Xsplit2, &Xsplit0);\
	process2DTriangleY( &Xsplit1, &Xsplit0, &Xsplit2);
#define SX4_B(a,b, c,d )\
	doubleSplitEdgeX2D( &Xsplit0, &Xsplit1,  c, d, displayWM1deltaX, displayDeltaX );\
	doubleSplitEdgeX2D( &Xsplit2, &Xsplit3,  a, b, displayDeltaX, displayWM1deltaX );\
	process2DTriangleY( &Xsplit3, &Xsplit2, &Xsplit0);\
	process2DTriangleY( &Xsplit1, &Xsplit0, &Xsplit2);

//================================================================================================================================
//process2DTriangle
//-----------------
//
//	X clip a triangle in 2D to produce triangle(s) which are completed enclosed on screen within the X plane. They can then be
//  clipped to 'Y' by the method:  ' process2DTriangleY'
//
//================================================================================================================================

void engine::process2DTriangle( pointdata2d * p, pointdata2d * q, pointdata2d * r)		
{
	
	// firstly lets split triangles according to X
	static pointdata2d Xsplit0, Xsplit1, Xsplit2, Xsplit3;
	
	
	if(clip)
	{
		
		if ( (p->v->SCmaskX & q->v->SCmaskX & r->v->SCmaskX ) == 0 )
			if ( (p->v->SCmaskY & q->v->SCmaskY & r->v->SCmaskY ) == 0 )
				outputTriangle( p, q, r );
	}
	else
	{
		switch( (p->v->SCmaskX << 4) |  (q->v->SCmaskX << 2) |(r->v->SCmaskX ))
		{
			
			case 0:	// 00 00 00 .... No X split required.. do Y split on this triangle
				process2DTriangleY( p, q, r );
				break;
			
			case 21:		// 01 01 01 = completely off left - IGNORE
			case 42:		// 10 10 10 = completely off right - IGNORE
				break;
			
			case 1: // 00 00 01 ...  'r' is off left (x<0)
				splitEdgeX2D( &Xsplit0,  q, r, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayDeltaX);										 
				process2DTriangleY( p, q, &Xsplit1 );
				process2DTriangleY( q, &Xsplit0, &Xsplit1 );
				break;
				
			case 2: // 00 00 10 ...  'r' is off right (x>displayWM1deltaX)
				splitEdgeX2D( &Xsplit0,  q, r, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayWM1deltaX);										 
				process2DTriangleY( p, q, &Xsplit1 );
				process2DTriangleY( q, &Xsplit0, &Xsplit1 );
				break;
				
			case  4:		// 00 01 00	=   'q' is off left (x<0)
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayDeltaX);										 
				process2DTriangleY( r, p, &Xsplit0);
				process2DTriangleY( r, &Xsplit0, &Xsplit1 );
				break;
				
			case  8:		// 00 10 00 =   'q' is off right (x>displayWM1deltaX)
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX );
				splitEdgeX2D( &Xsplit1,  q, r, displayWM1deltaX);										 
				process2DTriangleY( r, p, &Xsplit0);
				process2DTriangleY( r, &Xsplit0, &Xsplit1 );
				break;
				
			case 16:		// 01 00 00	=   
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayDeltaX);										 
				process2DTriangleY( q, r, &Xsplit0);
				process2DTriangleY( r, &Xsplit1, &Xsplit0 );
				break;
				
			case 32:		// 10 00 00	=	
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayWM1deltaX);										 
				process2DTriangleY( q, r, &Xsplit0);
				process2DTriangleY( r, &Xsplit1, &Xsplit0 );
				break;
				
			case  5:		// 00 01 01 
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  p, r, displayDeltaX);										 
				process2DTriangleY( p, &Xsplit0, &Xsplit1);
				break;
				
			case 17:		// 01 00 01 
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayDeltaX);										 
				process2DTriangleY( q, &Xsplit1, &Xsplit0);
				break;
				
			case 20:		// 01 01 00 
				splitEdgeX2D( &Xsplit0,  r, p, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayDeltaX);										 
				process2DTriangleY( r, &Xsplit0, &Xsplit1);
				break;
				
			case 10:		// 00 10 10	
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  p, r, displayWM1deltaX);										 
				process2DTriangleY( p, &Xsplit0, &Xsplit1);
				break;
				
			case 34:		// 10 00 10 
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayWM1deltaX);										 
				process2DTriangleY( q, &Xsplit1, &Xsplit0);
				break;
				
			case 40:		// 10 10 00 
				splitEdgeX2D( &Xsplit0,  r, p, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayWM1deltaX);										 
				process2DTriangleY( r, &Xsplit0, &Xsplit1);
				break;
				
			case  6:		// 00 01 10 
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayWM1deltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3, q, r, displayWM1deltaX, displayDeltaX );
				process2DTriangleY( p, &Xsplit0, &Xsplit3);
				process2DTriangleY( p, &Xsplit3, &Xsplit2);
				process2DTriangleY( p, &Xsplit2, &Xsplit1);
				break;
				
			case  9:        // 00 10 01	
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  r, p, displayDeltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3, q, r,  displayDeltaX, displayWM1deltaX);
				process2DTriangleY( p, &Xsplit0, &Xsplit3);
				process2DTriangleY( p, &Xsplit3, &Xsplit2);
				process2DTriangleY( p, &Xsplit2, &Xsplit1);
				break;
				
			case 18:		// 01 00 10 =  'p' beyond backplane,  'r' in front of z==1
				splitEdgeX2D( &Xsplit0,  p, q, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayWM1deltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3, r, p, displayWM1deltaX, displayDeltaX );
				process2DTriangleY( q, &Xsplit1, &Xsplit2);
				process2DTriangleY( q, &Xsplit2, &Xsplit3);
				process2DTriangleY( q, &Xsplit3, &Xsplit0);
				break;
				
			case 33:		// 10 00 01	=  'p' in front of z==1,  'r' beyond backplane
				splitEdgeX2D( &Xsplit0,  p, q, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayDeltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3, r, p, displayDeltaX, displayWM1deltaX );
				process2DTriangleY( q, &Xsplit1, &Xsplit2);
				process2DTriangleY( q, &Xsplit2, &Xsplit3);
				process2DTriangleY( q, &Xsplit3, &Xsplit0);
				break;
				
			case 24:		// 01 10 00 =  'p' beyond backplane,  'q' in front of z==1
				splitEdgeX2D( &Xsplit0,  r, p, displayDeltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayWM1deltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3,  p, q, displayWM1deltaX, displayDeltaX );
				process2DTriangleY( r, &Xsplit0, &Xsplit3);
				process2DTriangleY( r, &Xsplit3, &Xsplit2);
				process2DTriangleY( r, &Xsplit2, &Xsplit1);
				break;
				
			case 36:		// 10 01 00 =  'p' in front of z==1,  'q' beyond backplane
				splitEdgeX2D( &Xsplit0,  r, p, displayWM1deltaX);
				splitEdgeX2D( &Xsplit1,  q, r, displayDeltaX);
				doubleSplitEdgeX2D( &Xsplit2, &Xsplit3,  p, q, displayDeltaX, displayWM1deltaX );
				process2DTriangleY( r, &Xsplit0, &Xsplit3);
				process2DTriangleY( r, &Xsplit3, &Xsplit2);
				process2DTriangleY( r, &Xsplit2, &Xsplit1);
				break;
				
			case 22:		// 01 01 10 
				SX4_A( r,p,   q,r);
				break;

			case 41:		// 10 10 01	
				SX4_B( r,p,   q,r);
				break;

			case 25:		// 01 10 01	
				SX4_A( q,r,   p,q);
				break;

			case 38:		// 10 01 10 	
				SX4_B( q,r,   p,q);
				break;

			case 26:		// 01 10 10  
				SX4_A( r,p,   p,q);
				break;

			case 37:		// 10 01 01 
				SX4_B( r,p,   p,q);
				break;
				/**/
		}
	}
}

ulong combineColour( float a, float i )
{
	slong t;
	float f;

	f = a + i;
	FASTFTOL( &t, f );
	if( t > 255) 
		return(255);
	return(t);
}

ulong combineColourL( float a, float i, float l, float sl)
{
	slong t;
	float f;

	float specresult = sl*sl*sl*sl*sl;
	specresult = specresult * sl*sl*sl * 3;

	f = a + i*l  + specresult*20;//* (z );
	FASTFTOL( &t, f );
	if( t > 255) 
		return(255);
	return(t);
}

ulong EcombineColourL( float a, float i, float l, float sl)
{
	slong t;
	float f;

	float specresult = sl*sl*sl*sl*sl;
	specresult = specresult * sl*sl*sl * 3;

	f = a + i*l + specresult*20;//*0.5f  ;//* (z );
	FASTFTOL( &t, f );
	if( t > 255) 
		return(255);
	return(t);
}

ulong formfog( float z )//, float y )
{
	float zz = z*z*z*z;
	float f = ( 1 -  zz * zz * zz) * 255.0f*5.5f;
	slong t;
	FASTFTOL( &t, f );
	if( t > 255 ) 
		t = 255;
	if( t < 0 ) 
		t = 0;

	return( ((ulong)t) << 24);
}
						
//
// Note that we draw NON-alphas first from FRONT to BACK, then we draw alphas on top from BACK to FRONT. This order is
// vitally important for two reasons:
//				- (i)  the alpha regions of alpha-textures must be alpha'd to the correct texture beneath.
//				- (ii) Non-alpha textures can utilise any intelligent rasterisation overdraw elimination methods implemented
//


__inline ulong engine::formHwareKeyAlpha( ulong k,  float f0, float f1, float f2 )
{
	slong l;
	float f;

	f = (f0 + f1 + f2) * 0.3333f * 256.0f * zNormalizer1;

	if( f >= (8191 * 256) ) 
		f = 8191 * 256;
	if( f <= 0) 
		f = 0;

	FASTFTOL( &l, (float)(8191.0f*256 - (f)));		// Alphas drawn in DESCENDING Z order (BACK TO FRONT)
	return(((((ulong)(l)) ))  | 0x080000000);		// key is  0x08000000 +   'z'(13) 't' (9)  'm'(9)
}

__inline ulong engine::formHwareKeyNonAlpha( ulong k,  float f0, float f1, float f2 )
{
	slong l;

	float f;

	f = (f0 + f1 + f2) * 0.3333f * zNormalizer1;
	if( f >= (8191) ) 
		f = 8191;
	if( f <= 0) 
		f = 0;

	FASTFTOL( &l, f);			// Non Alphas drawn in ASCENDING Z order (FRONT TO BACK)
	return ( (k | ((ulong)(l))) & 0x07FFFFFFF);							// key is  't'(9)  'm'(9)  'z'(13)
}


bool triangleNOTflat( vector2P * pvPtr, vector2P * qvPtr, vector2P * rvPtr )
{
	float min, max;
	slong minl, maxl;

	minmax3( pvPtr->y, qvPtr->y, rvPtr->y, &min, &max );

	FASTFTOL( &minl, min );
	FASTFTOL( &maxl, max );

	return( minl != maxl );
}


material dummyMaterial;

void engine::performRender()
{ 
	static FILE *aaa=NULL;
	
	outTriangle * tlist = currentOutListPtr->triangleList;
	outTriangle * llist = outTriangleNextPtr;
	sortlistentry * sortlistPtr = currentOutListPtr->sortlistPtr;
	ulong  zmask;
	ulong  count, thiskey;
	bool lit;
	bool thismapped, thisdetailed;
	float ambientRed, ambientGrn, ambientBlu, ambientAlpha;
	float diffuseRed, diffuseGrn, diffuseBlu, diffuseAlpha;
	
	if( tlist != llist)		// Not empty list
	{
		thiskey = tlist->mtl->sortingKey;
		if( tlist->mtl->bm.flags & MAT_ALPHA )	// alpha texture
			zmask = formHwareKeyAlpha(thiskey, tlist->p.v->v3.z,tlist->q.v->v3.z,tlist->r.v->v3.z );
		else
			zmask = formHwareKeyNonAlpha(thiskey, tlist->p.v->v3.z,tlist->q.v->v3.z,tlist->r.v->v3.z );
		
		sortlistPtr->key= zmask;
		(sortlistPtr++)->idx= (ulong) (tlist++);
		
		// other triangles
		while( tlist != llist )
		{
			thiskey = tlist->mtl->sortingKey;
			if( tlist->mtl->bm.flags & MAT_ALPHA )	// alpha texture
				thiskey = formHwareKeyAlpha(thiskey, tlist->p.v->v3.z,tlist->q.v->v3.z,tlist->r.v->v3.z );
			else
				thiskey = formHwareKeyNonAlpha(thiskey, tlist->p.v->v3.z,tlist->q.v->v3.z,tlist->r.v->v3.z );
			
			zmask |=  (zmask ^ thiskey);
			sortlistPtr->key= thiskey;
			(sortlistPtr++)->idx= (ulong)(tlist++);
		}
		sort( (count = llist - currentOutListPtr->triangleList), 
			zmask, 
			currentOutListPtr->sortlistPtr );
		//
		// render output list to display  ( if neither world nor objects exist then we should have nothing here!)
		// - use the currentOutListPtr->sortlistPtr to produce the triangles in order
		//
		
		ulong tnum;
		sortlistentry * slist =  currentOutListPtr->sortlistPtr;
		
		outTriangle   * tlist;
		
		D3DTLVERTEX * testVertex;
		D3DTLVERTEX2 * testVertex2;
		
		ulong total;
		
		vector2P * pvPtr, * qvPtr, * rvPtr;
		nongeodata * npvPtr, * nqvPtr, * nrvPtr;
		nongeodata * npvPtr1, * nqvPtr1, * nrvPtr1;
		bool shadow;
		
		tnum = 0;
		
#define LITCOLOUR  0x00FFFFFF
		if( lightSwitch )
		{ 
			while( tnum < count )
			{
				total = 0;
				currentSelectedMaterial = &dummyMaterial;
				
				while( (tnum < count) && ( total < BATCHSIZEM3) )
				{
					// if( slist->idx != 0x0FFFFFFFF)
					{
						tlist = (outTriangle * )slist->idx;
						thismapped = envon && (( tlist->mtl->bm.flags & MAT_ENVIRONMENT ) != 0);
						thisdetailed = deton && (( tlist->mtl->bm.flags & MAT_DETAILMAP ) != 0);
						
						shadow = (( tlist->mtl->bm.flags & MAT_SHADOW ) != 0);
//						lit = (( tlist->mtl->bm.flags & MAT_NOLIGHTING ) == 0);
						lit =false;
						
						//
						// If we change base material  ... OR ...
						// If we change from one to two textures (or vice versa)
						//
						
						if( ( tlist->mtl != currentSelectedMaterial ) 
							|| (thismapped != environmentMappedMaterial) 
							|| (thisdetailed != detailMappedMaterial)
							|| currentSelectedMaterial->bm.textureHandle==NULL)
						{
							currentSelectedMaterial = tlist->mtl;


							
							nulltexture = (currentSelectedMaterial->bm.textureHandle == NULL);
							
							if(total)
							{
								if( environmentMappedMaterial || detailMappedMaterial)			// old list was with environment mapping on
									renderList(vertexListD3D2, total, wframe, clip);
								else										// old list was without environment mapping
									renderList(vertexListD3D, total, wframe, clip);
								
								total = 0;
							}
							
							environmentMappedMaterial = thismapped;
							detailMappedMaterial = thisdetailed;
							
							if(thismapped)
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm, &skyEnvironment, MAP_ENVIRONMENT );
								testVertex2 = vertexListD3D2;
							}
							else 
							if (thisdetailed)
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm, &detailMap, MAP_DETAIL );
								testVertex2 = vertexListD3D2;
							}
							else
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm);
								testVertex = vertexListD3D;
							}
							
							ambientColour = currentworld->sunAmbientColour;
							diffuseColour = currentworld->sunIncidentColour;
							
							if(shadow)
								UdiffuseColour = currentSelectedMaterial->diffuse;							
							
							if(  nulltexture )
							{
								ambientColour = //currentSelectedMaterial->diffusec;
									diffuseColour = currentSelectedMaterial->diffusec;//currentworld->sunIncidentColour;
								UdiffuseColour = currentSelectedMaterial->diffuse;
							}
							//
							// Calculate colour float values for efficiency
							//
							ambientRed = (float)(ambientColour.red);
							ambientGrn = (float)(ambientColour.grn);
							ambientBlu = (float)(ambientColour.blu);
							ambientAlpha = (float)(ambientColour.alpha);
							diffuseRed = (float)(diffuseColour.red);
							diffuseGrn = (float)(diffuseColour.grn);
							diffuseBlu = (float)(diffuseColour.blu);
							diffuseAlpha = (float)(diffuseColour.alpha);
							
						}
						
						
						pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
						qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
						rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;
						
						if( environmentMappedMaterial  || detailMappedMaterial)
						{
							
							npvPtr1 = &tlist->p.nv1;
							nqvPtr1 = &tlist->q.nv1;
							nrvPtr1 = &tlist->r.nv1;
							
							testVertex2->sx  =  pvPtr->x ;
							testVertex2->sy  =  pvPtr->y ;
							testVertex2->sz = pvPtr->z ;
							testVertex2->rhw =  pvPtr->w ; //RECIPROCAL(1000 * pvPtr->z); //pvPtr->w ;
							testVertex2->tu1  = npvPtr->u ;
							testVertex2->tv1  = npvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = npvPtr->u * DETSCALE;
								testVertex2->tv2  = npvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = npvPtr1->u ;
								testVertex2->tv2  = npvPtr1->v ;
							}
							
							if(currentcamera->underwater)
								testVertex2->color = UNDERWATER_UDIFFUSE;
							else
							if( lit )
							{
								//PT1
									
								testVertex2->color = 
									( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
									( EcombineColourL(ambientRed,diffuseRed,pvPtr->l, pvPtr->specularl ) << 16) +
									( EcombineColourL(ambientGrn,diffuseGrn,pvPtr->l, pvPtr->specularl ) << 8 ) +
									( EcombineColourL(ambientBlu,diffuseBlu,pvPtr->l, pvPtr->specularl ) );
							} 
							else
								testVertex2->color = LITCOLOUR;
								
							if( boris2->fog )
								testVertex2->specular = formfog( testVertex2->sz);
								
							testVertex2++;
								
							testVertex2->sx  =  qvPtr->x ;
							testVertex2->sy  =  qvPtr->y ;
							testVertex2->sz = qvPtr->z ;
							testVertex2->rhw =  qvPtr->w ;
							testVertex2->tu1  = nqvPtr->u ;
							testVertex2->tv1  = nqvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = nqvPtr->u * DETSCALE;
								testVertex2->tv2  = nqvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = nqvPtr1->u  ;
								testVertex2->tv2  = nqvPtr1->v  ;
							}
								
								
							if(currentcamera->underwater)
								testVertex2->color = UNDERWATER_UDIFFUSE;
							else
							if( lit )
							{
								testVertex2->color = 
									( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
									( EcombineColourL(ambientRed,diffuseRed,qvPtr->l, qvPtr->specularl ) << 16) +
									( EcombineColourL(ambientGrn,diffuseGrn,qvPtr->l, qvPtr->specularl  ) << 8 ) +
										( EcombineColourL(ambientBlu,diffuseBlu,qvPtr->l, qvPtr->specularl ) );
							} 
							else
								testVertex2->color = LITCOLOUR;
									
							if( boris2->fog )
								testVertex2->specular = formfog( testVertex2->sz );//, qvPtr->v3.y );
									
									
							testVertex2++;
									
							testVertex2->sx  =  rvPtr->x ;
							testVertex2->sy  =  rvPtr->y ;
							testVertex2->sz  =  rvPtr->z ;
							testVertex2->rhw =  rvPtr->w ;
							testVertex2->tu1  = nrvPtr->u ;
							testVertex2->tv1  = nrvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = nrvPtr->u * DETSCALE;
								testVertex2->tv2  = nrvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = nrvPtr1->u ;
								testVertex2->tv2  = nrvPtr1->v ;
							}
									
							if(currentcamera->underwater)
								testVertex2->color = UNDERWATER_UDIFFUSE;
							else
							if (lit )
							{
								testVertex2->color = 
									( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
									( EcombineColourL(ambientRed,diffuseRed,rvPtr->l, rvPtr->specularl  ) << 16) +
									( EcombineColourL(ambientGrn,diffuseGrn,rvPtr->l, rvPtr->specularl ) << 8 ) +
									( EcombineColourL(ambientBlu,diffuseBlu,rvPtr->l, rvPtr->specularl  ) );
							} 
							else
								testVertex2->color = LITCOLOUR;
										
							if( boris2->fog )
								testVertex2->specular = formfog( testVertex2->sz );
										
										
							testVertex2++;
										
						} 
						else
						{
							testVertex->sx  =  pvPtr->x ;
							testVertex->sy  =  pvPtr->y ;
							testVertex->sz  =  pvPtr->z ;
							testVertex->rhw =  pvPtr->w ;
							testVertex->tu  = npvPtr->u ;
							testVertex->tv  = npvPtr->v ;
						
							if(currentcamera->underwater)
								testVertex->color = UNDERWATER_UDIFFUSE;
							else
							if(shadow)
							{
								testVertex->color = UdiffuseColour;
								
							} 
							else
							{
								if(lit)
								{
									testVertex->color = 
										( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
										( combineColourL(ambientRed,diffuseRed,pvPtr->l, pvPtr->specularl  ) << 16) +
										( combineColourL(ambientGrn,diffuseGrn,pvPtr->l, pvPtr->specularl ) << 8 ) +
										( combineColourL(ambientBlu,diffuseBlu,pvPtr->l, pvPtr->specularl ) );
								}
								else
									testVertex->color = LITCOLOUR;
								if( boris2->fog )
									testVertex->specular = formfog( testVertex->sz);//, pvPtr->v3.y );
							}
							
							testVertex++;
							
							testVertex->sx  =  qvPtr->x ;
							testVertex->sy  =  qvPtr->y ;
							testVertex->sz  =  qvPtr->z ;
							testVertex->rhw =  qvPtr->w ;
							testVertex->tu  = nqvPtr->u ;
							testVertex->tv  = nqvPtr->v ;
							
							if(currentcamera->underwater)
								testVertex->color = UNDERWATER_UDIFFUSE;
							else
							if(shadow)
							{
								testVertex->color = UdiffuseColour;
									
							} 
							else
							{
								if(lit)
								{
									testVertex->color = 
										( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
										( combineColourL(ambientRed,diffuseRed,qvPtr->l, qvPtr->specularl  ) << 16) +
										( combineColourL(ambientGrn,diffuseGrn,qvPtr->l, qvPtr->specularl ) << 8 ) +
										( combineColourL(ambientBlu,diffuseBlu,qvPtr->l, qvPtr->specularl  ) );
								} 
								else
									testVertex->color = LITCOLOUR;
									
								if( boris2->fog )
									testVertex->specular = formfog( testVertex->sz );//, qvPtr->v3.y );
							}
								
							testVertex++;
								
							testVertex->sx  =  rvPtr->x ;
							testVertex->sy  =  rvPtr->y ;
							testVertex->sz  =  rvPtr->z ;
							testVertex->rhw =  rvPtr->w ;
							testVertex->tu  = nrvPtr->u ;
							testVertex->tv  = nrvPtr->v ;
							
							if(currentcamera->underwater)
									testVertex->color = UNDERWATER_UDIFFUSE;
							else
								if(shadow)
								{
									testVertex->color = UdiffuseColour;
										
								} 
								else
								{
									if(lit)
									{
										testVertex->color = 
											( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
											( combineColourL(ambientRed,diffuseRed,rvPtr->l, rvPtr->specularl  ) << 16) +
											( combineColourL(ambientGrn,diffuseGrn,rvPtr->l, rvPtr->specularl ) << 8 ) +
											( combineColourL(ambientBlu,diffuseBlu,rvPtr->l, rvPtr->specularl ) );
									} 
									else
										testVertex->color = LITCOLOUR;
										
									if( boris2->fog )
										testVertex->specular = formfog( testVertex->sz );//, rvPtr->v3.y );
								}
									
								testVertex++;
							}
							total+=3;
						}	
						tnum++;
						slist++;
					}
			
					// BATCHSIZE REACHES OR ALL TRIANGLES REACHED
					if(total)
					{
						if( environmentMappedMaterial  || detailMappedMaterial)
							renderList(vertexListD3D2, total, wframe, clip);
						else
							renderList(vertexListD3D, total, wframe, clip);
						total = 0;
					}
			
				}
			}
			else
			{
				while( tnum < count )
				{
		
					total = 0;
		
					while( (tnum < count) && ( total < BATCHSIZEM3) )
					{
						tlist = (outTriangle * )slist->idx;
						thismapped = envon && (( tlist->mtl->bm.flags & MAT_ENVIRONMENT ) != 0);
						thisdetailed = deton && (( tlist->mtl->bm.flags & MAT_DETAILMAP ) != 0);
				
				
				
						//
						// If we change base material  ... OR ...
						// If we change from one to two textures (or vice versa)
						//
						if( ( tlist->mtl != currentSelectedMaterial ) 
								|| (thismapped != environmentMappedMaterial) 
								|| (thisdetailed != detailMappedMaterial)
							|| currentSelectedMaterial->bm.textureHandle==NULL)
						{
					
							currentSelectedMaterial = tlist->mtl;

							nulltexture = (currentSelectedMaterial->bm.textureHandle == NULL);
					
							if(total)
							{
								if(dumpframe)
								{
									if(!aaa)
										aaa=fopen("c:\\dump.log","w");
					
									fprintf(aaa,"%d\t%s\t%08lx\t%d\t%08x\n",
										tnum,tlist->mtl->textureDataPtr->filename,tlist->mtl->sortingKey,total,tlist->mtl);
									fflush(aaa);
								}
								if( environmentMappedMaterial  || detailMappedMaterial)			// old list was with environment mapping on
								{
									if(currentdisplay->mirrorx)
										for(int i=0;i<total; ++i)
											vertexListD3D2[i].sx = currentdisplay->mirrorVal - vertexListD3D2[i].sx;
									renderList(vertexListD3D2, total, wframe, clip);
								}
								else										// old list was without environment mapping
								{
									if(currentdisplay->mirrorx)
										for(int i=0;i<total; ++i)
											vertexListD3D[i].sx = currentdisplay->mirrorVal - vertexListD3D[i].sx;
									renderList(vertexListD3D, total, wframe, clip);
								}
								total = 0;
							}
					
							environmentMappedMaterial = thismapped;
							detailMappedMaterial = thisdetailed;
					
							if(thismapped)
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm, &skyEnvironment, MAP_ENVIRONMENT );
								testVertex2 = vertexListD3D2;
							}
							else 
							if (thisdetailed)
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm, &detailMap, MAP_DETAIL );
								testVertex2 = vertexListD3D2;
							}
							else
							{
								renderSetCurrentMaterial( &currentSelectedMaterial->bm);
								testVertex = vertexListD3D;
							}
					
							if(currentcamera->underwater)
								UdiffuseColour = UNDERWATER_UDIFFUSE;
							else
								UdiffuseColour = currentSelectedMaterial->diffuse;
						}
				
						pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
						qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
						rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;
				
						if( environmentMappedMaterial || detailMappedMaterial )
						{
							npvPtr1 = &tlist->p.nv1;
							nqvPtr1 = &tlist->q.nv1;
							nrvPtr1 = &tlist->r.nv1;
							testVertex2->sx  =  pvPtr->x ;
							testVertex2->sy  =  pvPtr->y ;
							testVertex2->sz  =  pvPtr->z ;
							testVertex2->rhw =  pvPtr->w ;
							testVertex2->tu1  = npvPtr->u ;
							testVertex2->tv1  = npvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = npvPtr->u * DETSCALE;
								testVertex2->tv2  = npvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = npvPtr1->u ;
								testVertex2->tv2  = npvPtr1->v ;
							}
						
							testVertex2->color = UdiffuseColour;
						if(pvPtr->l>9999.0f)
							testVertex2->color = 0x808080;
							testVertex2++;
						
							testVertex2->sx  =  qvPtr->x ;
							testVertex2->sy  =  qvPtr->y ;
							testVertex2->sz  =  qvPtr->z ;
							testVertex2->rhw =  qvPtr->w ;
							testVertex2->tu1  = nqvPtr->u ;
							testVertex2->tv1  = nqvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = nqvPtr->u * DETSCALE;
								testVertex2->tv2  = nqvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = nqvPtr1->u  ;
								testVertex2->tv2  = nqvPtr1->v  ;
							}
						
							testVertex2->color = UdiffuseColour;
						if(rvPtr->l>9999.0f)
							testVertex2->color = 0x808080;
						
							testVertex2++;
						
							testVertex2->sx  =  rvPtr->x ;
							testVertex2->sy  =  rvPtr->y ;
							testVertex2->sz  =  rvPtr->z ;
							testVertex2->rhw =  rvPtr->w ;
							testVertex2->tu1  = nrvPtr->u ;
							testVertex2->tv1  = nrvPtr->v ;
							if(detailMappedMaterial)
							{
								testVertex2->tu2  = nrvPtr->u * DETSCALE;
								testVertex2->tv2  = nrvPtr->v * DETSCALE;
							} 
							else
							{
								testVertex2->tu2  = nrvPtr1->u ;
								testVertex2->tv2  = nrvPtr1->v ;
							}
						
							testVertex2->color = UdiffuseColour;
						if(qvPtr->l>9999.0f)
							testVertex2->color = 0x808080;
						
							testVertex2++;
						} 
						else
						{
							testVertex->sx  =  pvPtr->x ;
							testVertex->sy  =  pvPtr->y ;
							testVertex->sz  =  pvPtr->z ;
							testVertex->rhw =  pvPtr->w ;
							testVertex->tu  = npvPtr->u ;
							testVertex->tv  = npvPtr->v ;
						
							testVertex->color = UdiffuseColour;
						if(pvPtr->l>9999.0f)
							testVertex->color = 0x808080;
						
							testVertex++;
						
							testVertex->sx  =  qvPtr->x ;
							testVertex->sy  =  qvPtr->y ;
							testVertex->sz  =  qvPtr->z ;
							testVertex->rhw =  qvPtr->w ;
							testVertex->tu  = nqvPtr->u ;
							testVertex->tv  = nqvPtr->v ;
						
							testVertex->color = UdiffuseColour;
						if(rvPtr->l>9999.0f)
							testVertex->color = 0x808080;
						
							testVertex++;
						
							testVertex->sx  =  rvPtr->x ;
							testVertex->sy  =  rvPtr->y ;
							testVertex->sz  =  rvPtr->z ;
							testVertex->rhw =  rvPtr->w ;
							testVertex->tu  = nrvPtr->u ;
							testVertex->tv  = nrvPtr->v ;
						
							testVertex->color = UdiffuseColour;
						if(qvPtr->l>9999.0f)
							testVertex->color = 0x808080;
						
							testVertex++;
						}
						total+=3;
					tnum++;
					slist++;
				
				}
				if(total)
				{
					if( environmentMappedMaterial  || detailMappedMaterial)			// old list was with environment mapping on
					{
						if(currentdisplay->mirrorx)
							for(int i=0;i<total; ++i)
								vertexListD3D2[i].sx = currentdisplay->mirrorVal - vertexListD3D2[i].sx;
						renderList(vertexListD3D2, total, wframe, clip);
					}
					else										// old list was without environment mapping
					{
						if(currentdisplay->mirrorx)
							for(int i=0;i<total; ++i)
								vertexListD3D[i].sx = currentdisplay->mirrorVal - vertexListD3D[i].sx;
						renderList(vertexListD3D, total, wframe, clip);
					}
				}
			}
		}
	}  // if (tlist
	
	if(dumpframe)
	{
		dumpframe=false;
		fclose(aaa);
	}

}

void  engine::start2Dlist()
{
	current2dvertexListPtr = vertex2dlist;
	currentvertex2dmatlistPtr  = vertex2dmatList;
}

void  engine::start2DlistEnv()
{
	current2dvertexListEnvPtr = vertex2dlistEnv;
	currentvertex2dmatlistEnvPtr  = vertex2dmatListEnv;
}


//
// Give up close values for both z and w so that whichever way the drivers buffer is ok!!
//

#define INYOURFACEZ 0.001f
#define INYOURFACEW 1.01f

//================================================================================================================================
//================================================================================================================================

void  engine::add2dvertex( float x, float y, float u, float v, DWORD col )
{
	current2dvertexListPtr->sx = x;
	current2dvertexListPtr->sy = y;
	current2dvertexListPtr->sz = INYOURFACEZ;
	current2dvertexListPtr->rhw = INYOURFACEW;
	current2dvertexListPtr->color = col;
	current2dvertexListPtr->specular=0;
	current2dvertexListPtr->tu = u;
	current2dvertexListPtr->tv = v;
	current2dvertexListPtr++;
}

void  engine::add2dvertex( float x, float y, float u, float v, float z,DWORD col )
{
	current2dvertexListPtr->sx = x;
	current2dvertexListPtr->sy = y;
	current2dvertexListPtr->sz = z;
	current2dvertexListPtr->rhw = INYOURFACEW;
	current2dvertexListPtr->color = col;
	current2dvertexListPtr->specular=0;
	current2dvertexListPtr->tu = u;
	current2dvertexListPtr->tv = v;
	current2dvertexListPtr++;
}

void  engine::add2dvertexEnv( float x, float y, float u1, float v1 , float u2, float v2,DWORD col)
{
	current2dvertexListEnvPtr->sx = x;
	current2dvertexListEnvPtr->sy = y;
	current2dvertexListEnvPtr->sz = INYOURFACEZ;
	current2dvertexListEnvPtr->rhw = INYOURFACEW;
	current2dvertexListEnvPtr->color = col;
	current2dvertexListEnvPtr->specular=0;
	current2dvertexListEnvPtr->tu1 = u1;
	current2dvertexListEnvPtr->tv1 = v1;
	current2dvertexListEnvPtr->tu2 = u2;
	current2dvertexListEnvPtr->tv2 = v2;
	current2dvertexListEnvPtr++;
}

void  engine::add2dvertexEnv( float x, float y, float u1, float v1, float u2, float v2, float z,DWORD col )
{
	current2dvertexListEnvPtr->sx = x;
	current2dvertexListEnvPtr->sy = y;
	current2dvertexListEnvPtr->sz = z;
	current2dvertexListEnvPtr->rhw = INYOURFACEW;
	current2dvertexListEnvPtr->color = col;
	current2dvertexListEnvPtr->specular=0;
	current2dvertexListEnvPtr->tu1 = u1;
	current2dvertexListEnvPtr->tv1 = v1;
	current2dvertexListEnvPtr->tu2 = u2;
	current2dvertexListEnvPtr->tv2 = v2;
	current2dvertexListEnvPtr++;
}
//================================================================================================================================
//addTo2Dlist
//-----------
//		add a texture 2D entry to the scene. (ALL CO-ORDS MUST BE ON SCREEN!!) -  used for HUD etc.
//================================================================================================================================

//debugger DBGR;
DWORD engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   char * filename, ulong flags)
{

		// every one material list entry covers 6 vertices (two triangles )

	DWORD hdl;
	currentvertex2dmatlistPtr->textureHandle = hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = flags | MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v ,shadow_2d);
	add2dvertex( x+w, y,  u+uw, v,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh,shadow_2d);

	add2dvertex( x, y, u, v,shadow_2d);
	add2dvertex( x+w, y+h, u+uw, v+vh,shadow_2d);
	add2dvertex( x, y+h, u, v+vh,shadow_2d );

	return( hdl );
}

DWORD engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   char * filename, ulong flags,   float z)
{

		// every one material list entry covers 6 vertices (two triangles )

	DWORD hdl;
	currentvertex2dmatlistPtr->textureHandle = hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = flags| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x+w, y,  u+uw, v, z,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh, z,shadow_2d );

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh, z,shadow_2d );
	add2dvertex( x, y+h, u, v+vh, z,shadow_2d );

	return( hdl );
}

DWORD engine::addTo2DlistRotated( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   char * filename, ulong flags,float rot,float rx,float ry,float z)
{

		// every one material list entry covers 6 vertices (two triangles )

	DWORD hdl;
	currentvertex2dmatlistPtr->textureHandle = hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = flags| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	rx *= coeff2dW;
	ry *= coeff2dH;

	float x2,y2,x3,y3,x4,y4;

	x2 = x+w;	y2 = y+h;
	x3 = x2;	y3 = y;
	x4 = x;		y4 = y2;

	rotatePointAroundPoint(&x,&y,rx,ry,rot);
	rotatePointAroundPoint(&x2,&y2,rx,ry,rot);
	rotatePointAroundPoint(&x3,&y3,rx,ry,rot);
	rotatePointAroundPoint(&x4,&y4,rx,ry,rot);

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x3, y3,  u+uw, v, z,shadow_2d );
	add2dvertex( x2, y2, u+uw, v+vh, z,shadow_2d );

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x2, y2, u+uw, v+vh, z,shadow_2d );
	add2dvertex( x4, y4, u, v+vh, z,shadow_2d );

	return( hdl );
}


#define	ENVMAPPED2D	0

float jjsHack1;
int jjsHack2=0;
DWORD engine::addTo2DlistRotatedEnvMapped( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   char * filename, ulong flags,float rot,float rx,float ry,float z)
{

		// every one material list entry covers 6 vertices (two triangles )
	float u1,v1,u2,v2,u3,v3,u4,v4;

	if(!envmapped2d)return 0;

	DWORD hdl;
	currentvertex2dmatlistEnvPtr->textureHandle = hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistEnvPtr->flags = flags| MAT_NOBLEND;
#if ENVMAPPED2D
	currentvertex2dmatlistEnvPtr->flags |= MAT_2DENVMAP;
#endif
	currentvertex2dmatlistEnvPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	rx *= coeff2dW;
	ry *= coeff2dH;

	float x2,y2,x3,y3,x4,y4;

	x2 = x+w;	y2 = y+h;
	x3 = x2;	y3 = y;
	x4 = x;		y4 = y2;

	rotatePointAroundPoint(&x,&y,rx,ry,rot);
	rotatePointAroundPoint(&x2,&y2,rx,ry,rot);
	rotatePointAroundPoint(&x3,&y3,rx,ry,rot);
	rotatePointAroundPoint(&x4,&y4,rx,ry,rot);


	if(jjsHack2==0)
	{
		u1 = 0.2;  v1 = 0.2;
		u2 = 0.4;  v2 = 0.2;
		u3 = 0.1;  v3 = 0.6;
		u4 = 0.4;  v4 = 0.6;
		++jjsHack2;
	}
	else
	{
		u1 = 0.4;  v1 = 0.2;
		u2 = 0.6;  v2 = 0.2;
		u3 = 0.4;  v3 = 0.6;
		u4 = 0.7;  v4 = 0.6;
		++jjsHack2;
	}

	rotatePointAroundPoint(&u1,&v1,0.4f,0.4f,jjsHack1);
	rotatePointAroundPoint(&u2,&v2,0.4f,0.4f,jjsHack1);
	rotatePointAroundPoint(&u3,&v3,0.4f,0.4f,jjsHack1);
	rotatePointAroundPoint(&u4,&v4,0.4f,0.4f,jjsHack1);

	add2dvertexEnv( x, y, u, v,u1,v1, z,shadow_2d );
	add2dvertexEnv( x3, y3,  u+uw,v ,u2,v2, z,shadow_2d );
	add2dvertexEnv( x2, y2, u+uw, v+vh, u4,v4,z,shadow_2d );

	add2dvertexEnv( x, y, u, v,u1,v1, z,shadow_2d );
	add2dvertexEnv( x2, y2, u+uw, v+vh, u4,v4,z,shadow_2d );
	add2dvertexEnv( x4, y4, u, v+vh,u3,v3, z,shadow_2d );


	return( hdl );
}

void  engine::addTo2DlistRotated( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle, ulong flags,float rot,float rx,float ry,float z)
{

	// every one material list entry covers 6 vertices (two triangles )

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = flags| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	rx *= coeff2dW;
	ry *= coeff2dH;

	float x2,y2,x3,y3,x4,y4;

	x2 = x+w;	y2 = y+h;
	x3 = x2;	y3 = y;
	x4 = x;		y4 = y2;

	rotatePointAroundPoint(&x,&y,rx,ry,rot);
	rotatePointAroundPoint(&x2,&y2,rx,ry,rot);
	rotatePointAroundPoint(&x3,&y3,rx,ry,rot);
	rotatePointAroundPoint(&x4,&y4,rx,ry,rot);
	
	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x3, y3,  u+uw, v, z,shadow_2d );
	add2dvertex( x2, y2, u+uw, v+vh, z,shadow_2d );

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x2, y2, u+uw, v+vh, z,shadow_2d );
	add2dvertex( x4, y4, u, v+vh, z,shadow_2d );

}

void  engine::addTo2DlistRotatedEnvMapped( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle, ulong flags,float rot,float rx,float ry,float z)
{

	float u1,v1,u2,v2;
	// every one material list entry covers 6 vertices (two triangles )

	if(!envmapped2d)return;
	currentvertex2dmatlistEnvPtr->textureHandle = texHandle;
	currentvertex2dmatlistEnvPtr->flags = flags| MAT_NOBLEND;

#if ENVMAPPED2D
	currentvertex2dmatlistEnvPtr->flags |= MAT_2DENVMAP;
#endif

	currentvertex2dmatlistEnvPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	rx *= coeff2dW;
	ry *= coeff2dH;

	float x2,y2,x3,y3,x4,y4;

	x2 = x+w;	y2 = y+h;
	x3 = x2;	y3 = y;
	x4 = x;		y4 = y2;

	rotatePointAroundPoint(&x,&y,rx,ry,rot);
	rotatePointAroundPoint(&x2,&y2,rx,ry,rot);
	rotatePointAroundPoint(&x3,&y3,rx,ry,rot);
	rotatePointAroundPoint(&x4,&y4,rx,ry,rot);

	u1 = x /800.0f;
	u2 = x2/800.0f;
	v1 = y/6000.0f;
	v2 = y2/6000.0f;

	add2dvertexEnv( x, y, u, v,u1,v1, z,shadow_2d );
	add2dvertexEnv( x3, y3,  u+uw,v ,u2,v1, z,shadow_2d );
	add2dvertexEnv( x2, y2, u+uw, v+vh, u2,v2,z,shadow_2d );

	add2dvertexEnv( x, y, u, v,u1,v1, z,shadow_2d );
	add2dvertexEnv( x2, y2, u+uw, v+vh, u2,v2,z,shadow_2d );
	add2dvertexEnv( x4, y4, u, v+vh,u1,v2, z,shadow_2d );

}


//================================================================================================================================


void  engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle)
{
	// every one material list entry covers 6 vertices (two triangles )

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = 0| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v,shadow_2d );
	add2dvertex( x+w, y,  u+uw, v,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh,shadow_2d );

	add2dvertex( x, y, u, v, shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh,shadow_2d );
	add2dvertex( x, y+h, u, v+vh,shadow_2d );
}

void  engine::addTo2Dlistcolor( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD color)
{
	// every one material list entry covers 6 vertices (two triangles )

	DWORD col = color | 0xff000000;

	currentvertex2dmatlistPtr->textureHandle = 0L;
	currentvertex2dmatlistPtr->flags = 0| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v,col);
	add2dvertex( x+w, y,  u+uw, v,col);
	add2dvertex( x+w, y+h, u+uw, v+vh,col);

	add2dvertex( x, y, u, v, col);
	add2dvertex( x+w, y+h, u+uw, v+vh,col);
	add2dvertex( x, y+h, u, v+vh,col);
}

void engine::addTriTo2Dlist( float x1,float y1,	float u1, float v1,
							float x2,float y2,	float u2, float v2,
							float x3,float y3,	float u3, float v3,
							DWORD texHandle)
{
	x1 *= coeff2dW;	x2 *= coeff2dW;	x3 *= coeff2dW;
	y1 *= coeff2dH;	y2 *= coeff2dH;	y3 *= coeff2dH;

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = 0| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	add2dvertex(x1,y1,u1,v1,shadow_2d);
	add2dvertex(x2,y2,u2,v2,shadow_2d);
	add2dvertex(x3,y3,u3,v3,shadow_2d);


	add2dvertex(x1,y1,u1,v1,shadow_2d);
	add2dvertex(x2,y2,u2,v2,shadow_2d);
	add2dvertex(x3,y3,u3,v3,shadow_2d);
}

void  engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle, float z)
{
	// every one material list entry covers 6 vertices (two triangles )

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = 0| MAT_NOBLEND;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v , z,shadow_2d);
	add2dvertex( x+w, y,  u+uw, v, z,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh, z,shadow_2d );

	add2dvertex( x, y, u, v, z,shadow_2d );
	add2dvertex( x+w, y+h, u+uw, v+vh, z,shadow_2d );
	add2dvertex( x, y+h, u, v+vh, z,shadow_2d );
}

//================================================================================================================================
// render2Dlist
// ------------
//		renders all 2D entries in the world.
//================================================================================================================================

void engine::render2Dlist()
{ 
	if( vertex2dmatList == currentvertex2dmatlistPtr )		// if nothing added
		return;

	D3DTLVERTEX   * thisvertex = vertex2dlist;
	BorisMaterial * thismaterial = vertex2dmatList;
	D3DTLVERTEX   * firstvertex;

	DWORD inusetexture;
	long counter;

	inusetexture = thismaterial->textureHandle;
	counter = 0;
	firstvertex = thisvertex;

	renderSetCurrentMaterial(thismaterial);
	while( thismaterial != currentvertex2dmatlistPtr )
	{
		if( inusetexture != thismaterial->textureHandle )
		{
			renderList(firstvertex, counter, wframe, clip);
			inusetexture = thismaterial->textureHandle;
			renderSetCurrentMaterial(thismaterial);
			counter = 0;
			firstvertex = thisvertex;
		}
		counter += 6;
		thisvertex+=6;
		thismaterial++;
	}
	renderList(firstvertex, counter, wframe, clip);
}
//================================================================================================================================
// render2Dlist
// ------------
//		renders all 2D entries in the world.
//================================================================================================================================

void engine::render2DlistEnv()
{ 
	if( vertex2dmatListEnv == currentvertex2dmatlistEnvPtr )		// if nothing added
		return;

	D3DTLVERTEX2   * thisvertex = vertex2dlistEnv;
	BorisMaterial * thismaterial = vertex2dmatListEnv;
	D3DTLVERTEX2   * firstvertex;

	DWORD inusetexture;
	long counter;

	inusetexture = thismaterial->textureHandle;
	counter = 0;
	firstvertex = thisvertex;

	renderSetCurrentMaterial(thismaterial, &skyEnvironment, MAP_ENVIRONMENT);
	while( thismaterial != currentvertex2dmatlistEnvPtr )
	{
		if( inusetexture != thismaterial->textureHandle )
		{
			renderList(firstvertex, counter, wframe, clip);
			inusetexture = thismaterial->textureHandle;
			renderSetCurrentMaterial(thismaterial, &skyEnvironment, MAP_ENVIRONMENT  );
			counter = 0;
			firstvertex = thisvertex;
		}
		counter += 6;
		thisvertex+=6;
		thismaterial++;
	}
	renderList(firstvertex, counter, wframe, clip);
	jjsHack2 = 0;
}

//================================================================================================================================
// render
// ------
//		- render the world and everything in it using the current camera, display, world, objectLists....
//			
//		- This routine embodies triangle splitting and projection and production of the final triangle lists to render and
//		  consequently is a bit complicated!! especially as we need to do this as efficiently as we can.
//
// TODO::
//			- replace two splitEdgeZ on same line to a single routine (and hence save some maths)
//================================================================================================================================

void engine::renderstart()
{
	globalpctr++;

    currentSelectedMaterial = &dummyMaterial;//NULL;

	outNextVector2Pptr =  currentOutListPtr->vector2PList;
	outNextNonGeoptr   =  currentOutListPtr->nongeoList;
	outTriangleNextPtr =  currentOutListPtr->triangleList;
}


//================================================================================================================================
// ShadowToGround
// ---------------
//				A square area defining a shadow texture 
//================================================================================================================================

void engine::render()
{
	if(!zoomdone)
	{
		ulong ticks = zoomtimer.getTicks();
		
		zoomtickselapsed += ticks;
		zoomproportiondone = zoomtickselapsed * oneovertotalzoomticks;
		if( zoomproportiondone > 1.0f)
			zoomproportiondone = 1.0f;
		
		if(ticks)
		{
			zoomz += ticks * zoompertick;
			if(zoompertick >= 0)
			{
				if( zoomz >= zoomdistance )
				{
					zoomz = zoomdistance;
					zoomdone = true;
				}
			} else
			if( zoomz <= zoomdistance )
			{
				zoomz = zoomdistance;
				zoomdone = true;
			}
		}
	}

	if(currentcamera->moved)
	{
		currentcamera->checkUnderWater();
		currentcamera->moved = false;
	}

	//
	// render according to what has been set
	//

	STARTSTAT(3, 0,255,0 );
	renderObjects();
	ENDSTAT(3);

	if(skyon)
		if( currentSky)
		{
			currentSky->updateIfNeeded();
			renderSky();
		}
	STARTSTAT(2, 0,255,0 );
	if(currentworld) 
		renderWorld();
	ENDSTAT(2);

	renderSpriteFXlist();

#ifdef FEATURE_ROLLOVERNUMBER
			//rollOverList;
#endif


	STARTSTAT(5, 255,0,0);
	performRender();

	outNextVector2Pptr =  currentOutListPtr->vector2PList;
	outNextNonGeoptr   =  currentOutListPtr->nongeoList;
	outTriangleNextPtr =  currentOutListPtr->triangleList;

	ENDSTAT(5);

	render2Dlist();
	render2DlistEnv();
}

//================================================================================================================================
// insertObject
// ------------
//					place an object into the engines chain of objects - at the front of the list
//================================================================================================================================

void engine::insertObject( object * optr )
{
	optr->prev = NULL;
	optr->next = objectList;
	objectList = optr;
}

//================================================================================================================================
// removeObject
// ------------
//				   remove an object from the engines chain of objects
//================================================================================================================================

void engine::removeObject( object * optr )
{
	if( optr->prev )
		optr->prev->next = optr->next;
	if( optr->next)
		optr->next->prev = optr->prev;

	if( objectList == optr )
		objectList = optr->next;
}

//================================================================================================================================
// removeAllObjects
// ----------------
//					remove all objects from the engines chain of objects...
//================================================================================================================================

void engine::removeAllObjects()
{
	object * currentObjPtr = objectList;
	object * nextObjPtr;

	while( currentObjPtr )
	{
		nextObjPtr = currentObjPtr->next;

		currentObjPtr->deleteObjectTree();
	
		currentObjPtr = nextObjPtr;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------
// checkVisibility
// ---------------
//
//			check the visibility of a Rboundbox against the current display
// 
//--------------------------------------------------------------------------------------------------------------------------------

bool engine::checkRboundboxVisibility(RboundBox * r)
{
	static vector3 tempvectorN;
	static vector3 result;

	#define OFFSCREENA( x3, y3, z3 )  \
	{  \
		px = displayWd2divprojcoeffW*z3; \
		py = displayHd2divprojcoeffH*z3; \
		mask &= ( \
				(( x3 < -px ) ? 1 : ( (x3 > px) ? 2 : 0 )) |\
				(( y3 < -py ) ? 4 : ( (y3 > py) ? 8 : 0 )) |\
				(( z3 < ZFRONTPLANE ) ? 16 : ( (z3 > currentcamera->zbackplane) ? 32 : 0 )) \
			    ); 		 \
		if( mask == 0) return true; \
	} 

    float px,py;
	uchar mask; 

	mask = 0x3F;
	tempvectorN = r->v000 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );

	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v001 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v010 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v011 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v100 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v101 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v110 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v111 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x3mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )

	return false;
}
 
//
// moved = has window moved
// heldlParam = lParam fro WM_MOVE ofwindows message handler if move occurs.
//

void engine::beginFrame( bool moved, LPARAM heldlParam)
{
	renderBeginScene( (!skyon) | (wframe) );

	start2Dlist();
	start2DlistEnv();

	if( moved )			
		renderMove(heldlParam);
		
	renderstart();
}

void engine::lightingOn()
{
	if(currentworld)
		currentworld->turnSunOn();
	lightSwitch = true;
}

void engine::lightingOff()
{
	if(currentworld)
		currentworld->turnSunOff();
	lightSwitch = false;
}

spriteFXentry * engine::appendSpriteFX( spriteFX * sfx, vector3 * position, ulong * counter, bool fU, bool fV )
{
    spriteFXentry * newEntry = spritePool->Allocate();

	if(!newEntry)
	{
		return NULL;
	}

	if(counter)
		(*counter)++;

	if(sfx->ydrift)
	{
		newEntry->mydrift.x = (float)((rand()%13)-6)*(1.0f / 300.0f);
		newEntry->mydrift.y = (float)((rand()%13)*(1.0f/300.0f)+0.01f);
		newEntry->mydrift.z = (float)((rand()%13)-6)*(1.0f / 300.0f);
	}

	if(sfx->engydrift)
	{
		//newEntry->mydrift.x = (float)((rand()%13)-6)*(1.0f / 300.0f);
		newEntry->mydrift.x = 0.0f;
		newEntry->mydrift.y = (float)((rand()%13)*(1.0f/300.0f)+0.01f);
		newEntry->mydrift.z = 0.0f;
		//newEntry->mydrift.z = (float)((rand()%13)-6)*(1.0f / 300.0f);
	}

	newEntry->flipU = fU;
	newEntry->flipV = fV;

	newEntry->externalcounter = counter;

	spriteVertexGroup *vg = vertexPool->Allocate();

	if(!vg)
	{
		spritePool->Free(newEntry);
		return NULL;
	}

	newEntry->triangleB.p.v =
	newEntry->triangleA.p.v = &(vg->a);

	newEntry->triangleA.q.v = &(vg->b);

	newEntry->triangleB.q.v =
	newEntry->triangleA.r.v = &(vg->c);

	newEntry->triangleB.r.v = &(vg->d);


	newEntry->spriteFXptr = sfx;
	newEntry->worldPosition = *position;
	newEntry->currentFrame = sfx->firstFrame;
	newEntry->currentCount = sfx->repeatCount;
	newEntry->reverse = false;					// signifies current direction of sequence
	if( newEntry->useTimer = (sfx->framesPerSecond > 0) )
		newEntry->frameTimer.setTimer(sfx->framesPerSecond );
	newEntry->firstFrame = true;

	if( newEntry->next = spriteFXlist)	// = is correct
		spriteFXlist->prev = newEntry;

	newEntry->prev = NULL;

	spriteFXlist = newEntry;
	return( newEntry );
}	


void engine::RotatePoints(vertex *pvPtr,vertex *qvPtr,vertex *rvPtr,float x,float y)
{
	rotatePointAroundPoint(&pvPtr->transformed.x,&pvPtr->transformed.y,x,y,cameraangle);
	rotatePointAroundPoint(&qvPtr->transformed.x,&qvPtr->transformed.y,x,y,cameraangle);
	rotatePointAroundPoint(&rvPtr->transformed.x,&rvPtr->transformed.y,x,y,cameraangle);
}

// 
//  update sprite list (will include delinking when a spriteFX occurence used up) 
//  and render the frames. (if projected on screen)
//
 

void engine::renderSpriteFXlist()
{
	//  NOTE THAT WHEN WE HAVE FINISHED WITH A SPRITE EFFECT WE MUST DESTORY ITS 'SPITEFXENTRY' OCCURENCE

	spriteFXentry * thisFXentry = spriteFXlist;
	spriteFXentry * oldthisFXentry;
	static vector3 tempcoord,basepoint;
	float actualLengthX, actualLengthY;

	vertex   * pvPtr, * qvPtr, * rvPtr;

	envmapping = false;		// NO ENVIRONMENT MAPPING ON SPRITES!!!!!
	detmapping = false;

	while( thisFXentry)
	{
		ulong framesToGo = 0;
		if( thisFXentry->useTimer )
		{
 			framesToGo = thisFXentry->frameTimer.getTicks();
			if( framesToGo == 0)
				if( thisFXentry->firstFrame ) framesToGo = 1;
		} 
		else
		if(thisFXentry->firstFrame )
			framesToGo = 1;

		thisFXentry->firstFrame = false;

		bool  disconnectEntry = false; 
	
		//
		// advance to next frame(s) in sequence according to the time elapsed
		//

	
		if(runsprites)while( framesToGo ) 
		{
			if (thisFXentry->currentFrame == NULL)
				break; // IMG

			if(thisFXentry->reverse)
				thisFXentry->currentFrame = thisFXentry->currentFrame->prev;
			else
				thisFXentry->currentFrame = thisFXentry->currentFrame->next;

			if( thisFXentry->currentFrame == NULL)
			{
				thisFXentry->currentCount--;
				if(thisFXentry->currentCount)		// do we have any more iterations of the series ?
				{
					if( thisFXentry->spriteFXptr->oscillate )	// change direction ?
						thisFXentry->reverse = !thisFXentry->reverse;
					if(thisFXentry->reverse)
						thisFXentry->currentFrame = thisFXentry->spriteFXptr->lastFrame;
					else
						thisFXentry->currentFrame = thisFXentry->spriteFXptr->firstFrame;
				
				} 
				else
				{
					disconnectEntry = true;
					break;			// No more iterations... IGNORE ANY REMAINING FRAMES TO GO
				}
			}

			// frame exists and has changed so update anything possible

			triPtr = &thisFXentry->triangleA;
			triangle * triPtrB = &thisFXentry->triangleB;


			triPtrB->p.nv = triPtr->p.nv =  thisFXentry->currentFrame->nv;

			triPtr->q.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
			triPtr->q.nv.v = triPtr->p.nv.v;

			triPtrB->q.nv.u = triPtr->r.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
			triPtrB->q.nv.v = triPtr->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

			triPtrB->r.nv.u = triPtr->p.nv.u ;
			triPtrB->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

			if( !thisFXentry->flipV )
			{
				triPtr->p.nv.v = 1.0f - triPtr->p.nv.v;
				triPtr->q.nv.v = 1.0f - triPtr->q.nv.v;
				triPtr->r.nv.v = 1.0f - triPtr->r.nv.v;

				triPtrB->p.nv.v = 1.0f - triPtrB->p.nv.v;
				triPtrB->q.nv.v = 1.0f - triPtrB->q.nv.v;
				triPtrB->r.nv.v = 1.0f - triPtrB->r.nv.v;

			}

			if(runsprites)framesToGo--;
		}

		//
		// advance to next FX list entry
		//

	
		if( disconnectEntry)
		{
			if( spriteFXlist == thisFXentry )
				spriteFXlist = thisFXentry->next;
				
			if( thisFXentry->next )
				thisFXentry->next->prev = thisFXentry->prev;

			if( thisFXentry->prev )
				thisFXentry->prev->next = thisFXentry->next;

			//
			// if an external counter exists then decrement it (the address of the counter held here!)
			//

			if(runsprites)
			{
				ulong * counter = thisFXentry->externalcounter;
				if( counter )
					(*counter)--;
			}

			oldthisFXentry = thisFXentry;
			thisFXentry = thisFXentry->next;

			// remove the data from the pools

			vertexPool->Free((spriteVertexGroup *)(oldthisFXentry->triangleA.p.v)); // must be first tri in group
			spritePool->Free(oldthisFXentry);
		}
		else
		{
			// Render frame
	
			spriteFXframe * thisFrame = thisFXentry->currentFrame;
			if( thisFrame)
			{
				vertex * oldrvPtr;
				
				currentmtl = &thisFrame->mat; //&thisFXentry->spriteFXptr->mat;
					
				triPtr = &thisFXentry->triangleA; //new triangle;

				pvPtr = triPtr->p.v; 
						
				actualLengthX = thisFrame->xlength;
				actualLengthY = thisFrame->ylength;


// JCF change to make some sprites float upwards

				if(runsprites)
				{
					if( thisFrame->wvariation )
						actualLengthX +=  ((float)(rand()) * thisFrame->wvariation ) * randomScale ;
					if( thisFrame->hvariation )
						actualLengthY += ((float)(rand()) * thisFrame->hvariation ) * randomScale ;
				}

				boris2->currentcamera->transformWorldVector(&tempcoord, &( thisFXentry->worldPosition +
						thisFrame->offset )); 
				if(runsprites && (thisFXentry->spriteFXptr->ydrift || thisFXentry->spriteFXptr->engydrift))
					thisFXentry->worldPosition += thisFXentry->mydrift;

				basepoint = tempcoord;

				tempcoord.x -= actualLengthX * 0.5f;
				tempcoord.y -= actualLengthY * 0.5f;

				pvPtr->transformed = tempcoord;// + currentcamera->position;

				if(runsprites)
				{
					if( thisFrame->xvariation )
						pvPtr->transformed.x +=((float)(rand()) * thisFrame->xvariation ) * randomScale ;
					if( thisFrame->yvariation )
						pvPtr->transformed.y +=((float)(rand()) * thisFrame->yvariation ) * randomScale ; 
				}

		
				pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					
				qvPtr = triPtr->q.v;
				oldrvPtr = rvPtr = triPtr->r.v;

				rvPtr->transformed.x =
				qvPtr->transformed.x = pvPtr->transformed.x + actualLengthX;

				qvPtr->transformed.y = pvPtr->transformed.y;

				rvPtr->transformed.y = pvPtr->transformed.y + actualLengthY;

				rvPtr->transformed.z =
				qvPtr->transformed.z = pvPtr->transformed.z;

				qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
				rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);


RotatePoints(pvPtr,qvPtr,rvPtr,basepoint.x,basepoint.y);
					
				mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);

				if( (mask & 48) == 0 )				// if vertex is between z-planes
				{

					projectedp.nv = &triPtr->p.nv;
					project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, 1.0f, 0.0f );
					projectedp.v = pvPtr->projectedPtr;
				}
			
				if( (mask & 12) == 0 )
				{
					projectedq.nv = &triPtr->q.nv;
					project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, 1.0f, 0.0f );
					projectedq.v = qvPtr->projectedPtr;	
				}

				if( (mask & 3) == 0 )
				{
					projectedr.nv = &triPtr->r.nv;
					project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f, 0.0f);
					projectedr.v = rvPtr->projectedPtr;	
				}

				zsplitting();

				//
				//new triangle;
				//
				
				triPtr = &thisFXentry->triangleB; 

				qvPtr = triPtr->q.v;
				rvPtr = triPtr->r.v;

				rvPtr->transformed.y = 
				qvPtr->transformed.y = oldrvPtr->transformed.y;

				rvPtr->transformed.x = pvPtr->transformed.x;
				rvPtr->transformed.z = pvPtr->transformed.z;

				qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
				rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					
RotatePoints(pvPtr,qvPtr,rvPtr,basepoint.x,basepoint.y);

				mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);

				if( (mask & 12) == 0 )
				{
					projectedq.nv = &triPtr->q.nv;
					//qvPtr->projectedPtr= oldrvPtr->projectedPtr;
					projectedq.v = oldrvPtr->projectedPtr;
				}
			
				if( (mask & 3) == 0 )
				{
					projectedr.nv = &triPtr->r.nv;
					project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f, 0.0f);
					projectedr.v = rvPtr->projectedPtr;	
				}
				zsplitting();

			}


			thisFXentry = thisFXentry->next;
		}
	}
}							// update and render spriteFX's

#if 0
void  engine::renderSpriteFXlist()
{
	//  NOTE THAT WHEN WE HAVE FINISHED WITH A SPRITE EFFECT WE MUST DESTORY ITS 'SPITEFXENTRY' OCCURENCE

	spriteFXentry * thisFXentry = spriteFXlist;
	spriteFXentry * oldthisFXentry;
	static vector3 tempcoord;
	float actualLengthX, actualLengthY;

	vertex   * pvPtr, * qvPtr, * rvPtr;


	envmapping = false;		// NO ENVIRONMENT MAPPING ON SPRITES!!!!!
	detmapping = false;

	while( thisFXentry)
	{
		ulong framesToGo = 0;
		if( thisFXentry->useTimer )
		{
 			framesToGo = thisFXentry->frameTimer.getTicks();
			if( framesToGo == 0)
				if( thisFXentry->firstFrame ) framesToGo = 1;
		} 
		else
		if(thisFXentry->firstFrame )
			framesToGo = 1;

		thisFXentry->firstFrame = false;

		bool  disconnectEntry = false; 
	
		//
		// advance to next frame(s) in sequence according to the time elapsed
		//

	
		while( framesToGo ) 
		{
			if (thisFXentry->currentFrame == NULL)
				break; // IMG

			if(thisFXentry->reverse)
				thisFXentry->currentFrame = thisFXentry->currentFrame->prev;
			else
				thisFXentry->currentFrame = thisFXentry->currentFrame->next;

			if( thisFXentry->currentFrame == NULL)
			{
				thisFXentry->currentCount--;
				if(thisFXentry->currentCount)		// do we have any more iterations of the series ?
				{
					if( thisFXentry->spriteFXptr->oscillate )	// change direction ?
						thisFXentry->reverse = !thisFXentry->reverse;
					if(thisFXentry->reverse)
						thisFXentry->currentFrame = thisFXentry->spriteFXptr->lastFrame;
					else
						thisFXentry->currentFrame = thisFXentry->spriteFXptr->firstFrame;
				
				} 
				else
				{
					disconnectEntry = true;
					break;			// No more iterations... IGNORE ANY REMAINING FRAMES TO GO
				}
			}
			// frame exists and has changed so update anything possible

			triPtr = &thisFXentry->triangleA;
			triangle * triPtrB = &thisFXentry->triangleB;


			triPtrB->p.nv = triPtr->p.nv =  thisFXentry->currentFrame->nv;

			triPtr->q.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
			triPtr->q.nv.v = triPtr->p.nv.v;

			triPtrB->q.nv.u = triPtr->r.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
			triPtrB->q.nv.v = triPtr->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

			triPtrB->r.nv.u = triPtr->p.nv.u ;
			triPtrB->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

			if( !thisFXentry->flipV )
			{
				triPtr->p.nv.v = 1.0f - triPtr->p.nv.v;
				triPtr->q.nv.v = 1.0f - triPtr->q.nv.v;
				triPtr->r.nv.v = 1.0f - triPtr->r.nv.v;

				triPtrB->p.nv.v = 1.0f - triPtrB->p.nv.v;
				triPtrB->q.nv.v = 1.0f - triPtrB->q.nv.v;
				triPtrB->r.nv.v = 1.0f - triPtrB->r.nv.v;

			}

			framesToGo--;
		}

		//
		// advance to next FX list entry
		//

	
		if( disconnectEntry)
		{
			if( spriteFXlist == thisFXentry )
				spriteFXlist = thisFXentry->next;
				
			if( thisFXentry->next )
				thisFXentry->next->prev = thisFXentry->prev;

			if( thisFXentry->prev )
				thisFXentry->prev->next = thisFXentry->next;

			//
			// if an external counter exists then decrement it (the address of the counter held here!)
			//

			ulong * counter = thisFXentry->externalcounter;
			if( counter )
				(*counter)--;

			oldthisFXentry = thisFXentry;
			thisFXentry = thisFXentry->next;

			delete oldthisFXentry;
		}
		else
		{
			// Render frame
	
			spriteFXframe * thisFrame = thisFXentry->currentFrame;
			if( thisFrame)
			{
				vertex * oldrvPtr;
				
				currentmtl = &thisFrame->mat; //&thisFXentry->spriteFXptr->mat;
					
				triPtr = &thisFXentry->triangleA; //new triangle;

				pvPtr = triPtr->p.v; 
						
				actualLengthX = thisFrame->xlength;
				actualLengthY = thisFrame->ylength;

				if( thisFrame->wvariation )
					actualLengthX +=  ((float)(rand()) * thisFrame->wvariation ) * randomScale ;
				if( thisFrame->hvariation )
					actualLengthY += ((float)(rand()) * thisFrame->hvariation ) * randomScale ;
				boris2->currentcamera->transformWorldVector(&tempcoord, &( thisFXentry->worldPosition +
							thisFrame->offset )); 

// JCF change to make some sprites float upwards

				if(thisFXentry->spriteFXptr->ydrift || thisFXentry->spriteFXptr->engydrift)
					thisFXentry->worldPosition += thisFXentry->mydrift;

				tempcoord.x -= actualLengthX * 0.5f;
				tempcoord.y -= actualLengthY * 0.5f;

				pvPtr->transformed = tempcoord;// + currentcamera->position;

				if( thisFrame->xvariation )
					pvPtr->transformed.x +=((float)(rand()) * thisFrame->xvariation ) * randomScale ;
				if( thisFrame->yvariation )
					pvPtr->transformed.y +=((float)(rand()) * thisFrame->yvariation ) * randomScale ; 

		
				pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					
				qvPtr = triPtr->q.v;
				oldrvPtr = rvPtr = triPtr->r.v;

				rvPtr->transformed.x =
				qvPtr->transformed.x = pvPtr->transformed.x + actualLengthX;

				qvPtr->transformed.y = pvPtr->transformed.y;

				rvPtr->transformed.y = pvPtr->transformed.y + actualLengthY;

				rvPtr->transformed.z =
				qvPtr->transformed.z = pvPtr->transformed.z;

				qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
				rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);

					
				mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);

				if( (mask & 48) == 0 )				// if vertex is between z-planes
				{

					projectedp.nv = &triPtr->p.nv;
					project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, 1.0f, 0.0f );
					projectedp.v = pvPtr->projectedPtr;
				}
			
				if( (mask & 12) == 0 )
				{
					projectedq.nv = &triPtr->q.nv;
					project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, 1.0f, 0.0f );
					projectedq.v = qvPtr->projectedPtr;	
				}

				if( (mask & 3) == 0 )
				{
					projectedr.nv = &triPtr->r.nv;
					project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f, 0.0f);
					projectedr.v = rvPtr->projectedPtr;	
				}

				zsplitting();

				//
				//new triangle;
				//
				
				triPtr = &thisFXentry->triangleB; 

				qvPtr = triPtr->q.v;
				rvPtr = triPtr->r.v;

				rvPtr->transformed.y = 
				qvPtr->transformed.y = oldrvPtr->transformed.y;

				rvPtr->transformed.x = pvPtr->transformed.x;
				rvPtr->transformed.z = pvPtr->transformed.z;

				qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
				rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					
				mask = ((pvPtr->SCmaskInZ)<<4) | ((qvPtr->SCmaskInZ)<<2) | (rvPtr->SCmaskInZ);

				if( (mask & 12) == 0 )
				{
					projectedq.nv = &triPtr->q.nv;
					//qvPtr->projectedPtr= oldrvPtr->projectedPtr;
					projectedq.v = oldrvPtr->projectedPtr;
				}
			
				if( (mask & 3) == 0 )
				{
					projectedr.nv = &triPtr->r.nv;
					project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f, 0.0f);
					projectedr.v = rvPtr->projectedPtr;	
				}
				zsplitting();

			}


			thisFXentry = thisFXentry->next;
		}
	}
}							// update and render spriteFX's

#endif

static char zzz[222];

class spritenamestore{
	
public:
	spritenamestore(){};
	~spritenamestore(){};

	char name[64];
	spriteFX * sfx;

	//private:
};

static spritenamestore spritename[100];


char *tempBuffer11;
void engine::loadInSpriteList(float worldscale)
{
	// read the 'spriteCoords.txt' file from the archive
	// - for each line in the file read off the name of the sprite and the appropriate co-ordinate
	// if the name is not already held in a list then create a new entry 'new spriteFX("sparks")'
	// - create an occurence of the sprite.


	ulong texhandle = arcExtract_txt( "SpriteCoords" );

	if(!texhandle)return;	// no sprite coords

	char *SpriteData = arcGetData(texhandle);

	if(!SpriteData)return;	// extra test

	int SpriteDataSize = arcGetSize(texhandle);

	vector3 v;

	spritenamestore * nextspritename = spritename;
int count=0, FileOffset=0;

	tempBuffer11 = strtok(SpriteData, "\n"); 

	if( texhandle ) 
	{
//		while(  arcGets( tempBuffer11, 128, texhandle ) )
		// StrTok() doesn`t work - presumably it's used within the loop.
				
		do
		{
			char * strPtr;
			char * cptr;
			char * nptr;
			int addon;
			
			tempBuffer11 = strtok(tempBuffer11, "\n");
			dprintf("%d %s", count++, tempBuffer11);
				
			addon = strlen(tempBuffer11)+1;
			cptr = tempBuffer11;

			char c = *cptr++;
					// read off leading white space
			while( (c == 32) || (c==9) )
					c = *cptr++;

			if(( c != '#') && ( c != 10 ) &&( c != 13 ) && (c!=0) )
			{
			
				strPtr = cptr;			// start of sprite name

				while( (c != 32) && (c!=9) && (c!=',') )
					c = *cptr++;

				cptr--;
				*cptr++ = 0;

				// strPtr = name of sprite, asciiz

				while( (c == 32) || (c==9) || (c==',') )
						c = *cptr++;
				nptr = cptr-1;
				while( (c != 32) && (c!=9) && (c!=',') )
						c = *cptr++;
				cptr--;
				*cptr++ = 0;

				v.x = (float)atof(nptr);
				
	
				while( (c == 32) || (c==9) || (c==',') )
						c = *cptr++;
				nptr = cptr-1;
				while( (c != 32) && (c!=9) && (c!=',') )
						c = *cptr++;
				cptr--;
				*cptr++ = 0;

				v.z = (float)atof(nptr);
	
	
				while( (c == 32) || (c==9) || (c==',') )
						c = *cptr++;
				nptr = cptr-1;
				while( (c != 32) && (c!=9) && (c!=',')  && (c!=0xd))
						c = *cptr++;
				cptr--;
				*cptr++ = 0;

				v.y = (float)atof(nptr) ;//.75f;
	

				// Is the spritename 'tempBuffer11' already encountered?

				spriteFX * thissfx;
				bool foundit = false;

				spritenamestore * firstspritename = spritename;
				while( firstspritename != nextspritename )
				{
					if( strcmp(firstspritename->name,tempBuffer11) == 0)	// found it...
					{
						thissfx = firstspritename->sfx;
						foundit = true;
						break;
					}
					firstspritename++;
				}
				if( !foundit)
				{
					strcpy(nextspritename->name,tempBuffer11);
					thissfx = nextspritename->sfx = new spriteFX(tempBuffer11);
					nextspritename++;
				}
				appendSpriteFX( thissfx, &(v * worldscale) , NULL, false, false);

			}

			FileOffset+=addon; tempBuffer11 = SpriteData+FileOffset;
//			FileOffset+=strlen(tempBuffer11)+1; tempBuffer11 = SpriteData+FileOffset;
//			FileOffset+=strlen(tempBuffer11)+1; tempBuffer11 = SpriteData+FileOffset;

		} while (FileOffset < SpriteDataSize); 
	}
}

void engine::wheelDirection( long wheelNumber, float angle )
{
	currentvertex2dmatlistPtr->textureHandle = NULL;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	float x0,y0;
	float deltax, deltay;

	deltax = 60;
	deltay = 60;

	x0 = 500;  
	y0 = 50;

	switch (wheelNumber)
	{
		case 1:
			break;
		case 0:
			x0 += deltax;  
			break;
		case 3:
			y0 += deltay;
			break;

		case 2:
			x0 += deltax;	
			y0 += deltay;
			break;
	}

	angle = -angle;

	angle += PI;

	float sinx = (float)sin( angle);
	float cosx = (float)cos(angle);
	float dx,dy;

	float tx =6;
	float ty =30;

	dx =  (-tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (-tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (0)  * cosx + (ty) * sinx;
	dy =  (ty)  * cosx - (0) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

}

void engine::showmagnitude( long num, float min, float max, float value )
{
	if(!current2dvertexListPtr)
		return;
	
	currentvertex2dmatlistPtr->textureHandle = NULL;//hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	if (value<min) 
		value = min;
	if (value>max) 
		value = max;
	
	float normval=(value-min)/(max-min);

	float x0,y0;
	float deltax, deltay;

	deltax = 60;
	deltay = 150;

	x0 = 500;  
	y0 = 250-normval*100.0f;

	float offx,offy;

	switch (num)
	{
		case 1:
			offx = 0.0f;  
			offy = 0.0f;
			break;
		case 0:
			offx = deltax;  
			offy = 0.0f;
			break;
		case 3:
			offx = 0.0;
			offy = deltay;
			break;

		case 2:
			offx = deltax;	
			offy = deltay;
			break;
	}

	x0+=offx;
	y0+=offy;
	float angle = -PI/2.0f;

	float sinx = (float)sin( angle);
	float cosx = (float)cos(angle);
	float dx,dy;

	float tx = 3.0f;
	float ty =20.0f;

	dx =  (-tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (-tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (0)  * cosx + (ty) * sinx;
	dy =  (ty)  * cosx - (0) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	angle = PI/2.0f;
	currentvertex2dmatlistPtr->textureHandle = NULL;//hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	sinx = (float)sin( angle);
	cosx = (float)cos(angle);

	tx =2.0f;
	ty =10.0f;

	x0 -= 20.0f;

	y0 = 150.0f+offy; 

	dx =  (-tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (-tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (0)  * cosx + (ty) * sinx;
	dy =  (ty)  * cosx - (0) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );
	currentvertex2dmatlistPtr->textureHandle = NULL;//hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	y0 = 250.0f+offy; 

	dx =  (-tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (-tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (0)  * cosx + (ty) * sinx;
	dy =  (ty)  * cosx - (0) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	if (min<0.0f && max>0.0f)
	{
		normval=(-min)/(max-min);

		y0 = 250-normval*100.0f;

		y0+=offy;
		currentvertex2dmatlistPtr->textureHandle = NULL;//hdl = (getTexturePtr(filename, false ))->textureHandle;
		currentvertex2dmatlistPtr->flags = 0;
		currentvertex2dmatlistPtr++;
		 

		dx =  (-tx)  * cosx + (0) * sinx;
		dy =  (0)  * cosx - (-tx) * sinx;
		add2dvertex( x0 + dx, y0 + dy, 0,0 );
		
		dx =  (tx)  * cosx + (0) * sinx;
		dy =  (0)  * cosx - (tx) * sinx;
	 	add2dvertex( x0 + dx, y0 + dy, 0,0 );

		dx =  (0)  * cosx + (ty) * sinx;
		dy =  (ty)  * cosx - (0) * sinx;
	 	add2dvertex( x0 + dx, y0 + dy, 0,0 );
	}
}

void engine::showmagdir( long num, float max, vector3 vect)
{
	if(!current2dvertexListPtr)
		return;

	float value=vect.Mag();
	if (value>max) 
		value = max;
	
	float normval=value/max;

	float vmag=vect.Mag();
	float ang;
	if (vmag<1.0f)
	{
		ang=0.0f;
	}
	else
	{
		ang=(float)acos(vect.z/vmag);
		if( vect.x < 0 ) 
			ang = -ang;
	}

	currentvertex2dmatlistPtr->textureHandle = NULL;//hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	float x0,y0;
	float deltax, deltay;

	deltax = 80;
	deltay = 80;

	x0 = 200;  
	y0 = 50;

	switch (num)
	{
		case 1:
			break;
		case 0:
			x0 += deltax;  
			break;
		case 3:
			y0 += deltay;
			break;

		case 2:
			x0 += deltax;	
			y0 += deltay;
			break;
	}

	ang = -ang;

	ang += PI;

	float sinx = (float)sin( ang);
	float cosx = (float)cos(ang);
	float dx,dy;

	float tx =3.0f;
	float ty =2.0f+40.0f*normval;

	dx =  (-tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (-tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (tx)  * cosx + (0) * sinx;
	dy =  (0)  * cosx - (tx) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );

	dx =  (0)  * cosx + (ty) * sinx;
	dy =  (ty)  * cosx - (0) * sinx;
	add2dvertex( x0 + dx, y0 + dy, 0,0 );
}


//
// checks to see if a pointer to a spriteFXentry is still in the engine's chain (ie. still exists ... for example
// when a sprite sequence finishes it will be removed!
//

bool engine::doesSpriteFXentryExist( spriteFXentry * e )
{
	if(e == NULL)
		return(false);
	spriteFXentry * thisfx = spriteFXlist;
	while( thisfx )
	{
		if( thisfx == e )
			return(true);
		thisfx = thisfx->next;
	}
	return(false);
}

void engine::DrawParticle(vector3 *v,DWORD col,float psize,material *mat)
{
	vector3 camcoord;
	vertex p,q,r;
	triangle t;

	float ps2 = psize * 0.5;

	currentcamera->transformWorldVector(&camcoord,v);	// got camera coordinates

	psize *= 10/(camcoord.z);

	if(psize > 0.03 && camcoord.z > ZFRONTPLANE && camcoord.z < 30)
	{
		if(psize<1) psize=1;
		if(psize>4) psize=4;

		p.transformed.x = camcoord.x;
		p.transformed.y = camcoord.y;
		p.transformed.z = camcoord.z;

		r = q = p;	// all the same point; tweaking is done in the pseudoprojection below.

		// project. The bracketed bits below are actually 'project' with tweaks; it's pretty
		// horrid. I'll tidy it up a bit later.

		projectedp.nv = &t.p.nv;
		{

			vector2P *v2 = p.projectedPtr = outNextVector2Pptr++;
			vector3 *v = &p.transformed;

			z2Dcalc(v2->z, v->z, v2->w);		
		
			float vw = RECIPROCAL( v->z );

			v2->x	= v->x * vw * projcoeffW + displayWd2deltaX;
			v2->y	= displayHd2deltaY - v->y * vw * projcoeffH;

			v2->v3 = *v;
			v2->l   = 1.0;
			v2->specularl = 0.0;

			v2->x -=ps2;	// coordinate tweak
			v2->y -=ps2;

			v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
			v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
		}
		projectedp.v = p.projectedPtr;

		projectedq.nv = &t.q.nv;
		{
			vector2P *v2 = q.projectedPtr = outNextVector2Pptr++;
			vector3 *v = &q.transformed;

			*v2 = *(p.projectedPtr);

			v2->x +=psize;	// coordinate tweak

			v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
			v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
		}
		projectedq.v = q.projectedPtr;
	
		projectedr.nv = &t.r.nv;
		{
			vector2P *v2 = r.projectedPtr = outNextVector2Pptr++;
			vector3 *v = &r.transformed;

			*v2 = *(p.projectedPtr);

			v2->x +=ps2;
			v2->y +=psize;

			v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
			v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

		}
		projectedr.v = r.projectedPtr;

		// set up the material

		memset(mat,0,sizeof(*mat));
		mat->ambient=col;
		mat->diffuse=col;
		mat->specular=col;

		currentmtl = mat;
		triPtr = &t;
		triPtr->mtl = currentmtl;

		// render (set mask to 0, triangle guaranteed to be between zplanes)

		mask=0;zsplitting();
	}

}


void engine::DrawLine(float x1,float y1,float x2,float y2,DWORD col)
{
	D3DTLVERTEX v[2];
	triangle t;

	v[1].color = v[0].color = col & 0xff000000;
	v[1].specular = v[0].specular = 0xffffffff;

	v[0].tu = v[1].tu = 0;
	v[0].tv = v[1].tv = 0;

	v[0].rhw = v[1].rhw = 0;

	v[0].sx = x1;	v[0].sy = y1;
	v[1].sx = x2;	v[1].sy = y2;

	renderLine(&(v[0]),2);
}

