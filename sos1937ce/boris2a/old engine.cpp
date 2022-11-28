//================================================================================================================================
// engine.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"

	//
	// CULLINGEPSILON is the amount needed to compensate for approximate trig. resulting in the transformed normal SEEMING to
	// face backwards when it really faces forwards, and hence culling is inacurate!!!
	//

#define CULLINGEPSILON 0.2f

#define ZFRONTPLANE 1.0f

//
// ttotal = number of triangles in OUTPUT lists
// vtotal = number of vertices in VERTEX lists
//

//extern debugger DBGR;


engine  * boris2;			// The global engine



__int64 timing_basecounter;
float   timing_oneOverFrequency;
__int64 timing_frequency;


#define BATCHSIZE 8000
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

//debugger DBGR;

engine::engine(  ulong eflags,  slong outListCount, slong ttotal, slong vtotal, slong ntotal, float physicsTicks)
{
	slong i;

//DBGR.Dprintf("Constructor!!!");

	initialiseArcHandles();

	outLists ** myolPtr;

	engine_flags      = eflags;

	currentdisplay    = NULL;
    currentcamera     = NULL;
    currentworld      = NULL;
	objectList		  = NULL;
    spriteFXlist	  = NULL; 

	lightSwitch = false;
	skyon = false;
	envon = false;
	shadon = false;
	wframe = false;
	fog = false;
	clip = false;

	zoomz = 0;
	zoomdone = true;

	//id3x4.setmat(  1.0f,    0,   0,
	//			   0   , 1.0f,   0,
	//			   0   ,    0, 1.0f,
	//			   0   ,    0,   0  );


	vertex2dlist	= new D3DTLVERTEX[ MAX2DENTRIES * 6 ];
	vertex2dmatList = new BorisMaterial[ MAX2DENTRIES * 3 ];

	randomScale =  (RECIPROCAL( ((float)(RAND_MAX))  )) ;		// for scaling random float values

			// Create a list of outLists

	olCount = outListCount;

	myolPtr = outListListPtr = new outLists *[ olCount ];

	for(i = olCount; i--; )
		(*myolPtr++) =  new outLists( ttotal, vtotal, ntotal );

	currentOutListPtr =  * outListListPtr;
//return;

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
}


engine::~engine()
{
	slong i;
	outLists ** myolPtr;

//DBGR.Dprintf("Deconstructor!!!");

			// remove outLists 

	myolPtr = outListListPtr;

	for( i = olCount; i--; )
		(*myolPtr++)->~outLists();
	
	delete[] outListListPtr;
	delete[] vertex2dlist;
	delete[] vertex2dmatList;

	renderDeInitialise();
}


void engine::shadowOn()
{
	shadon = true;
};

void engine::shadowOff()
{
	shadon = false;
};


void engine::wframeOn()
{
	wframe = true;
};

void engine::wframeOff()
{
	wframe = false;
};


void engine::setFogColour( ulong fcol )
{
	fogColour = fcol;

};

void engine::fogOn()
{
	renderFog(true);
	SetFogColor( fogColour );
	fog = true;
};

void engine::fogOff()
{
	renderFog(false);
	fog = false;
};

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
	};
};


bool engine::getZoomStatus()
{
	return(zoomdone);
};

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
};

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
	};
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
		
			mesh * meshData = &currentworld->worldModel->meshData;
		
			loHunkX = (slong)((xl - meshData->MinCoordX) * currentworld->oneoverxhunksize ) ;
			hiHunkX = (slong)((xh - meshData->MinCoordX) * currentworld->oneoverxhunksize ) + 1;

			loHunkZ = (slong)((zl - meshData->MinCoordZ) * currentworld->oneoverzhunksize) ;
			hiHunkZ = (slong)((zh - meshData->MinCoordZ) * currentworld->oneoverzhunksize) + 1;
		
			if( hiHunkX >= currentworld->xhunks) hiHunkX = currentworld->xhunks-1;
			if( hiHunkZ >= currentworld->zhunks) hiHunkZ = currentworld->zhunks-1;

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

							vector3 * l = &triPtr->p.v->geoCoord;
							vector3 * m = &triPtr->q.v->geoCoord;
							vector3 * n = &triPtr->r.v->geoCoord;

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

								//minmax3( l->x, m->x, n->x, &xl, &xh );																								
								//minmax3( l->y, m->y, n->y, &yl, &yh );																								
								//minmax3( l->z, m->z, n->z, &zl, &zh );																								
							
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

										float rsqr = (dx * triPtr->A + 
												  dy * triPtr->B + 	
											      dz * triPtr->C +
												  triPtr->planeD
												 );

										rsqr *= rsqr * triPtr->ABC;

										if (rsqr < dsqr * 0.2)
										{

// TEMP.
// Find closest corner of bounding box around object and return a pointer to the co-ordinate
//

float mindistance = 99999999.0f;
ulong wnum;


		 rsqr = ( bboxPtr->v000.x * triPtr->A + 
				  bboxPtr->v000.y * triPtr->B + 	
				  bboxPtr->v000.z * triPtr->C + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
				mindistance = rsqr;
				wnum = 0;
				//bestcoord = &bboxPtr->v000;
		 };
		 rsqr = ( bboxPtr->v001.x * triPtr->A + 
				  bboxPtr->v001.y * triPtr->B + 	
				  bboxPtr->v001.z * triPtr->C + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
 				mindistance = rsqr;
				 wnum = 2;
				//bestcoord = &bboxPtr->v001;
		 };
	
		 rsqr = ( bboxPtr->v100.x * triPtr->A + 
				  bboxPtr->v100.y * triPtr->B + 	
				  bboxPtr->v100.z * triPtr->C + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
				mindistance = rsqr;
				wnum = 1;
				//bestcoord = &bboxPtr->v100;
		 };
		 rsqr = ( bboxPtr->v101.x * triPtr->A + 
				  bboxPtr->v101.y * triPtr->B + 	
				  bboxPtr->v101.z * triPtr->C + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
				mindistance = rsqr;
				wnum = 3;
				//bestcoord = &bboxPtr->v101;
		 };


										callback( nextCollisionObject, wnum);															
										cont = false;
										}

									}

							};

							tlistPtr++;
						};

						hz += currentworld->zhunksize;
						hunkPtrZ++;
				}

				hx += currentworld->xhunksize;
				hunkPtrX += currentworld->zhunks;
			};


		

		};
		nextCollisionObject = nextCollisionObject->next;
	}
}

//
//   F( 1 - D / z  ) / ( F - D )
//

// 0.1f

#define ZSCALE  0.1f
#define Z2DCALC(zz, ww)  (( ( 1.0f - (ww = RECIPROCAL( zz*ZSCALE + (1.0f - ZSCALE) ))) ) * zNormalizer ); 

//================================================================================================================================
// produce a projected vertex from the 3d co-oridnate vertex.
//		- automatically produces the SutherlandCohen mask against the current display since this will always be used.
//
//================================================================================================================================

__inline void engine::project( vector2P * v2,  vector3 * v, float l )
{

	v2->z	= Z2DCALC(v->z, v2->w);		
	
	#define vw  RECIPROCAL( v->z )

	v2->x	= v->x * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - v->y * vw * projcoeffH;


	//if( v2->z > 1.0f ) v2->z = 1.0f;

	v2->v3 = *v;
	v2->l   = l;

	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

};

__inline void engine::project( vector2P * v2,  float vx, float vy, float vz, float l )
{

	v2->z   = Z2DCALC(vz, v2->w);

	#define vw  RECIPROCAL( vz )

	v2->x	= vx * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - vy * vw * projcoeffH;

	//if( v2->z > 1.0f ) v2->z = 1.0f;


	v2->v3.x = vx;
	v2->v3.y = vy;
	v2->v3.z = vz;

	v2->l = l;

	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
};

//----
//
//----

__inline void engine::zLimitedproject( vector2P * v2,  vector3 * v, float l )
{

	v2->z = Z2DCALC(v->z, v2->w);				

	#define vw  RECIPROCAL( v->z )

	v2->x	= v->x * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - v->y * vw * projcoeffH;

	//if( v->z > currentcamera->zbackplane )
	//{
	//	v2->z = Z2DCALC( currentcamera->zbackplane, v2->w);		
	//};
		

	v2->v3 = *v;
	v2->l   = l;
														
	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

};


__inline void engine::zLimitedproject( vector2P * v2,  float vx, float vy, float vz, float l )
{

	v2->z = Z2DCALC(vz, v2->w);

	#define vw  RECIPROCAL( vz )

	v2->x	= vx * vw * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - vy * vw * projcoeffH;

	//if ( vz > currentcamera->zbackplane)
	//{
	//	 v2->z = Z2DCALC( currentcamera->zbackplane, v2->w);
	//}

	v2->v3.x = vx;
	v2->v3.y = vy;
	v2->v3.z = vz;


	v2->l = l;
		
	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));
};

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
	zNormalizer =  MAXIMUMREARZ * zNormalizer1 ;//* 4.0f;  
	zNormalizer1 *= 2730.0f ;//* 0.25f;
#else
	zNormalizer1 = RECIPROCAL( currentcamera->zbackplane -  ZFRONTPLANE) ;
	zNormalizer =  currentcamera->zbackplane * zNormalizer1 ;//* 4.0f; 
	zNormalizer1 *= 2730.0f ;//* 0.25f;
#endif

	if( engine_flags & EFLAGS_SOFTWARE )			// change range from [0,1) to [0,,,SOFTWARE_ZMAX)
	{
	    zNormalizer *= SOFTWARE_ZMAX;		// the maximum sofware z-buf. range
	};

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


//================================================================================================================================
//setCurrentDisplayPosition
//-------------------------
//
//		set the position of the current display window (top left corner)
//================================================================================================================================
/*
void engine::setCurrentDisplayPosition( slong posx, slong posy )
{
	currentdisplay->positionx = posx;
	currentdisplay->positiony = posy;

	//displayDeltaX = (float)( currentdisplay->deltaX + posx);
	//displayDeltaY = (float)( currentdisplay->deltaY + posy);

	//displayHM1deltaY = displayHM1 + currentdisplay->deltaY ;
	//displayWM1deltaX = displayWM1 + currentdisplay->deltaX;

	//displayHd2deltaY = displayHd2 + currentdisplay->deltaY;
	//displayWd2deltaX = displayWd2 + currentdisplay->deltaX;

	//updateDCdata();


};
*/

//================================================================================================================================
// setCurrentWorld
// ----------------
//		- define the world being used by the engine.
//================================================================================================================================

void engine::setCurrentWorld( world * w )
{
	currentworld = w;
}





//================================================================================================================================
// unsetCurrentCamera
// ----------------
//		disconnect current camera
//================================================================================================================================

void engine::unsetCurrentCamera()
{
	currentcamera = 0;
}

//================================================================================================================================
// unsetCurrentDisplay
// -----------------
//		disconnect current display
//================================================================================================================================

void engine::unsetCurrentDisplay()
{
	currentdisplay = 0;
}

//================================================================================================================================
// unsetCurrentWorld
// ----------------
//		disconnect current world
//================================================================================================================================

void engine::unsetCurrentWorld()
{
	currentworld = 0;
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
								  a->v->l + (float)(t * ( b->v->l - a->v->l )));

			// now split non-goemetric data to create  r->nv...  (we must create a new entry of nongeodata)

	outNextNonGeoptr->u = a->nv.u + (float)(t * ( b->nv.u - a->nv.u ));
	outNextNonGeoptr->v = a->nv.v + (float)(t * ( b->nv.v - a->nv.v ));

   	r->v = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
	if( envmapping )
	{
		getEnvmapUVs( &r->nv1, a->v);	 	// TEMPORARILY - CHANGE TO MID VALUE WHEN WORKING
	};
};

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
								  a->v->l + (float)(t * ( b->v->l - a->v->l )));

			// now split non-goemetric data to create  r->nv...  (we must create a new entry of nongeodata)

	outNextNonGeoptr->u = a->nv.u + (float)(t * ( b->nv.u - a->nv.u ));
	outNextNonGeoptr->v = a->nv.v + (float)(t * ( b->nv.v - a->nv.v ));

	r->v = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
};
//================================================================================================================================
// When an edge needs splitting againsty two Z values this is more efficient than two calls
// to 'splitEdgeZ'
//================================================================================================================================

void engine::doubleSplitEdgeZ( pointdata2d * r0,  pointdata2d * r1,
							   pointdata3d * a, pointdata3d * b, 
							   float splitz0, float splitz1)
{

	splitEdgeZ( r0, a, b, splitz0 );
	splitEdgeZ( r1, a, b, splitz1 );
return;


	float t, r, xd, yd, ud, vd, ld;
	
	vector3 * avg = &a->v->transformed;
	vector3 * bvg = &b->v->transformed;

			// calculate parametric co-efficient of line at split (and set z at split )

	r = RECIPROCAL( bvg->z - avg->z );


	xd = ( bvg->x - avg->x );
	yd = ( bvg->y - avg->y );
	ud =   b->nv.u - a->nv.u;
	vd =   b->nv.v - a->nv.v;
	ld =   b->v->l - a->v->l;

	t  = (float)(( splitz0 - avg->z) * r);
	outNextNonGeoptr->u = a->nv.u + (float)(t * ud);
	outNextNonGeoptr->v = a->nv.v + (float)(t * vd);
	project( outNextVector2Pptr, avg->x + (float)(t * xd), avg->y + (float)(t * yd), splitz0, a->v->l + (float)(t * ld));
	r0->nv = outNextNonGeoptr++;
	r0->v = outNextVector2Pptr++;

	t  = (float)(( splitz1 - avg->z) * r);
	outNextNonGeoptr->u = a->nv.u + (float)(t * ud);
	outNextNonGeoptr->v = a->nv.v + (float)(t * vd);
	project( outNextVector2Pptr, avg->x + (float)(t * xd), avg->y + (float)(t * yd), splitz1, a->v->l + (float)(t * ld));
	r1->nv = outNextNonGeoptr++;
	r1->v = outNextVector2Pptr++;

    if( envmapping )
	{
		getEnvmapUVs(&r0->nv1, a->v);		// TEMPORARILY - CHANGE TO MID VALUE WHEN WORKING
		getEnvmapUVs(&r1->nv1, b->v);		// TEMPORARILY - CHANGE TO MID VALUE WHEN WORKING
	};

};

			
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
//outNextVector2Pptr->w =  av->w + t * ( bv->w - av->w );

			//
			// calculate the 3d parametric co-efficient
			// and split 3d values  (z, u,v,l )
			//		( we must maintain the 3d x,y values because we shall split on Y next
			//

	float Rw = (float)(projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y ));
	float Rh = (float)(projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX ));

	t = (float)(( Rh * av->v3.y - Rw * av->v3.x )  * RECIPROCAL( (bv->v3.x - av->v3.x) * Rw - (bv->v3.y - av->v3.y) * Rh ));

//t = 1 - outNextVector2Pptr->w;

	outNextVector2Pptr->v3.x = av->v3.x + (float)(t * ( bv->v3.x - av->v3.x ));	
	outNextVector2Pptr->v3.y = av->v3.y + (float)(t * ( bv->v3.y - av->v3.y ));	
	outNextVector2Pptr->v3.z = av->v3.z + (float)(t * ( bv->v3.z - av->v3.z ));	
		
//	outNextVector2Pptr->w =   RECIPROCAL( outNextVector2Pptr->v3.z );
	outNextVector2Pptr->z =   Z2DCALC(outNextVector2Pptr->v3.z, outNextVector2Pptr->w ) ;	
//	if( outNextVector2Pptr->z > 1.0f ) outNextVector2Pptr->z = 1.0f;

	//outNextVector2Pptr->z = av->z + t * ( bv->z - av->z );	
	//outNextVector2Pptr->w =   RECIPROCAL( outNextVector2Pptr->z );			// TEMP .. change to sim. to above

	outNextNonGeoptr->u = anv->u + (float)(t * ( bnv->u - anv->u ));
	outNextNonGeoptr->v = anv->v + (float)(t * ( bnv->v - anv->v ));
	outNextVector2Pptr->l = av->l + (float)(t * ( bv->l - av->l ));

		
			// We actually DO need to calculate the partial SC mask in Y since Y-clipping is done AFTER x-clipping
			// (we dont need to do the X mask)

	outNextVector2Pptr->SCmaskY = (( outNextVector2Pptr->y < displayDeltaY ) ? 1 : ( (outNextVector2Pptr->y > displayHM1deltaY) ? 2 : 0 ));

	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
    if( envmapping )
	{
		rnv1 = &r->nv1;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		rnv1->u =  anv1->u + (float)(t * ( bnv1->u - anv1->u ));
		rnv1->v =  anv1->v + (float)(t * ( bnv1->v - anv1->v ));
	};
};


//================================================================================================================================
// When an edge (2D) needs splitting againsty two X values this is more efficient than two calls
// to 'splitEdgeX2D'
//================================================================================================================================

void engine::doubleSplitEdgeX2D(  pointdata2d * r0,  pointdata2d * r1,
								  pointdata2d * a, pointdata2d * b, 
								  float splitx0, float splitx1 )
{
			
splitEdgeX2D( r0, a, b, splitx0);
splitEdgeX2D( r1, a, b ,splitx1);
return;

	float t0, t1, r, ud, vd, ld, yd, zd, wd;
	nongeodata * r0nv1, * r1nv1,  * anv1, * bnv1;
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;
			
	vector2P    * outNextVector2Pptr1 = outNextVector2Pptr + 1;
	nongeodata  * outNextNonGeoptr1   = outNextNonGeoptr + 1;

	r = RECIPROCAL( bv->x - av->x );

	t0 = (  (outNextVector2Pptr->x = splitx0) - av->x) * r;		// parametric coeff at splitx0
	t1 = (  (outNextVector2Pptr1->x = splitx1) - av->x) * r;	// parametric coeff at splitx1


	t0 = 

	yd = bv->y - av->y; 
	ud = bnv->u - anv->u;
	vd = bnv->v - anv->v;
	ld = bv->l - av->l;

	zd = bv->z - av->z;
	wd = bv->w - av->w;

	outNextVector2Pptr->y  =  av->y + t0 * yd;
	outNextVector2Pptr1->y =  av->y + t1 * yd;
//	outNextVector2Pptr->w  =  av->w + t0 * wd;
//	outNextVector2Pptr1->w =  av->w + t1 * wd;

	float bax, bay, baz, bngu, bngv, bngl, Rw, Rh;

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );
	t0 = ( Rh * av->v3.y - Rw * av->v3.x )  * RECIPROCAL( (bax = bv->v3.x - av->v3.x) * Rw - (bay = bv->v3.y - av->v3.y) * Rh );

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr1->y );
	Rh = projcoeffH*( outNextVector2Pptr1->x - displayWd2deltaX );
	t1 = ( Rh * av->v3.y - Rw * av->v3.x )  * RECIPROCAL( bax * Rw - bay * Rh );


	outNextVector2Pptr->v3.x = av->v3.x + t0 * ( bax = bv->v3.x - av->v3.x );	
	outNextVector2Pptr->v3.y = av->v3.y + t0 * ( bay = bv->v3.y - av->v3.y );	
	outNextVector2Pptr->v3.z = av->v3.z + t0 * ( baz = bv->v3.z - av->v3.z );	

	outNextVector2Pptr->z =   Z2DCALC(outNextVector2Pptr->v3.z, outNextVector2Pptr->w);	
 
	outNextNonGeoptr->u = anv->u + t0 * ( bngu = bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t0 * ( bngv = bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t0 * ( bngl = bv->l - av->l );

	outNextVector2Pptr1->v3.x = av->v3.x + t1 * bax;	
	outNextVector2Pptr1->v3.y = av->v3.y + t1 * bay;
	outNextVector2Pptr1->v3.z = av->v3.z + t1 * baz;	

    outNextVector2Pptr1->z =   Z2DCALC(outNextVector2Pptr1->v3.z, outNextVector2Pptr1->w);	
 
	outNextNonGeoptr1->u = anv->u + t1 * bngu;
	outNextNonGeoptr1->v = anv->v + t1 * bngv;
	outNextVector2Pptr1->l = av->l + t1 * bngl;


			// We actually DO need to calculate the partial SC mask in Y since Y-clipping is done AFTER x-clipping
			// (we dont need to do the X mask)

	outNextVector2Pptr->SCmaskY = (( outNextVector2Pptr->y < displayDeltaY ) ? 1 : ( (outNextVector2Pptr->y > displayHM1deltaY) ? 2 : 0 ));
	outNextVector2Pptr1->SCmaskY = (( outNextVector2Pptr1->y < displayDeltaY ) ? 1 : ( (outNextVector2Pptr1->y > displayHM1deltaY) ? 2 : 0 ));

	r0->v  = outNextVector2Pptr;
	r0->nv = outNextNonGeoptr;
	r1->v  = outNextVector2Pptr1;
	r1->nv = outNextNonGeoptr1;

	outNextVector2Pptr += 2;
	outNextNonGeoptr += 2;


	if( envmapping )
	{
		r0nv1 = &r0->nv1;
		r1nv1 = &r1->nv1;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		r0nv1->u =  anv1->u + (float)(t0 * ( bnv1->u - anv1->u ));
		r0nv1->v =  anv1->v + (float)(t0 * ( bnv1->v - anv1->v ));
		r1nv1->u =  anv1->u + (float)(t1 * ( bnv1->u - anv1->u ));
		r1nv1->v =  anv1->v + (float)(t1 * ( bnv1->v - anv1->v ));
	};
};





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
//outNextVector2Pptr->w = av->w + t * ( bv->w - av->w );


			//
			// calculate the 3d parametric co-efficient
			// and split 3d values  (z, u,v,l )
			//		( we must maintain the 3d x,y values because we shall split on Y next
			//

	float Rw = (float)(projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y ));
	float Rh = (float)(projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX ));

	t = (float)(( Rh * av->v3.y - Rw * av->v3.x ) * RECIPROCAL( (bv->v3.x - av->v3.x) * Rw - (bv->v3.y - av->v3.y) * Rh ));
//t = 1 - outNextVector2Pptr->w;

	outNextVector2Pptr->v3.x = av->v3.x + (float)(t * ( bv->v3.x - av->v3.x ));	
	outNextVector2Pptr->v3.y = av->v3.y + (float)(t * ( bv->v3.y - av->v3.y ));	
	outNextVector2Pptr->v3.z = av->v3.z + (float)(t * ( bv->v3.z - av->v3.z ));	
		
//	outNextVector2Pptr->w =   (float)(RECIPROCAL( outNextVector2Pptr->v3.z ));
	outNextVector2Pptr->z = Z2DCALC(outNextVector2Pptr->v3.z, outNextVector2Pptr->w );	
	//if( outNextVector2Pptr->z > 1.0f ) outNextVector2Pptr->z = 1.0f;

//	outNextVector2Pptr->z = av->z + t * ( bv->z - av->z );	

	outNextNonGeoptr->u = anv->u + (float)(t * ( bnv->u - anv->u ));
	outNextNonGeoptr->v = anv->v + (float)(t * ( bnv->v - anv->v ));
	outNextVector2Pptr->l = av->l + (float)(t * ( bv->l - av->l ));


			// NOTE that we have no need for setting local SCmaskX or SCmaskY here..


	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;

	if( envmapping )
	{
		rnv1 = &r->nv1;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		rnv1->u =  anv1->u + (float)(t * ( bnv1->u - anv1->u ));
		rnv1->v =  anv1->v + (float)(t * ( bnv1->v - anv1->v ));
	};

}


//================================================================================================================================
// When an edge (2D) needs splitting againsty two Y values this is more efficient than two calls
// to 'splitEdgeY2D'
//================================================================================================================================

void engine::doubleSplitEdgeY2D(  pointdata2d * r0,  pointdata2d * r1,
								  pointdata2d * a, pointdata2d * b, 
								  float splity0, float splity1 )
{
			
splitEdgeY2D( r0, a, b, splity0);
splitEdgeY2D( r1, a, b ,splity1);
return;


	float t0, t1, r, ud, vd, ld, xd, zd, wd;
	nongeodata * r0nv1, * r1nv1,  * anv1, * bnv1;
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;
			
	vector2P    * outNextVector2Pptr1 = outNextVector2Pptr + 1;
	nongeodata  * outNextNonGeoptr1   = outNextNonGeoptr + 1;

	r = RECIPROCAL( bv->y - av->y );

	t0 = (  (outNextVector2Pptr->y = splity0) - av->y) * r;		// parametric coeff at splity0
	t1 = (  (outNextVector2Pptr1->y = splity1) - av->y) * r;	// parametric coeff at splity1

	//TRIMOFF(t0);
	//TRIMOFF(t1);

	zd = bv->z - av->z;  
	wd = bv->w - av->w;  

	xd = bv->x - av->x;  
	ud = bnv->u - anv->u;
	vd = bnv->v - anv->v;
	ld = bv->l - av->l;


	outNextVector2Pptr->x  = av->x + t0 * xd;
	outNextVector2Pptr1->x = av->x + t1 * xd;
//	outNextVector2Pptr->w  = av->w + t0 * wd;
//	outNextVector2Pptr1->w = av->w + t1 * wd;

	float bax, bay, baz, bngu, bngv, bngl, Rw, Rh;

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );
	t0 = ( Rh * av->v3.y - Rw * av->v3.x )  * RECIPROCAL( (bax = bv->v3.x - av->v3.x) * Rw - (bay = bv->v3.y - av->v3.y) * Rh );

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr1->y );
	Rh = projcoeffH*( outNextVector2Pptr1->x - displayWd2deltaX );
	t1 = ( Rh * av->v3.y - Rw * av->v3.x )  * RECIPROCAL( bax * Rw - bay * Rh );


	outNextVector2Pptr->v3.x = av->v3.x + t0 * ( bax = bv->v3.x - av->v3.x );	
	outNextVector2Pptr->v3.y = av->v3.y + t0 * ( bay = bv->v3.y - av->v3.y );	
	outNextVector2Pptr->v3.z = av->v3.z + t0 * ( baz = bv->v3.z - av->v3.z );	

	outNextVector2Pptr->z =   Z2DCALC(outNextVector2Pptr->v3.z,  outNextVector2Pptr->w);	
    //outNextVector2Pptr->w =   RECIPROCAL( outNextVector2Pptr->v3.z );			// TEMP .. change to sim. to above

//	outNextVector2Pptr->z = av->z + t0 * ( baz = bv->z - av->z );	

	outNextNonGeoptr->u = anv->u + t0 * ( bngu = bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t0 * ( bngv = bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t0 * ( bngl = bv->l - av->l );

	outNextVector2Pptr1->v3.x = av->v3.x + t1 * bax;	
	outNextVector2Pptr1->v3.y = av->v3.y + t1 * bay;	
	outNextVector2Pptr1->v3.z = av->v3.z + t1 * baz;	
	
	outNextVector2Pptr1->z =   Z2DCALC(outNextVector2Pptr1->v3.z, outNextVector2Pptr1->w);	
    //outNextVector2Pptr1->w =   RECIPROCAL( outNextVector2Pptr1->v3.z );			// TEMP .. change to sim. to above

//	outNextVector2Pptr1->z = av->z + t1 * baz;	

	outNextNonGeoptr1->u = anv->u + t1 * bngu;
	outNextNonGeoptr1->v = anv->v + t1 * bngv;
	outNextVector2Pptr1->l = av->l + t1 * bngl;

			//
			//  Since we will have already split against X and no further splitting occurs
			//  with the new points, no new SCmaskX needs calculating!!
			//

	r0->v  = outNextVector2Pptr;
	r0->nv = outNextNonGeoptr;
	r1->v  = outNextVector2Pptr1;
	r1->nv = outNextNonGeoptr1;

	outNextVector2Pptr += 2;
	outNextNonGeoptr += 2;


	if( envmapping )
	{
		r0nv1 = &r0->nv1 ;//= outNextNonGeoptr++;
		r1nv1 = &r1->nv1 ;//= outNextNonGeoptr++;
		anv1 = &a->nv1;
		bnv1 = &b->nv1;
		r0nv1->u =  anv1->u + (float)(t0 * ( bnv1->u - anv1->u ));
		r0nv1->v =  anv1->v + (float)(t0 * ( bnv1->v - anv1->v ));
		r1nv1->u =  anv1->u + (float)(t1 * ( bnv1->u - anv1->u ));
		r1nv1->v =  anv1->v + (float)(t1 * ( bnv1->v - anv1->v ));
	};	

};


__inline bool windingOrder( vector2P *p, vector2P *q, vector2P * r)
{
	return( ((q->x - p->x) * (r->y - p->y))  >=  ((r->x - p->x) * (q->y - p->y)) );
};

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


						//renderTriangle(&testTriangle);
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

	};

};



/*
#define wSZ4_A(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, ZFRONTPLANE, currentcamera->zbackplane);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, currentcamera->zbackplane, ZFRONTPLANE);\
	Wprocess2DTriangle( &split2, &split3, &split0);\
	Wprocess2DTriangle( &split0, &split1, &split2);
#define wSZ4_B(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, currentcamera->zbackplane, ZFRONTPLANE);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, ZFRONTPLANE, currentcamera->zbackplane);\
	Wprocess2DTriangle( &split2, &split3, &split0);\
	Wprocess2DTriangle( &split0, &split1, &split2);
*/



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
	if (windingOrder( (p)->v, (q)->v, (r)->v )) \
		process2DTriangle( p , q, r ); \



__inline void engine::zsplitting()
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
	} else
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
};



__inline void engine::Skyzsplitting()
{

	static   pointdata2d split0, split1, split2, split3;

					switch( mask ) // (clip)?0:mask)
					{
						case 0:			// 000000 = completely between z-planes: NO SPLIT
								process2DTriangle(&projectedp, &projectedq, &projectedr);
		
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

};

//================================================================================================================================
// getEnvmapUVs
// -------------
//		calculate the environment map UVs for this 2d point
//================================================================================================================================

void engine::getEnvmapUVs(nongeodata * nv1, vertex * v)
{

	static vector3 r;

//	r = v->transformedvertexNormal + currentcamera->direction;


float dp = v->transformedvertexNormal.DotProduct(&currentcamera->direction);
r.x = v->transformedvertexNormal.x - dp*currentcamera->direction.x;
r.y = v->transformedvertexNormal.y - dp*currentcamera->direction.y;
r.z = v->transformedvertexNormal.z - dp*currentcamera->direction.z;

	nv1->u = ( 2.0f + r.x ) * 0.25f;
	nv1->v = ( 2.0f + r.y ) * 0.25f;


};


void engine::getEnvmapUVs(nongeodata * nv1, vertex * v, vector3 * p)
{

	static vector3 r;

	r = v->transformedvertexNormal + currentcamera->direction;


	nv1->u = ( 1.0f + r.x ) * 0.25f;
	nv1->v = ( 1.0f + r.y ) * 0.25f;


	nv1->u += p->x * 0.0003f;
	nv1->v += p->z * 0.0003f;

};

//================================================================================================================================
// renderSky
// ---------
//
//		render the current sky dome.
//================================================================================================================================



//debugger DBGR;
void engine::renderSky()
{

	static matrix3x4 WCtransform;
	ulong t;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;

	mesh * meshData = &currentSky->skyModel->meshData;

	WCtransform.multiply3x3submats( &currentcamera->RotMatrix, &currentSky->RotMatrix );
	triPtr = meshData->tlistPtr;

	globalpctr++;

//DBGR.Dprintf("rendering sky");
//DBGR.Dprintf("count = %f",(float)currentSky->meshData.tcount);

	envmapping = false;				// NO ENVIRONMENT MAPPING ON SKY!!!

	for( t = meshData->tcount; t--; )
	{

			currentmtl = triPtr->mtl;
			//	envmapping = (( currentmtl.flags & MAT_ENVIRONMENT ) != 0);

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

pvPtr->geoCoord.y -= SUBTEND;
				WCtransform.multiplyV3By3x3mat( &pvPtr->transformed, &pvPtr->geoCoord);
pvPtr->geoCoord.y += SUBTEND;
				pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
			};
			if( qvPtr->tcnt != globalpctr )	
			{
				qvPtr->tcnt = globalpctr;
qvPtr->geoCoord.y -= SUBTEND;
				WCtransform.multiplyV3By3x3mat( &qvPtr->transformed, &qvPtr->geoCoord );
qvPtr->geoCoord.y += SUBTEND;
				qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
			};
			if( rvPtr->tcnt != globalpctr )		
			{
				rvPtr->tcnt = globalpctr;
rvPtr->geoCoord.y -= SUBTEND;
				WCtransform.multiplyV3By3x3mat( &rvPtr->transformed, &rvPtr->geoCoord);
rvPtr->geoCoord.y += SUBTEND;
				rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : 0;//((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
			};

 
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
						zLimitedproject( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l );
				  };
				  projectedp.v = pvPtr->projectedPtr;		// use already projected value
			};
			
			if( (mask & 12) == 0 )
			{
				  projectedq.nv = &triPtr->q.nv;
				  if( qvPtr->pcnt != globalpctr )		// not yet projected 
				  {
						qvPtr->pcnt = globalpctr;
						zLimitedproject( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l  );
				  };
				  projectedq.v = qvPtr->projectedPtr;
			};

			if( (mask & 3) == 0 )
			{
				   projectedr.nv = &triPtr->r.nv;
				   if( rvPtr->pcnt != globalpctr )		// not yet projected 
				   {	
						rvPtr->pcnt = globalpctr;
						zLimitedproject( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
				   };
				   projectedr.v = rvPtr->projectedPtr;
			};
			
				// temp. stop lighting if on

			pvPtr->l = 
			qvPtr->l = 
			rvPtr->l = 1.0f;

float zbp = currentcamera->zbackplane;
currentcamera->zbackplane = 9999999.0f;
			Skyzsplitting();
currentcamera->zbackplane = zbp;
			triPtr++;				// next triangle in sky
	
	};



};


//void engine::processTriangle( triangle * triPtr )

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

					if( !( potentiallyVisible = (currentmtl->bm.flags & MAT_DOUBLESIDED )) )
					{

						potentiallyVisible = ( (triPtr->faceNormal.x * currentcamera->RotMatrix.E20 + 
											    triPtr->faceNormal.y * currentcamera->RotMatrix.E21 + 
									             triPtr->faceNormal.z * currentcamera->RotMatrix.E22 )<= CULLINGEPSILON ) ;
				
					};						 
if( potentiallyVisible )
{

				
					envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
					

						//
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//

					pvPtr = triPtr->p.v;
					if( pvPtr->tcnt != globalpctr )		// not yet transformed 
					{
						pvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &pvPtr->transformed, &pvPtr->geoCoord );
pvPtr->transformed.z -= zoomz;
					    if( envon )
							getEnvmapUVs( &pvPtr->nv1, pvPtr);
						pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					};

					qvPtr = triPtr->q.v;
					if( qvPtr->tcnt != globalpctr )	
					{
						qvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &qvPtr->transformed, &qvPtr->geoCoord );
qvPtr->transformed.z -= zoomz;
					    if( envon )
							getEnvmapUVs( &qvPtr->nv1, qvPtr);
						qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					};

					rvPtr = triPtr->r.v;
					if( rvPtr->tcnt != globalpctr )		
					{
						rvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &rvPtr->transformed, &rvPtr->geoCoord );
rvPtr->transformed.z -= zoomz;
					    if( envon )
							getEnvmapUVs( &rvPtr->nv1, rvPtr);
						rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					};

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
							project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l );
					  };
					  projectedp.v = pvPtr->projectedPtr;		// use already projected value
					};
			
					if( (mask & 12) == 0 )
					{
					  projectedq.nv = &triPtr->q.nv;
					  projectedq.nv1 = qvPtr->nv1;

					  
					  if( qvPtr->pcnt != globalpctr )		// not yet projected 
					  {
							qvPtr->pcnt = globalpctr;
							project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l  );
					  };
					  projectedq.v = qvPtr->projectedPtr;
					};

					if( (mask & 3) == 0 )
					{
					   projectedr.nv = &triPtr->r.nv;
					   projectedr.nv1 = rvPtr->nv1;

					   if( rvPtr->pcnt != globalpctr )		// not yet projected 
					   {	
							rvPtr->pcnt = globalpctr;
							project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
					   };
					   projectedr.v = rvPtr->projectedPtr;
					};
				
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// According to which front/back planes are crossed (if any) by this triangle, split if necessary
						// (creating new 2D vertices) and process resulting 2D triangles
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - - 

polygoncounter++;
					zsplitting();
}

					triPtr++;

				}
		} 



};



//================================================================================================================================
// Macro adapted from ::project for rejection of 3d coords against 2D volume where we DONT need the actual projected coord.
//
//						Note that we need no divisions for this code
//
// THIS MACRO IS EXPLICIT TO THE ONE ROUTINE, ITS PARAMETERS WILL ALWAYS BE EXPRESSIONS SO WE HAVE 'l3 = z3'
//================================================================================================================================

//	if (( lz >= ZFRONTPLANE ) && ( lz <= currentcamera->zbackplane) )  

#define OFFSCREEN( x3, y3, z3 )  \
	lz = z3;\
	{  \
		px = displayWd2divprojcoeffW*lz; \
		py = displayHd2divprojcoeffH*lz; \
		mask &= ( \
				(( x3 < -px ) ? 1 : ( (x3 > px) ? 2 : 0 )) |\
				(( y3 < -py ) ? 4 : ( (y3 > py) ? 8 : 0 )) |\
				(( lz < ZFRONTPLANE ) ? 16 : ( (lz > currentcamera->zbackplane) ? 32 : 0 )) \
			    ); 		 \
	}; 


//----------------------------------------------------------------------------------------------------------------------------
// processObjectLit
// ----------------
//			RECURSIVE processing of the object tree  (LIGHTING ON)
//----------------------------------------------------------------------------------------------------------------------------

//ulong bumf;


void engine::setCurrentSky( sky * s )
{
	if( (currentSky = s) != NULL )
		skyon = true;
};



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
	} else
	{
			catMat = &myObject->concatenatedTransform;
			catMat->translation -= currentcamera->position;
			WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
			catMat->translation += currentcamera->position;
	};


			if( myObject->modelData != 0)		// ie not a NULL model 
			{

						// check bounding sphere is on screen

				//tempvectorN = myObject->transformedCentroid - currentcamera->position;  
				//currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );

				//float Hi = result.z + myObject->modelData->boundingRadius;
				//float Lo = result.z - myObject->modelData->boundingRadius;
				//float pp;

				//bool ok = false;

				//if ( (
				//	(( Lo < ZFRONTPLANE ) ? 1 : (( Lo > currentcamera->zbackplane ) ? 2 : 0)) &
				//	(( Hi < ZFRONTPLANE ) ? 1 : (( Hi > currentcamera->zbackplane ) ? 2 : 0))
				//	 ) == 0 )
				//{
				//	pp = displayWd2divprojcoeffW * result.z; 
				//	Hi = result.x + myObject->modelData->boundingRadius;
				//	Lo = result.x - myObject->modelData->boundingRadius;
				//	if ( (
				//		(( Lo < -pp ) ? 1 : (( Lo > pp ) ? 2 : 0)) &
				//		(( Hi < -pp ) ? 1 : (( Hi > pp ) ? 2 : 0))
				//		 ) == 0)
				//	{
				//		pp = displayHd2divprojcoeffH * result.z; 
				//		Hi = result.y + myObject->modelData->boundingRadius;
				//		Lo = result.y - myObject->modelData->boundingRadius;
				//		if ( (
				//			(( Lo < -pp ) ? 1 : (( Lo > pp ) ? 2 : 0)) &
				//			(( Hi < -pp ) ? 1 : (( Hi > pp ) ? 2 : 0))
				//			 )  == 0)
				//			ok = true;
				//	}
				//};
				//if(ok)
				{

						// check bounding box is onscreen ....

				if( checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{
					triPtr = myObject->modelData->meshData.tlistPtr;

					globalpctr++;

					for( t = myObject->modelData->meshData.tcount; t--; )
					{

						currentmtl = triPtr->mtl;


						//if (!( potentiallyVisible = ( (currentmtl->bm.flags & MAT_SHADOW) == 0 ) ) )
						//	potentiallyVisible = shadon;			// if shadow material, only visible if shadow on.

 						//if( potentiallyVisible )
						{

							if( ! ( potentiallyVisible = ( ( currentmtl->bm.flags & MAT_DOUBLESIDED ) != 0 ) ) )
							{
							
								potentiallyVisible = ( (triPtr->faceNormal.x * WCtransform.E20 + 
											 triPtr->faceNormal.y * WCtransform.E21 + 
											 triPtr->faceNormal.z * WCtransform.E22 ) <= CULLINGEPSILON ) ;
							};

							if( potentiallyVisible )
							{
								envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
								shadowTri = ( currentmtl->bm.flags & MAT_SHADOW );

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
							#define SEPSILON -0.5f//0.0f
							potentiallyVisible = (  dp  <= SEPSILON) ;
						};
				 if( potentiallyVisible)
				 {
						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
							//WCtransform.multiplyV3By3x4mat( &pvPtr->transformed, &pvPtr->geoCoord );

							if( shadowTri )
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &pvPtr->geoCoord );
								catMat->translation += currentcamera->position;

								//tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
								tempv -= currentcamera->position;


								currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &pvPtr->transformed, &pvPtr->geoCoord );


pvPtr->transformed.z -= zoomz;
							catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal, &pvPtr->vertexNormal);
								pvPtr->l = pvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
								if( pvPtr->l < 0 ) pvPtr->l = 0;
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						    if( envon )
								getEnvmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
						};
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
							//WCtransform.multiplyV3By3x4mat( &qvPtr->transformed, &qvPtr->geoCoord );

							if( shadowTri )
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &qvPtr->geoCoord );
								catMat->translation += currentcamera->position;

								//tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv,  &transformedFnormal );
								tempv -= currentcamera->position;

								currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &qvPtr->transformed, &qvPtr->geoCoord );


qvPtr->transformed.z -= zoomz;
							catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
								qvPtr->l = qvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
								if( qvPtr->l < 0 ) qvPtr->l = 0;
							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						    if( envon )
								getEnvmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
						};
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							//WCtransform.multiplyV3By3x4mat( &rvPtr->transformed, &rvPtr->geoCoord );


							if( shadowTri )
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &rvPtr->geoCoord );
								catMat->translation += currentcamera->position;

								//tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv,  &transformedFnormal );
								tempv -= currentcamera->position;

								currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &rvPtr->transformed, &rvPtr->geoCoord );


rvPtr->transformed.z -= zoomz;
							catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
								rvPtr->l = rvPtr->transformedvertexNormal.DotProduct( &currentworld->sunLightDirection );
								if( rvPtr->l < 0 ) rvPtr->l = 0;
							rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						    if( envon )
								getEnvmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);

						};


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
								project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l );
						  };
						  projectedp.v = pvPtr->projectedPtr;		// use already projected value
						};
			
						if( (mask & 12) == 0 )
						{
						  projectedq.nv = &triPtr->q.nv;
						  projectedq.nv1 = qvPtr->nv1;

						  if( qvPtr->pcnt != globalpctr )		// not yet projected 
						  {
								qvPtr->pcnt = globalpctr;
								project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l  );
						  };
						  projectedq.v = qvPtr->projectedPtr;
						};

						if( (mask & 3) == 0 )
						{
						   projectedr.nv = &triPtr->r.nv;
						   projectedr.nv1 = rvPtr->nv1;

						   if( rvPtr->pcnt != globalpctr )		// not yet projected 
						   {	
								rvPtr->pcnt = globalpctr;
								project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
						   };
						   projectedr.v = rvPtr->projectedPtr;
						};
						
						zsplitting();
				}
						}
						};				// if (potentially visible
						triPtr++;
	
					};

					}	// if (ok)
				  
				}; // if (checkRboundboxVisibility

			};  // if(objPtr->modeldata != 0)

   if( myObject->definedInCameraSpace )
   {
			myObject->transform.translation.z -= zoomz;
   };

		   };			
			myObject = myObject->sister;

		};  // while( myObject)
}; 




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
			} else
			{
				catMat = &myObject->concatenatedTransform;
				catMat->translation -= currentcamera->position;
				WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
				catMat->translation += currentcamera->position;
			};


			//catMat = &myObject->concatenatedTransform;
			//catMat->translation -= currentcamera->position;
			//WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
			//catMat->translation += currentcamera->position;

			if( myObject->modelData != 0)		// ie not a NULL model 
			{

						// check bounding sphere is on screen

				//tempvectorN = myObject->transformedCentroid - currentcamera->position;  
				//currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );

				//float Hi = result.z + myObject->modelData->boundingRadius;
				//float Lo = result.z - myObject->modelData->boundingRadius;
				//float pp;

				//bool ok = false;

				//if ( (
				//	(( Lo < ZFRONTPLANE ) ? 1 : (( Lo > currentcamera->zbackplane ) ? 2 : 0)) &
				//	(( Hi < ZFRONTPLANE ) ? 1 : (( Hi > currentcamera->zbackplane ) ? 2 : 0))
				//	 ) == 0 )
				//{
				//	pp = displayWd2divprojcoeffW * result.z; 
				//	Hi = result.x + myObject->modelData->boundingRadius;
				//	Lo = result.x - myObject->modelData->boundingRadius;
				//	if ( (
				//		(( Lo < -pp ) ? 1 : (( Lo > pp ) ? 2 : 0)) &
				//		(( Hi < -pp ) ? 1 : (( Hi > pp ) ? 2 : 0))
				//		 ) == 0)
				//	{
				//		pp = displayHd2divprojcoeffH * result.z; 
				//		Hi = result.y + myObject->modelData->boundingRadius;
				//		Lo = result.y - myObject->modelData->boundingRadius;
				//		if ( (
				//			(( Lo < -pp ) ? 1 : (( Lo > pp ) ? 2 : 0)) &
				//			(( Hi < -pp ) ? 1 : (( Hi > pp ) ? 2 : 0))
				//			 )  == 0)
				//			ok = true;
				//	}
				//};
				//if(ok)
				{

						// check bounding box is onscreen ....

				if(checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{


					triPtr = myObject->modelData->meshData.tlistPtr;

					globalpctr++;

					for( t = myObject->modelData->meshData.tcount; t--; )
					{

						currentmtl = triPtr->mtl;

						//if (!( potentiallyVisible = ( (currentmtl->bm.flags & MAT_SHADOW) == 0 ) ) )
						//	potentiallyVisible = shadon;			// if ahdow material, only visible if shadow on.

						//if( potentiallyVisible )
						{

							if (!( potentiallyVisible = ((currentmtl->bm.flags & MAT_DOUBLESIDED) != 0) ) )
							{
							
								potentiallyVisible = ( (triPtr->faceNormal.x * WCtransform.E20 + 
											 triPtr->faceNormal.y * WCtransform.E21 + 
											 triPtr->faceNormal.z * WCtransform.E22 ) <= CULLINGEPSILON ) ;
							
							};
							if( potentiallyVisible )
							{
								envmapping = envon && (( currentmtl->bm.flags & MAT_ENVIRONMENT ) != 0);
								shadowTri = ( currentmtl->bm.flags & MAT_SHADOW );

							pvPtr = triPtr->p.v;
							qvPtr = triPtr->q.v;
							rvPtr = triPtr->r.v;

						if(shadowTri)
						{
							catMat->multiplyV3By3x3mat(	&transformedFnormal, &triPtr->faceNormal );
							float dp = boris2->currentworld->sunLightDirection.DotProduct( &transformedFnormal );
							#define SEPSILON 0.2f
							potentiallyVisible = (  dp  <= SEPSILON) ;
						};

		if(potentiallyVisible)
		{
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					 
	

						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
						

							if( shadowTri )
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &pvPtr->geoCoord );
								catMat->translation += currentcamera->position;

								//tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
								tempv -= currentcamera->position;


								currentcamera->RotMatrix.multiplyV3By3x4mat( &pvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &pvPtr->transformed, &pvPtr->geoCoord );

pvPtr->transformed.z -= zoomz;
							//catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
							if( envon)
							{
								catMat->multiplyV3By3x3mat( &pvPtr->transformedvertexNormal , &pvPtr->vertexNormal);
								getEnvmapUVs( &pvPtr->nv1, pvPtr, &objPtr->concatenatedTransform.translation);
							};
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
						
							if( shadowTri )
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &qvPtr->geoCoord );
								catMat->translation += currentcamera->position;

								//tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal );
								tempv -= currentcamera->position;


								currentcamera->RotMatrix.multiplyV3By3x4mat( &qvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &qvPtr->transformed, &qvPtr->geoCoord );
;  
qvPtr->transformed.z -= zoomz;
							//catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
							if( envon)
							{
								catMat->multiplyV3By3x3mat( &qvPtr->transformedvertexNormal , &qvPtr->vertexNormal);
								getEnvmapUVs( &qvPtr->nv1, qvPtr, &objPtr->concatenatedTransform.translation);
							};
							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							if ( shadowTri )						
							{
								float yresult;
								static vector3 tempv;
								triangle * triptr;
								catMat->translation -= currentcamera->position;
								catMat->multiplyV3By3x4mat( &tempv, &rvPtr->geoCoord );
								catMat->translation += currentcamera->position;

//								tempv.y += 100;
								tempv += currentcamera->position;
								boris2->currentworld->getShadowCoord( &tempv, &boris2->currentworld->sunLightDirection, &tempv, &transformedFnormal);
								tempv -= currentcamera->position;


								currentcamera->RotMatrix.multiplyV3By3x4mat( &rvPtr->transformed, &tempv );				
							} else
								WCtransform.multiplyV3By3x4mat( &rvPtr->transformed, &rvPtr->geoCoord );
rvPtr->transformed.z -= zoomz;
							//catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
							if( envon )
							{
								catMat->multiplyV3By3x3mat( &rvPtr->transformedvertexNormal , &rvPtr->vertexNormal);
								getEnvmapUVs( &rvPtr->nv1, rvPtr, &objPtr->concatenatedTransform.translation);
							};
							rvPtr->SCmaskInZ = (rvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((rvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};


						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						//  if a point is between the two z-planes  then we can project it into 2D (otherwise there
						//  is no need).
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					
						mask = (pvPtr->SCmaskInZ<<4) | (qvPtr->SCmaskInZ<<2) | rvPtr->SCmaskInZ;

						if( (mask & 48) == 0 )				// if vertex is between z-planes
						{
						  projectedp.nv = &triPtr->p.nv;
						  if( envon )
							  projectedp.nv1 = pvPtr->nv1;

						  if( pvPtr->pcnt != globalpctr )		// if vertex not yet projected
						  {
								pvPtr->pcnt = globalpctr;		// flag vertex as projected so another triangle won't project it again.
								project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, pvPtr->l );
						  };
						  projectedp.v = pvPtr->projectedPtr;		// use already projected value
						};
			
						if( (mask & 12) == 0 )
						{
						  projectedq.nv = &triPtr->q.nv;
						  if( envon )
							  projectedq.nv1 = qvPtr->nv1;

						  if( qvPtr->pcnt != globalpctr )		// not yet projected 
						  {
								qvPtr->pcnt = globalpctr;
								project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, qvPtr->l  );
						  };
						  projectedq.v = qvPtr->projectedPtr;
						};

						if( (mask & 3) == 0 )
						{
						   projectedr.nv = &triPtr->r.nv;
						   if( envon )
							  projectedr.nv1 = rvPtr->nv1;

						   if( rvPtr->pcnt != globalpctr )		// not yet projected 
						   {	
								rvPtr->pcnt = globalpctr;
								project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
						   };
						   projectedr.v = rvPtr->projectedPtr;
						};
						
						zsplitting();
			}
						}
						};


						triPtr++;
	
					};

					}	// if (ok)
				  
				}; // if (checkRboundboxVisibility

			};  // if(objPtr->modeldata != 0)

   if( myObject->definedInCameraSpace )
   {
			myObject->transform.translation.z -= zoomz;
   };
		  };

			myObject = myObject->sister;

		};  // while( myObject)
};

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
			
		};

	} else
	{
		objPtr = objectList;
		while (objPtr != 0 )		// for each object in the list
		{
			processObjectUnlit( objPtr);		// begin recursion at this object node
			objPtr = objPtr->next;
		};
	}


#ifdef EDITOR
	ulong i;
//DBGR.Dprintf("......");
	if(treecount)
	{
		//treeobject->setAngle(&vector3(0,0,0));

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

		//	treeobject->changed = true;
		//	treeobject->inheritTransformations();

//static char zzz[222];
//sprintf(zzz,"tree...%f %f %f",treestorePtr->x, treestorePtr->y, treestorePtr->z); 
//DBGR.Dprintf(zzz);

			processObjectUnlit(treeobject);

			treestorePtr++;
		};
	};
//DBGR.Dprintf("\n");

#endif

}


//----------------------------------------------------------------------------------------------------------------------------
// renderWorld
// -----------
//				render the world (asuming that one exists)
//----------------------------------------------------------------------------------------------------------------------------

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

polygoncounter = 0;

	slong firstXhunk = 0;
	slong lastXhunk = currentworld->xhunks-1; 
	slong firstZhunk = 0;
	slong lastZhunk = currentworld->zhunks-1; 

	zhunkcount= 1+lastZhunk - firstZhunk;


	hunk * thisHunkX = currentworld->hunkList + firstXhunk * currentworld->zhunks;		
		

	mesh * meshData = &currentworld->worldModel->meshData;

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


	for( xx = 1+lastXhunk-firstXhunk; xx--; )
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

	     for( zz = zhunkcount; zz--;  )
		 {
			  z1m02  = z0m02;
			  z0m02 += Me02zsize;
			  z1m12  = z0m12;
			  z0m12 += Me12zsize;
			  z1m22  = z0m22;
			  z0m22 += Me22zsize;

			  if( thisHunk->tcount_N + thisHunk->tcount_W)		// hunk not empty
			  {
				A = x0m00 + y0m01 ;
				B = x0m10 + y0m11 ;
				C = x0m20 + y0m21 ;

		
   				mask = 0x3F; 
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
				  };
										//if (mask==0) yes++; else no++;
				  if( mask == 0)
				  {
					renderthisHunk( thisHunk->tlistPtr_N, thisHunk->tcount_N) ;			// Non walls
					renderthisHunk( thisHunk->tlistPtr_W, thisHunk->tcount_W) ;			// Walls
				  };
			  };
			  thisHunk++;
		};	
		thisHunkX += currentworld->zhunks;   
	};	
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
	{

//		case 0:	// 00 00 00 .... No X split required.. do Y split on this triangle

			    
				outputTriangle( p, q, r );
 //			    break;
//
///		default:
//		case 21:		// 01 01 01 = completely off left - IGNORE
//		case 42:		// 10 10 10 = completely off right - IGNORE
//				break;
	};


  }
  else
	switch( (p->v->SCmaskX << 4) |  (q->v->SCmaskX << 2) |(r->v->SCmaskX ))
	{

		case 0:	// 00 00 00 .... No X split required.. do Y split on this triangle
			    process2DTriangleY( p, q, r );
//default:
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
	};

};

ulong combineColour( float a, float i )
{
	slong t;
	float f;

	f = a + i;
	FASTFTOL( &t, f );
	if( t > 255) return(255);
	return(t);
}

ulong combineColourL( float a, float i, float l)
{
	slong t;
	float f;

	f = a + i*l  ;//* (z );
	FASTFTOL( &t, f );
	if( t > 255) return(255);
	return(t);
}

ulong EcombineColourL( float a, float i, float l)
{
	slong t;
	float f;

	f = a + i*l*0.5f  ;//* (z );
	FASTFTOL( &t, f );
	if( t > 255) return(255);
	return(t);
}

ulong formfog( float z )//, float y )
{

		  //y = y * 0.01f;
	float zz = z*z*z*z;
	float f = ( 1 -  zz * zz * zz) * 255.0f * 22;
	slong t;
	FASTFTOL( &t, f );
	if( t > 255 ) t = 255;

	return( ((ulong)t) << 24);
}
						
//
// Note that we draw NON-alphas first from FRONT to BACK, then we draw alphas on top from BACK to FRONT. This order is
// vitally important for two reasons:
//				- (i)  the alpha regions of alpha-textures must be alpha'd to the correct texture beneath.
//				- (ii) Non-alpha textures can utilise any intelligent rasterisation overdraw elimination methods implemented
//

//debugger DBGR;

__inline ulong engine::formHwareKeyAlpha( ulong k,  float f )
{
	slong l;

	FASTFTOL( &l, (float)(8191.0f - (f * zNormalizer1 )));		// Alphas drawn in DESCENDING Z order (BACK TO FRONT)

	return ((((k & 0x07FFFFFFF) >> 13) + (((ulong)(l)) << 18))  + 0x080000000);		// key is  0x08000000 +   'z'(13) 't' (9)  'm'(9)

};

__inline ulong engine::formHwareKeyNonAlpha( ulong k,  float f )
{
	slong l;

	FASTFTOL( &l, (float)(f * zNormalizer1 ));//2730.0f) );			// Non Alphas drawn in ASCENDING Z order (FRONT TO BACK)

	return ( (k | ((ulong)(l))) );							// key is  't'(9)  'm'(9)  'z'(13)

};


bool triangleNOTflat( vector2P * pvPtr, vector2P * qvPtr, vector2P * rvPtr )
{
	float min, max;
	slong minl, maxl;

	minmax3( pvPtr->y, qvPtr->y, rvPtr->y, &min, &max );

	FASTFTOL( &minl, min );
	FASTFTOL( &maxl, max );

	return( minl != maxl );
};


static material dummyMaterial;

#define ASSIGNZ( r, z) \
	r =z;

void engine::performRender()
{ 
		outTriangle * tlist = currentOutListPtr->triangleList;
		outTriangle * llist = outTriangleNextPtr;
		sortlistentry * sortlistPtr = currentOutListPtr->sortlistPtr;
		ulong  zmask;
		ulong  count, thiskey;
		bool shadow;
		bool thismapped;
float ambientRed, ambientGrn, ambientBlu, ambientAlpha;
float diffuseRed, diffuseGrn, diffuseBlu, diffuseAlpha;

		if( tlist != llist)		// Not empty list
		{
		  	
		  if( (thiskey = tlist->mtl->sortingKey) & 0x080000000 )	// alpha texture
				zmask = formHwareKeyAlpha(thiskey, tlist->p.v->v3.z + tlist->q.v->v3.z + tlist->r.v->v3.z );
		  else
				zmask = formHwareKeyNonAlpha(thiskey, tlist->p.v->v3.z + tlist->q.v->v3.z + tlist->r.v->v3.z );

		  sortlistPtr->key= zmask;
 		  (sortlistPtr++)->idx= (ulong) (tlist++);
			 

					// other triangles
		  while( tlist != llist )
		  {
 			if( (thiskey = tlist->mtl->sortingKey) & 0x080000000 )	// alpha texture
					thiskey = formHwareKeyAlpha(thiskey, tlist->p.v->v3.z + tlist->q.v->v3.z + tlist->r.v->v3.z );
			else
					thiskey = formHwareKeyNonAlpha(thiskey, tlist->p.v->v3.z + tlist->q.v->v3.z + tlist->r.v->v3.z );

			zmask |=  (zmask ^ thiskey);
			sortlistPtr->key= thiskey;
			(sortlistPtr++)->idx= (ulong)(tlist++);
		  };

		  sort( (count = llist - currentOutListPtr->triangleList), 
			    zmask, 
				currentOutListPtr->sortlistPtr );

			//
			// render output list to display  ( if neither world nor objects exist then we should have nothing here!)
			// - use the currentOutListPtr->sortlistPtr to produce the triangles in order
			//

			//sortListPtr = currentOutListPtr->sortlistPtr;
			//outLists * olp = currentOutListPtr;

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
		
if( lightSwitch )
{ 
			while( tnum < count )
			{
				total = 0;
				//currentSelectedMaterial = &dummyMaterial;

			    while( (tnum < count) && ( total < BATCHSIZEM3) )
				{
					tlist = (outTriangle * )slist->idx;
					thismapped = envon && (( tlist->mtl->bm.flags & MAT_ENVIRONMENT ) != 0);

					shadow = (( tlist->mtl->bm.flags & MAT_SHADOW ) != 0);
						//
						// If we change base material  ... OR ...
						// If we change from one to two textures (or vice versa)
						//

					if( ( tlist->mtl != currentSelectedMaterial ) || (thismapped != environmentMappedMaterial) )
					{
						currentSelectedMaterial = tlist->mtl;
					
						nulltexture = (currentSelectedMaterial->bm.textureHandle == NULL);

						if(total)
						{
						  if( environmentMappedMaterial)			// old list was with environment mapping on
							renderList(vertexListD3D2, total, wframe, clip);
						  else										// old list was without environment mapping
							renderList(vertexListD3D, total, wframe, clip);

						  total = 0;
						};

						environmentMappedMaterial = thismapped;

						if(thismapped)
						{
							renderSetCurrentMaterial( &currentSelectedMaterial->bm, &skyEnvironment );
							testVertex2 = vertexListD3D2;
						}
						else
						{
							renderSetCurrentMaterial( &currentSelectedMaterial->bm);
							testVertex = vertexListD3D;
						};

						ambientColour = currentworld->sunAmbientColour;
						diffuseColour = currentworld->sunIncidentColour;


						if(  nulltexture )
						{
							ambientColour = //currentSelectedMaterial->diffusec;
							diffuseColour = currentSelectedMaterial->diffusec;//currentworld->sunIncidentColour;
							UdiffuseColour = currentSelectedMaterial->diffuse;
						};



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


						
					};


					pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
					qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
					rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;
	//if( triangleNOTflat( pvPtr, qvPtr, rvPtr ) )
	{
					if( environmentMappedMaterial)
					{

//DBGR.Dprintf(" env on.....");

						npvPtr1 = &tlist->p.nv1;
						nqvPtr1 = &tlist->q.nv1;
						nrvPtr1 = &tlist->r.nv1;


						testVertex2->sx  =  pvPtr->x ;
						testVertex2->sy  =  pvPtr->y ;
						ASSIGNZ( testVertex2->sz , pvPtr->z );
						testVertex2->rhw =  pvPtr->w ; //RECIPROCAL(1000 * pvPtr->z); //pvPtr->w ;
						testVertex2->tu1  = npvPtr->u ;
						testVertex2->tv1  = npvPtr->v ;
						testVertex2->tu2  = npvPtr1->u ;
						testVertex2->tv2  = npvPtr1->v ;
						
						if(currentcamera->underwater)
							testVertex2->color = UNDERWATER_UDIFFUSE;
						else
								testVertex2->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( EcombineColourL(ambientRed,diffuseRed,pvPtr->l ) << 16) +
								( EcombineColourL(ambientGrn,diffuseGrn,pvPtr->l ) << 8 ) +
								( EcombineColourL(ambientBlu,diffuseBlu,pvPtr->l ) );
						if( boris2->fog )
							testVertex2->specular = formfog( testVertex2->sz);//, pvPtr->v3.y );

						testVertex2++;

						testVertex2->sx  =  qvPtr->x ;
						testVertex2->sy  =  qvPtr->y ;
						ASSIGNZ( testVertex2->sz , qvPtr->z );
						testVertex2->rhw =  qvPtr->w ;//RECIPROCAL(1000 * qvPtr->z);// ;
						testVertex2->tu1  = nqvPtr->u ;
						testVertex2->tv1  = nqvPtr->v ;
						testVertex2->tu2  = nqvPtr1->u  ;
						testVertex2->tv2  = nqvPtr1->v  ;

						
						if(currentcamera->underwater)
							testVertex2->color = UNDERWATER_UDIFFUSE;
						else
								testVertex2->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( EcombineColourL(ambientRed,diffuseRed,qvPtr->l ) << 16) +
								( EcombineColourL(ambientGrn,diffuseGrn,qvPtr->l  ) << 8 ) +
								( EcombineColourL(ambientBlu,diffuseBlu,qvPtr->l ) );
						if( boris2->fog )
							testVertex2->specular = formfog( testVertex2->sz );//, qvPtr->v3.y );

						testVertex2++;

						testVertex2->sx  =  rvPtr->x ;
						testVertex2->sy  =  rvPtr->y ;
						ASSIGNZ( testVertex2->sz , rvPtr->z ) ;
						testVertex2->rhw =  rvPtr->w ;// RECIPROCAL(1000 * rvPtr->z);//rvPtr->w ;
						testVertex2->tu1  = nrvPtr->u ;
						testVertex2->tv1  = nrvPtr->v ;
						testVertex2->tu2  = nrvPtr1->u ;
						testVertex2->tv2  = nrvPtr1->v ;

						if(currentcamera->underwater)
							testVertex2->color = UNDERWATER_UDIFFUSE;
						else
								testVertex2->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( EcombineColourL(ambientRed,diffuseRed,rvPtr->l  ) << 16) +
								( EcombineColourL(ambientGrn,diffuseGrn,rvPtr->l ) << 8 ) +
								( EcombineColourL(ambientBlu,diffuseBlu,rvPtr->l  ) );
						if( boris2->fog )
							testVertex2->specular = formfog( testVertex2->sz );//, rvPtr->v3.y );

						testVertex2++;

					} else
					{

						testVertex->sx  =  pvPtr->x ;
						testVertex->sy  =  pvPtr->y ;
						ASSIGNZ( testVertex->sz ,  pvPtr->z );
						testVertex->rhw =  pvPtr->w ; //RECIPROCAL(1000 * pvPtr->z); //pvPtr->w ;
						testVertex->tu  = npvPtr->u ;
						testVertex->tv  = npvPtr->v ;

						if(currentcamera->underwater)
							testVertex->color = UNDERWATER_UDIFFUSE;
						else
							if(shadow)
							{
								testVertex->color = UdiffuseColour;
							
							} else
							{
								testVertex->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( combineColourL(ambientRed,diffuseRed,pvPtr->l  ) << 16) +
								( combineColourL(ambientGrn,diffuseGrn,pvPtr->l ) << 8 ) +
								( combineColourL(ambientBlu,diffuseBlu,pvPtr->l ) );
								if( boris2->fog )
									testVertex->specular = formfog( testVertex->sz);//, pvPtr->v3.y );
							};

						testVertex++;

						testVertex->sx  =  qvPtr->x ;
						testVertex->sy  =  qvPtr->y ;
						ASSIGNZ( testVertex->sz , qvPtr->z );
						testVertex->rhw =  qvPtr->w ;//RECIPROCAL(1000 * qvPtr->z);// ;
						testVertex->tu  = nqvPtr->u ;
						testVertex->tv  = nqvPtr->v ;

						if(currentcamera->underwater)
							testVertex->color = UNDERWATER_UDIFFUSE;
						else
							if(shadow)
							{
								testVertex->color = UdiffuseColour;
							
							} else
							{
								testVertex->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( combineColourL(ambientRed,diffuseRed,qvPtr->l  ) << 16) +
								( combineColourL(ambientGrn,diffuseGrn,qvPtr->l ) << 8 ) +
								( combineColourL(ambientBlu,diffuseBlu,qvPtr->l  ) );
								if( boris2->fog )
									testVertex->specular = formfog( testVertex->sz );//, qvPtr->v3.y );
							};
//testVertex->specular = 0;

						testVertex++;

						testVertex->sx  =  rvPtr->x ;
						testVertex->sy  =  rvPtr->y ;
						ASSIGNZ( testVertex->sz , rvPtr->z );
						testVertex->rhw =  rvPtr->w ;// RECIPROCAL(1000 * rvPtr->z);//rvPtr->w ;
						testVertex->tu  = nrvPtr->u ;
						testVertex->tv  = nrvPtr->v ;

						if(currentcamera->underwater)
							testVertex->color = UNDERWATER_UDIFFUSE;
						else
							if(shadow)
							{
								testVertex->color = UdiffuseColour;
							
							} else
							{
								testVertex->color = 
								( combineColour(ambientAlpha,diffuseAlpha  ) << 24) +
								( combineColourL(ambientRed,diffuseRed,rvPtr->l  ) << 16) +
								( combineColourL(ambientGrn,diffuseGrn,rvPtr->l ) << 8 ) +
								( combineColourL(ambientBlu,diffuseBlu,rvPtr->l ) );
								if( boris2->fog )
									testVertex->specular = formfog( testVertex->sz );//, rvPtr->v3.y );
							};
//testVertex->specular = 0x0FFFFFF;

						testVertex++;
					};
	

				total+=3;
			
			
		};	
		tnum++;
		slist++;
				};

						// BATCHSIZE REACHES OR ALL TRIANGLES REACHED
				if(total)
				{
				  if( environmentMappedMaterial )
				  {
					renderList(vertexListD3D2, total, wframe, clip);
				  }
				  else
					renderList(vertexListD3D, total, wframe, clip);
				  total = 0;
				}

			};

}
else
{

		while( tnum < count )
			{
				total = 0;
				//currentSelectedMaterial = &dummyMaterial;

			    while( (tnum < count) && ( total < BATCHSIZEM3) )
				{
					tlist = (outTriangle * )slist->idx;
					thismapped = envon && (( tlist->mtl->bm.flags & MAT_ENVIRONMENT ) != 0);
						//
						// If we change base material  ... OR ...
						// If we change from one to two textures (or vice versa)
						//

					if( ( tlist->mtl != currentSelectedMaterial ) || (thismapped != environmentMappedMaterial) )
					{
						currentSelectedMaterial = tlist->mtl;
					
						nulltexture = (currentSelectedMaterial->bm.textureHandle == NULL);
//if( nulltexture)
//{
//	currentSelectedMaterial->bm.flags |= MAT_ALPHA;
	//currentSelectedMaterial->bm.flags &= (0x0FFFFFFFF ^ MAT_ENVIRONMENT);
//}
						if(total)
						{
						if( environmentMappedMaterial)			// old list was with environment mapping on
						  renderList(vertexListD3D2, total, wframe, clip);
						else										// old list was without environment mapping
						  renderList(vertexListD3D, total, wframe, clip);

						total = 0;
						};

						environmentMappedMaterial = thismapped;

						if(thismapped)
						{
							renderSetCurrentMaterial( &currentSelectedMaterial->bm, &skyEnvironment );
							testVertex2 = vertexListD3D2;
						}
						else
						{
							renderSetCurrentMaterial( &currentSelectedMaterial->bm);
							testVertex = vertexListD3D;
						};

						if(currentcamera->underwater)
							UdiffuseColour = UNDERWATER_UDIFFUSE;
						else
							UdiffuseColour = currentSelectedMaterial->diffuse;

						//if(  nulltexture )
						//{
						//	UdiffuseColour = currentSelectedMaterial->diffuse;
						//};




//if( nulltexture)
//{
//	UdiffuseColour = 0x0FFFFFFFF;//x00FFFFFF;
//	//UdiffuseColour |= 0x0F000000;
//};

					};


					pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
					qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
					rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;
//if( triangleNOTflat( pvPtr, qvPtr, rvPtr ) )
{
					if( environmentMappedMaterial)
					{

//DBGR.Dprintf(" env on.....");

						npvPtr1 = &tlist->p.nv1;
						nqvPtr1 = &tlist->q.nv1;
						nrvPtr1 = &tlist->r.nv1;


						testVertex2->sx  =  pvPtr->x ;
						testVertex2->sy  =  pvPtr->y ;
						ASSIGNZ( testVertex2->sz , pvPtr->z );
						testVertex2->rhw =  pvPtr->w ; //RECIPROCAL(1000 * pvPtr->z); //pvPtr->w ;
						testVertex2->tu1  = npvPtr->u ;
						testVertex2->tv1  = npvPtr->v ;
						testVertex2->tu2  = npvPtr1->u ;
						testVertex2->tv2  = npvPtr1->v ;

						testVertex2->color = UdiffuseColour;
		
						testVertex2++;

						testVertex2->sx  =  qvPtr->x ;
						testVertex2->sy  =  qvPtr->y ;
						ASSIGNZ( testVertex2->sz ,  qvPtr->z );
						testVertex2->rhw =  qvPtr->w ;//RECIPROCAL(1000 * qvPtr->z);// ;
						testVertex2->tu1  = nqvPtr->u ;
						testVertex2->tv1  = nqvPtr->v ;
						testVertex2->tu2  = nqvPtr1->u  ;
						testVertex2->tv2  = nqvPtr1->v  ;

						testVertex2->color = UdiffuseColour;

						testVertex2++;

						testVertex2->sx  =  rvPtr->x ;
						testVertex2->sy  =  rvPtr->y ;
						ASSIGNZ( testVertex2->sz  , rvPtr->z );
						testVertex2->rhw =  rvPtr->w ;// RECIPROCAL(1000 * rvPtr->z);//rvPtr->w ;
						testVertex2->tu1  = nrvPtr->u ;
						testVertex2->tv1  = nrvPtr->v ;
						testVertex2->tu2  = nrvPtr1->u ;
						testVertex2->tv2  = nrvPtr1->v ;

						testVertex2->color = UdiffuseColour;
						
						testVertex2++;

					} else
					{

						testVertex->sx  =  pvPtr->x ;
						testVertex->sy  =  pvPtr->y ;
						ASSIGNZ( testVertex->sz  ,  pvPtr->z ) ;
						testVertex->rhw =  pvPtr->w ; //RECIPROCAL(1000 * pvPtr->z); //pvPtr->w ;
						testVertex->tu  = npvPtr->u ;
						testVertex->tv  = npvPtr->v ;

						testVertex->color = UdiffuseColour;
					
						testVertex++;

						testVertex->sx  =  qvPtr->x ;
						testVertex->sy  =  qvPtr->y ;
						ASSIGNZ( testVertex->sz  ,  qvPtr->z );
						testVertex->rhw =  qvPtr->w ;//RECIPROCAL(1000 * qvPtr->z);// ;
						testVertex->tu  = nqvPtr->u ;
						testVertex->tv  = nqvPtr->v ;

						testVertex->color = UdiffuseColour;
							
						testVertex++;

						testVertex->sx  =  rvPtr->x ;
						testVertex->sy  =  rvPtr->y ;
						ASSIGNZ( testVertex->sz  , rvPtr->z );
						testVertex->rhw =  rvPtr->w ;// RECIPROCAL(1000 * rvPtr->z);//rvPtr->w ;
						testVertex->tu  = nrvPtr->u ;
						testVertex->tv  = nrvPtr->v ;

						testVertex->color = UdiffuseColour;
					
						testVertex++;
					};
		total+=3;
};

				tnum++;
				slist++;
					
				};

						// BATCHSIZE REACHES OR ALL TRIANGLES REACHED
				if(total)
				{
				  if( environmentMappedMaterial )
				  {
					renderList(vertexListD3D2, total, wframe, clip);
				  }
				  else
					renderList(vertexListD3D, total, wframe, clip);
				  total = 0;
				}

			};
//............

}




	}  // if (tlist


}



//class entry2d {
//public:
//		outTriangle   otri;
//
//		entry2d * next;
//};

void  engine::start2Dlist()
{
	current2dvertexListPtr = vertex2dlist;
	currentvertex2dmatlistPtr  = vertex2dmatList;

};


//
// Give up close values for both z and w so that whichever way the drivers buffer is ok!!
//

#define INYOURFACEZ 0.001f
#define INYOURFACEW 1.01f

//================================================================================================================================
//================================================================================================================================

void  engine::add2dvertex( float x, float y, float u, float v )
{
	current2dvertexListPtr->sx = x;
	current2dvertexListPtr->sy = y;
	current2dvertexListPtr->sz = INYOURFACEZ;
	current2dvertexListPtr->rhw = INYOURFACEW;
	current2dvertexListPtr->color = 0x00FFFFFF;
	current2dvertexListPtr->specular=0;
	current2dvertexListPtr->tu = u;
	current2dvertexListPtr->tv = v;
	current2dvertexListPtr++;
};

void  engine::add2dvertex( float x, float y, float u, float v, float z )
{
	current2dvertexListPtr->sx = x;
	current2dvertexListPtr->sy = y;
	current2dvertexListPtr->sz = z;
	current2dvertexListPtr->rhw = INYOURFACEW;
	current2dvertexListPtr->color = 0x00FFFFFF;
	current2dvertexListPtr->specular=0;
	current2dvertexListPtr->tu = u;
	current2dvertexListPtr->tv = v;
	current2dvertexListPtr++;
};
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
	currentvertex2dmatlistPtr->flags = flags;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v );
	add2dvertex( x+w, y,  u+uw, v );
	add2dvertex( x+w, y+h, u+uw, v+vh );

	add2dvertex( x, y, u, v );
	add2dvertex( x+w, y+h, u+uw, v+vh );
	add2dvertex( x, y+h, u, v+vh );

	return( hdl );
};

DWORD engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   char * filename, ulong flags,   float z)
{

		// every one material list entry covers 6 vertices (two triangles )

	DWORD hdl;
	currentvertex2dmatlistPtr->textureHandle = hdl = (getTexturePtr(filename, false ))->textureHandle;
	currentvertex2dmatlistPtr->flags = flags;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v, z );
	add2dvertex( x+w, y,  u+uw, v, z );
	add2dvertex( x+w, y+h, u+uw, v+vh, z );

	add2dvertex( x, y, u, v, z );
	add2dvertex( x+w, y+h, u+uw, v+vh, z );
	add2dvertex( x, y+h, u, v+vh, z );

	return( hdl );
};
//================================================================================================================================


void  engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle)
{

		// every one material list entry covers 6 vertices (two triangles )

//DBGR.Dprintf("handle = %d", texHandle);

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v );
	add2dvertex( x+w, y,  u+uw, v );
	add2dvertex( x+w, y+h, u+uw, v+vh );

	add2dvertex( x, y, u, v );
	add2dvertex( x+w, y+h, u+uw, v+vh );
	add2dvertex( x, y+h, u, v+vh );
};

void  engine::addTo2Dlist( float x, float y, float w, float h,
						   float u, float v, float uw, float vh,
						   DWORD texHandle, float z)
{

		// every one material list entry covers 6 vertices (two triangles )

//DBGR.Dprintf("handle = %d", texHandle);

	currentvertex2dmatlistPtr->textureHandle = texHandle;
	currentvertex2dmatlistPtr->flags = 0;
	currentvertex2dmatlistPtr++;

	x *= coeff2dW;
	w *= coeff2dW;
	y *= coeff2dH;
	h *= coeff2dH;

	add2dvertex( x, y, u, v , z);
	add2dvertex( x+w, y,  u+uw, v, z );
	add2dvertex( x+w, y+h, u+uw, v+vh, z );

	add2dvertex( x, y, u, v, z );
	add2dvertex( x+w, y+h, u+uw, v+vh, z );
	add2dvertex( x, y+h, u, v+vh, z );
};
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

	//while( thismaterial != currentvertex2dmatlistPtr )
	//{
	//
	//	renderSetCurrentMaterial(thismaterial);
	//
	//	renderList(thisvertex, 6);
	//
	//	thisvertex+=6;
	//	thismaterial++;
	//};

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
		};
		counter += 6;
		thisvertex+=6;
		thismaterial++;
	};
	renderList(firstvertex, counter, wframe, clip);

};

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

//extern engine * doris;


//================================================================================================================================
// ShadowToGround
// ---------------
//				A square area defining a shadow texture 
//================================================================================================================================
/*
#define XDIVS   4
#define ZDIVS   6

vector3 shadowPoint[ XDIVS * ZDIVS ];
void engine::shadowToGround( float minx, float maxx, float minz, float maxz, matrix3x4 * m, float maxy )
{


	float xc, zc, xdelta, zdelta;
	vector3 * sPointer;
	long xx, zz;
	float yresult;
	float width, height;

	triangle * triptr;

	width = maxx - minx;
	height = maxz - minz;

	sPointer = shadowPoint;
	xdelta = width * RECIPROCAL( (float) XDIVS );
	zdelta = height * RECIPROCAL( (float) ZDIVS );

	xc = minx;
	for( xx = XDIVS; xx--; )
	{
		zc = minz;
		for( zz = ZDIVS; zz--; )
		{

			// Now transform the co-ordinate   xc, CONSTY, zc  to give the appropriate x and z components of the transform.
			//  - we are not interested in the Y value.
sPointer->y = maxy;
sPointer->x = m->E00 * xc +  m->E02 * zc + m->translation.x ; 
sPointer->z = m->E20 * xc +  m->E22 * zc + m->translation.z ;  

boris2->currentworld->getY( sPointer, &yresult, &triptr );

//void vector3::multiplyByMatrix3x4( vector3 * v, matrix3x4 * m )
//{
//
//	x = m->E00 * v->x + m->E01 * v->y + m->E02 * v->z + m->translation.x ;   // v->? == 1
//	y = m->E10 * v->x + m->E11 * v->y + m->E12 * v->z + m->translation.y ;  
//	z = m->E20 * v->x + m->E21 * v->y + m->E22 * v->z + m->translation.z ;  
//}
			// - now get the Y co-ordinate for the XZ

			sPointer++;
			zc += zdelta;
		};
		xc += xdelta;
	};

		//
		// Now generate each triangle into the list via 'zsplitting'
		//


}
*/


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
				};
			} else
				if( zoomz <= zoomdistance )
				{
					zoomz = zoomdistance;
					zoomdone = true;
				};
		  }
		};

		if(currentcamera->moved)
		{
			currentcamera->checkUnderWater();
			currentcamera->moved = false;
		};

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
		  };
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


			// advance output list 

	//outLists * olp = currentOutListPtr

//	if( --olCountdown ) 
//	{
//		currentOutListPtr =  * outListListPtr;		// reset to first output list
//		olCountdown = olCount;						// reset to number of output lists
//	}
//	else
//		currentOutListPtr++;


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

__inline bool engine::checkRboundboxVisibility(RboundBox * r)
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
	}; 

    float px,py;
	uchar mask; 

	mask = 0x3F;
	tempvectorN = r->v000 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );

	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v001 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v010 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v011 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v100 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v101 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v110 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )
	tempvectorN = r->v111 - currentcamera->position;  
	currentcamera->RotMatrix.multiplyV3By3x4mat( &result, &tempvectorN );
	OFFSCREENA( result.x, result.y, result.z )

	return false;
};

//
// moved = has window moved
// heldlParam = lParam fro WM_MOVE ofwindows message handler if move occurs.
//

void engine::beginFrame( bool moved, LPARAM heldlParam)
{
		renderBeginScene( (!skyon) | (wframe) );

		start2Dlist();

		if( moved )			
			renderMove(heldlParam);
		
		renderstart();
};

void engine::lightingOn()
{
	if(currentworld)
		currentworld->turnSunOn();
	lightSwitch = true;
};

void engine::lightingOff()
{
	if(currentworld)
		currentworld->turnSunOff();
	lightSwitch = false;
};




void  engine::appendSpriteFX( spriteFX * sfx, vector3 * position, ulong * counter, bool fU, bool fV )
{
    spriteFXentry * newEntry = new spriteFXentry;

	if(counter)
		(*counter)++;

	newEntry->flipU = fU;
	newEntry->flipV = fV;

	newEntry->externalcounter = counter;

	newEntry->triangleB.p.v =
	newEntry->triangleA.p.v = new vertex;

	newEntry->triangleA.q.v = new vertex;

	newEntry->triangleB.q.v =
	newEntry->triangleA.r.v = new vertex;

	newEntry->triangleB.r.v = new vertex;


	newEntry->spriteFXptr = sfx;
	newEntry->worldPosition = * position;
	newEntry->currentFrame = sfx->firstFrame;
	newEntry->currentCount = sfx->repeatCount;
	newEntry->reverse = false;					// signifies current direction of sequence
	newEntry->frameTimer.setTimer(sfx->framesPerSecond );

	if( newEntry->next = spriteFXlist)	// = is correct
		spriteFXlist->prev = newEntry;

	newEntry->prev = NULL;

	spriteFXlist = newEntry;
};	

//
//  update sprite list (will include delinking when a spriteFX occurence used up) 
//  and render the frames. (if projected on screen)
//

//debugger DBGR;


void  engine::renderSpriteFXlist()
{
	//  NOTE THAT WHEN WE HAVE FINISHED WITH A SPRITE EFFECT WE MUST DESTORY ITS 'SPITEFXENTRY' OCCURENCE

	spriteFXentry * thisFXentry = spriteFXlist;
	spriteFXentry * oldthisFXentry;
	static vector3 tempcoord;
//	static vector3 tempcoord1;
	float actualLengthX, actualLengthY;

	vertex   * pvPtr, * qvPtr, * rvPtr;


	envmapping = false;		// NO ENVIRONMENT MAPPING ON SPRITES!!!!!

	while( thisFXentry)
	{
		
	 	    ulong framesToGo = thisFXentry->frameTimer.getTicks();
			bool  disconnectEntry = false; 
	
				//
				// advance to next frame(s) in sequence according to the time elapsed
				//

			while( framesToGo ) 
			{
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
				
				} else
				{
					disconnectEntry = true;
					break;			// No more iterations... IGNORE ANY REMAINING FRAMES TO GO
				}
			  };
					// frame exists and has changed so update anything possible

				triPtr = &thisFXentry->triangleA;
				triangle * triPtrB = &thisFXentry->triangleB;


					triPtrB->p.nv = triPtr->p.nv = thisFXentry->currentFrame->nv;

					triPtr->q.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
					triPtr->q.nv.v = triPtr->p.nv.v;

					triPtrB->q.nv.u = triPtr->r.nv.u = triPtr->p.nv.u + thisFXentry->currentFrame->ulength;
					triPtrB->q.nv.v = triPtr->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

					triPtrB->r.nv.u = triPtr->p.nv.u ;
					triPtrB->r.nv.v = triPtr->p.nv.v + thisFXentry->currentFrame->vlength;

					if( thisFXentry->flipV )
					{
						triPtr->p.nv.v = 1.0f - triPtr->p.nv.v;
						triPtr->q.nv.v = 1.0f - triPtr->q.nv.v;
						triPtr->r.nv.v = 1.0f - triPtr->r.nv.v;

						triPtrB->p.nv.v = 1.0f - triPtrB->p.nv.v;
						triPtrB->q.nv.v = 1.0f - triPtrB->q.nv.v;
						triPtrB->r.nv.v = 1.0f - triPtrB->r.nv.v;

					}

			  framesToGo--;
			};

				
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

//		boris2->currentcamera->transformWorldVector(&tempcoord, &thisFXentry->worldPosition );

		boris2->currentcamera->transformWorldVector(&tempcoord, &( thisFXentry->worldPosition +
							thisFrame->offset)	);

					tempcoord.x -= actualLengthX * 0.5f;
					tempcoord.y -= actualLengthY * 0.5f;

				//	tempcoord.z += thisFXentry->spriteFXptr->zbufferDelta;
				//	if( tempcoord.z <  ZFRONTPLANE ) tempcoord.z = ZFRONTPLANE; 

				
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
					  project( (pvPtr->projectedPtr = outNextVector2Pptr++), &pvPtr->transformed, 1.0f );
					  projectedp.v = pvPtr->projectedPtr;
					};
			
					if( (mask & 12) == 0 )
					{
					  projectedq.nv = &triPtr->q.nv;
					  project( (qvPtr->projectedPtr = outNextVector2Pptr++), &qvPtr->transformed, 1.0f );
					  projectedq.v = qvPtr->projectedPtr;	
					};

					if( (mask & 3) == 0 )
					{
					  projectedr.nv = &triPtr->r.nv;
					  project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f);
					  projectedr.v = rvPtr->projectedPtr;	
					};

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
					};
			
					if( (mask & 3) == 0 )
					{
					  projectedr.nv = &triPtr->r.nv;
					  project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, 1.0f);
					  projectedr.v = rvPtr->projectedPtr;	
					};



					zsplitting();

				};


				thisFXentry = thisFXentry->next;
			}
	}

};							// update and render spriteFX's




void engine::enableEnvironmentMapping()
{
	envon = true;
};

void engine::disableEnvironmentMapping()
{
	envon = false;
};

void engine::enableSky()
{
	skyon = true;
};

void engine::disableSky()
{
	skyon = false;
};

void engine::clipOn()
{
	clip = true;
};

void engine::clipOff()
{
	clip = false;
};

void engine::clipToggle()
{
	clip = !clip;
};
//================================================================================================================================
// END OF FILE
//================================================================================================================================
