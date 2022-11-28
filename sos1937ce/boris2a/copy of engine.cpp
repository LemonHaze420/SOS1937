//================================================================================================================================
// engine.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"


#define ZFRONTPLANE 1.1f

//
// ttotal = number of triangles in OUTPUT lists
// vtotal = number of vertices in VERTEX lists
//

//debugger DBGR;



engine  * boris2;			// The global engine



__int64 timing_basecounter;
float   timing_oneOverFrequency;
__int64 timing_frequency;


#define BATCHSIZE 1500
D3DTLVERTEX vertexListD3D[BATCHSIZE];
#define BATCHSIZEM3 ( BATCHSIZE - 3 )

engine::engine(  ulong eflags,  slong outListCount, slong ttotal, slong vtotal, slong ntotal)
{
	slong i;

	outLists ** myolPtr;

	engine_flags      = eflags;

	currentdisplay    = NULL;
    currentcamera     = NULL;
    currentworld      = NULL;
	objectList		  = NULL;
    spriteFXlist	  = NULL; 

	lightSwitch = false;


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
}

engine::~engine()
{
	slong i;
	outLists ** myolPtr;


			// remove outLists 

	myolPtr = outListListPtr;

	for( i = olCount; i--; )
		(*myolPtr++)->~outLists();
	
	delete[] outListListPtr;

	renderDeInitialise();
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

		projcoeffW = currentcamera->zeye * displayW * CONST_RECIPROCAL(BASERES_WIDTH);
		projcoeffH = currentcamera->zeye * displayH * CONST_RECIPROCAL(BASERES_HEIGHT);
	
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
		objPtr->inheritTransformations();
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
		
		
			loHunkX = (slong)((xl - currentworld->meshData.MinCoordX) * currentworld->oneoverxhunksize ) ;
			hiHunkX = (slong)((xh - currentworld->meshData.MinCoordX) * currentworld->oneoverxhunksize ) + 1;

			loHunkZ = (slong)((zl - currentworld->meshData.MinCoordZ) * currentworld->oneoverzhunksize) ;
			hiHunkZ = (slong)((zh - currentworld->meshData.MinCoordZ) * currentworld->oneoverzhunksize) + 1;
		
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

			hx = loHunkX * currentworld->xhunksize + currentworld->meshData.MinCoordX ;
			hunkPtrX = currentworld->hunkList + loHunkX * currentworld->zhunks;

			bool cont = true;
	
			xx = 1 + hiHunkX - loHunkX;
			while( (xx!=0) && cont)
			{
				xx--;

				hz = loHunkZ * currentworld->zhunksize + currentworld->meshData.MinCoordZ; //minz;
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

						
								minmax3( l->x, m->x, n->x, &xl, &xh );																								
								minmax3( l->y, m->y, n->y, &yl, &yh );																								
								minmax3( l->z, m->z, n->z, &zl, &zh );																								
							
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

										float rsqr = (dx * triPtr->faceNormal.x + 
												  dy * triPtr->faceNormal.y + 	
											      dz * triPtr->faceNormal.z +
												  triPtr->planeD
												 );

										rsqr *= rsqr * triPtr->ABC;

										if (rsqr < dsqr * 0.2)
										{

// TEMP.
// Find closest corner of bounding box around object and return a pointer to the co-ordinate
//

float mindistance = 9999999.0f;
ulong wnum;


		 rsqr = ( bboxPtr->v000.x * triPtr->faceNormal.x + 
				  bboxPtr->v000.y * triPtr->faceNormal.y + 	
				  bboxPtr->v000.z * triPtr->faceNormal.z + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
				mindistance = rsqr;
				wnum = 0;
				//bestcoord = &bboxPtr->v000;
		 };
		 rsqr = ( bboxPtr->v001.x * triPtr->faceNormal.x + 
				  bboxPtr->v001.y * triPtr->faceNormal.y + 	
				  bboxPtr->v001.z * triPtr->faceNormal.z + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
 				mindistance = rsqr;
				 wnum = 2;
				//bestcoord = &bboxPtr->v001;
		 };
	
		 rsqr = ( bboxPtr->v100.x * triPtr->faceNormal.x + 
				  bboxPtr->v100.y * triPtr->faceNormal.y + 	
				  bboxPtr->v100.z * triPtr->faceNormal.z + 
				 triPtr->planeD ); 
		 if (rsqr < mindistance ) 
		 {
				mindistance = rsqr;
				wnum = 1;
				//bestcoord = &bboxPtr->v100;
		 };
		 rsqr = ( bboxPtr->v101.x * triPtr->faceNormal.x + 
				  bboxPtr->v101.y * triPtr->faceNormal.y + 	
				  bboxPtr->v101.z * triPtr->faceNormal.z + 
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


//================================================================================================================================
// produce a projected vertex from the 3d co-oridnate vertex.
//		- automatically produces the SutherlandCohen mask against the current display since this will always be used.
//
//================================================================================================================================

__inline void engine::project( vector2P * v2,  vector3 * v, float l )
{

	v2->w   = RECIPROCAL( v->z );

	v2->x	= v->x * v2->w * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - v->y * v2->w * projcoeffH;
	v2->z3d = (v->z - ZFRONTPLANE ) * zNormalizer;				

	v2->x3d = v->x;
	v2->y3d = v->y;
	v2->l   = l;
														
	v2->SCmaskX = (( v2->x < displayDeltaX ) ? 1 : ( (v2->x > displayWM1deltaX) ? 2 : 0 ));
	v2->SCmaskY = (( v2->y < displayDeltaY ) ? 1 : ( (v2->y > displayHM1deltaY) ? 2 : 0 ));

};

__inline void engine::project( vector2P * v2,  float vx, float vy, float vz, float l )
{
	
	v2->w   = RECIPROCAL( vz );

	v2->x	= vx * v2->w * projcoeffW + displayWd2deltaX;
	v2->y	= displayHd2deltaY - vy * v2->w * projcoeffH;
	v2->z3d = (vz - ZFRONTPLANE ) * zNormalizer;

	v2->x3d = vx;
	v2->y3d = vy;
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

	zNormalizer = RECIPROCAL(currentcamera->zbackplane - ZFRONTPLANE);

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

	t = ( splitz - avg->z) * RECIPROCAL( bvg->z - avg->z );



			//	calculate x and y at split point

	project(  outNextVector2Pptr, (avg->x + t * ( bvg->x - avg->x )),
								  (avg->y + t * ( bvg->y - avg->y )), 
								  splitz,
								  a->v->l + t * ( b->v->l - a->v->l ));

			// now split non-goemetric data to create  r->nv...  (we must create a new entry of nongeodata)

	outNextNonGeoptr->u = a->nv.u + t * ( b->nv.u - a->nv.u );
	outNextNonGeoptr->v = a->nv.v + t * ( b->nv.v - a->nv.v );

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

//	splitEdgeZ( r0, a, b, splitz0 );
//  splitEdgeZ( r1, a, b, splitz1 );
//return;


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

	t  = ( splitz0 - avg->z) * r;
	outNextNonGeoptr->u = a->nv.u + t * ud;
	outNextNonGeoptr->v = a->nv.v + t * vd;
	project( outNextVector2Pptr, avg->x + t * xd, avg->y + t * yd, splitz0, a->v->l + t * ld );
	r0->nv = outNextNonGeoptr++;
	r0->v = outNextVector2Pptr++;

	t  = ( splitz1 - avg->z) * r;
	outNextNonGeoptr->u = a->nv.u + t * ud;
	outNextNonGeoptr->v = a->nv.v + t * vd;
	project( outNextVector2Pptr, avg->x + t * xd, avg->y + t * yd, splitz1, a->v->l + t * ld);
	r1->nv = outNextNonGeoptr++;
	r1->v = outNextVector2Pptr++;
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
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;

			// calculate parametric co-efficient of line at split



	t = (  (outNextVector2Pptr->x = splitx) - av->x) * RECIPROCAL( bv->x - av->x );

	outNextVector2Pptr->y =  av->y + t * ( bv->y - av->y );
	outNextVector2Pptr->w =  av->w + t * ( bv->w - av->w );

			//
			// calculate the 3d parametric co-efficient
			// and split 3d values  (z, u,v,l )
			//		( we must maintain the 3d x,y values because we shall split on Y next
			//

	float Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	float Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );

	t = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( (bv->x3d - av->x3d) * Rw - (bv->y3d - av->y3d) * Rh );

	outNextVector2Pptr->x3d = av->x3d + t * ( bv->x3d - av->x3d );	
	outNextVector2Pptr->y3d = av->y3d + t * ( bv->y3d - av->y3d );	
	outNextVector2Pptr->z3d = av->z3d + t * ( bv->z3d - av->z3d );	

	//outNextVector2Pptr->w =   RECIPROCAL( outNextVector2Pptr->z3d );			// TEMP .. change to sim. to above

	outNextNonGeoptr->u = anv->u + t * ( bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t * ( bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t * ( bv->l - av->l );

		
			// We actually DO need to calculate the partial SC mask in Y since Y-clipping is done AFTER x-clipping
			// (we dont need to do the X mask)

	outNextVector2Pptr->SCmaskY = (( outNextVector2Pptr->y < displayDeltaY ) ? 1 : ( (outNextVector2Pptr->y > displayHM1deltaY) ? 2 : 0 ));

	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
	
};


//================================================================================================================================
// When an edge (2D) needs splitting againsty two X values this is more efficient than two calls
// to 'splitEdgeX2D'
//================================================================================================================================

void engine::doubleSplitEdgeX2D(  pointdata2d * r0,  pointdata2d * r1,
								  pointdata2d * a, pointdata2d * b, 
								  float splitx0, float splitx1 )
{
			
//	splitEdgeX2D( r0, a, b, splitx0);
//	splitEdgeX2D( r1, a, b ,splitx1);
//	return;

	float t0, t1, r, ud, vd, ld, yd, zd, wd;
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;
			
	vector2P    * outNextVector2Pptr1 = outNextVector2Pptr + 1;
	nongeodata  * outNextNonGeoptr1   = outNextNonGeoptr + 1;

	r = RECIPROCAL( bv->x - av->x );

	t0 = (  (outNextVector2Pptr->x = splitx0) - av->x) * r;		// parametric coeff at splitx0
	t1 = (  (outNextVector2Pptr1->x = splitx1) - av->x) * r;	// parametric coeff at splitx1

	yd = bv->y - av->y; 
	ud = bnv->u - anv->u;
	vd = bnv->v - anv->v;
	ld = bv->l - av->l;

	zd = bv->z3d - av->z3d;
	wd = bv->w   - av->w;

	outNextVector2Pptr->y  =  av->y + t0 * yd;
	outNextVector2Pptr1->y =  av->y + t1 * yd;
	outNextVector2Pptr->w  =  av->w + t0 * wd;
	outNextVector2Pptr1->w =  av->w + t1 * wd;

	float bax, bay, baz, bngu, bngv, bngl, Rw, Rh;

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );
	t0 = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( (bax = bv->x3d - av->x3d) * Rw - (bay = bv->y3d - av->y3d) * Rh );

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr1->y );
	Rh = projcoeffH*( outNextVector2Pptr1->x - displayWd2deltaX );
	t1 = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( bax * Rw - bay * Rh );


	outNextVector2Pptr->x3d = av->x3d + t0 * ( bax = bv->x3d - av->x3d );	
	outNextVector2Pptr->y3d = av->y3d + t0 * ( bay = bv->y3d - av->y3d );	
	outNextVector2Pptr->z3d = av->z3d + t0 * ( baz = bv->z3d - av->z3d );	

	//outNextVector2Pptr->w =   RECIPROCAL( outNextVector2Pptr->z3d );			// TEMP .. change to sim. to above

	outNextNonGeoptr->u = anv->u + t0 * ( bngu = bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t0 * ( bngv = bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t0 * ( bngl = bv->l - av->l );

	outNextVector2Pptr1->x3d = av->x3d + t1 * bax;	
	outNextVector2Pptr1->y3d = av->y3d + t1 * bay;	
	outNextVector2Pptr1->z3d = av->z3d + t1 * baz;	

	//outNextVector2Pptr1->w =   RECIPROCAL( outNextVector2Pptr1->z3d );			// TEMP .. change to sim. to above

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
	
	vector2P * av = a->v;
	vector2P * bv = b->v;
	nongeodata * anv = a->nv;
	nongeodata * bnv = b->nv;

			// calculate parametric co-efficient of line at split

	t = (  (outNextVector2Pptr->y = splity) - av->y) * RECIPROCAL( bv->y - av->y );

		//	calculate x at split point

	outNextVector2Pptr->x =  av->x + t * ( bv->x - av->x );
	outNextVector2Pptr->w = av->w + t * ( bv->w - av->w );


			//
			// calculate the 3d parametric co-efficient
			// and split 3d values  (z, u,v,l )
			//		( we must maintain the 3d x,y values because we shall split on Y next
			//

	float Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	float Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );

	t = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( (bv->x3d - av->x3d) * Rw - (bv->y3d - av->y3d) * Rh );

	outNextVector2Pptr->x3d = av->x3d + t * ( bv->x3d - av->x3d );	
	outNextVector2Pptr->y3d = av->y3d + t * ( bv->y3d - av->y3d );	
	outNextVector2Pptr->z3d = av->z3d + t * ( bv->z3d - av->z3d );	

	outNextNonGeoptr->u = anv->u + t * ( bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t * ( bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t * ( bv->l - av->l );


			// NOTE that we have no need for setting local SCmaskX or SCmaskY here..


	r->v  = outNextVector2Pptr++;
	r->nv = outNextNonGeoptr++;
}


//================================================================================================================================
// When an edge (2D) needs splitting againsty two Y values this is more efficient than two calls
// to 'splitEdgeY2D'
//================================================================================================================================

void engine::doubleSplitEdgeY2D(  pointdata2d * r0,  pointdata2d * r1,
								  pointdata2d * a, pointdata2d * b, 
								  float splity0, float splity1 )
{
			
//	splitEdgeY2D( r0, a, b, splity0);
//	splitEdgeY2D( r1, a, b ,splity1);
//	return;


	float t0, t1, r, ud, vd, ld, xd, zd, wd;
	
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

	zd = bv->z3d - av->z3d;  
	wd = bv->w - av->w;  

	xd = bv->x - av->x;  
	ud = bnv->u - anv->u;
	vd = bnv->v - anv->v;
	ld = bv->l - av->l;


	outNextVector2Pptr->x  = av->x + t0 * xd;
	outNextVector2Pptr1->x = av->x + t1 * xd;
	outNextVector2Pptr->w  = av->w + t0 * wd;
	outNextVector2Pptr1->w = av->w + t1 * wd;

	float bax, bay, baz, bngu, bngv, bngl, Rw, Rh;

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr->y );
	Rh = projcoeffH*( outNextVector2Pptr->x - displayWd2deltaX );
	t0 = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( (bax = bv->x3d - av->x3d) * Rw - (bay = bv->y3d - av->y3d) * Rh );

	Rw = projcoeffW*( displayHd2deltaY - outNextVector2Pptr1->y );
	Rh = projcoeffH*( outNextVector2Pptr1->x - displayWd2deltaX );
	t1 = ( Rh * av->y3d - Rw * av->x3d )  * RECIPROCAL( bax * Rw - bay * Rh );


	outNextVector2Pptr->x3d = av->x3d + t0 * ( bax = bv->x3d - av->x3d );	
	outNextVector2Pptr->y3d = av->y3d + t0 * ( bay = bv->y3d - av->y3d );	
	outNextVector2Pptr->z3d = av->z3d + t0 * ( baz = bv->z3d - av->z3d );	


	outNextNonGeoptr->u = anv->u + t0 * ( bngu = bnv->u - anv->u );
	outNextNonGeoptr->v = anv->v + t0 * ( bngv = bnv->v - anv->v );
	outNextVector2Pptr->l = av->l + t0 * ( bngl = bv->l - av->l );

	outNextVector2Pptr1->x3d = av->x3d + t1 * bax;	
	outNextVector2Pptr1->y3d = av->y3d + t1 * bay;	
	outNextVector2Pptr1->z3d = av->z3d + t1 * baz;	


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
	outputTriangle( &Ysplit2, &Ysplit3, &Ysplit0);\
	outputTriangle( &Ysplit0, &Ysplit1, &Ysplit2);
#define SY4_B(a,b, c,d )\
	doubleSplitEdgeY2D( &Ysplit0, &Ysplit1,  c, d, displayHM1deltaY, displayDeltaY );\
	doubleSplitEdgeY2D( &Ysplit2, &Ysplit3,  a, b, displayDeltaY, displayHM1deltaY );\
	outputTriangle( &Ysplit2, &Ysplit3, &Ysplit0);\
	outputTriangle( &Ysplit0, &Ysplit1, &Ysplit2);

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




#define SZ4_A(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, ZFRONTPLANE, currentcamera->zbackplane);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, currentcamera->zbackplane, ZFRONTPLANE);\
	Wprocess2DTriangle( &split2, &split3, &split0);\
	Wprocess2DTriangle( &split0, &split1, &split2);
#define SZ4_B(a,b, c,d )\
	doubleSplitEdgeZ( &split0, &split1,  c, d, currentcamera->zbackplane, ZFRONTPLANE);\
	doubleSplitEdgeZ( &split2, &split3,  a, b, ZFRONTPLANE, currentcamera->zbackplane);\
	Wprocess2DTriangle( &split2, &split3, &split0);\
	Wprocess2DTriangle( &split0, &split1, &split2);



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
					switch( mask )
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
					switch( mask )
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
};




//void engine::processTriangle( triangle * triPtr )

//================================================================================================================================
// - process a hunk which has been found to be at least partially visible
//================================================================================================================================


void engine::renderthisHunk( triangle **thistlist, ulong total)
{
		static   vertex   * pvPtr, * qvPtr, * rvPtr;
		long kk;

		for (kk = total; kk--; thistlist++)
		{

			    triPtr = *thistlist;
				if( triPtr->pcnt != globalpctr )		// if not already examined (another hunk)
				{


					triPtr->pcnt = globalpctr;			// flag the triangle as 'used' so we won't reuse it from another hunk

					currentmtl = triPtr->mtl;

						//
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//

					pvPtr = triPtr->p.v;
					if( pvPtr->tcnt != globalpctr )		// not yet transformed 
					{
						pvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &pvPtr->transformed, &pvPtr->geoCoord );
						pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					};

					qvPtr = triPtr->q.v;
					if( qvPtr->tcnt != globalpctr )	
					{
						qvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &qvPtr->transformed, &qvPtr->geoCoord );
						qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					};

					rvPtr = triPtr->r.v;
					if( rvPtr->tcnt != globalpctr )		
					{
						rvPtr->tcnt = globalpctr;
						currentcamera->transformWorldVector( &rvPtr->transformed, &rvPtr->geoCoord );
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

					zsplitting();

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

void engine::processObjectLit( object * objPtr )
{
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	ulong t;

	static vector3 tempvector;

		
			// - examine the model (if not 0) and transform every point by the local transform matrix
			//   and add on the position in the world!


			
			
		myObject = objPtr;
		while( myObject)		// for object and each of its sister objects
		{
				
		 if( myObject->child != 0)
		 	processObjectLit(myObject->child);

  		 if(myObject->visible)
		 {
			catMat = &myObject->concatenatedTransform;
			catMat->translation -= currentcamera->position;
			WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
			catMat->translation += currentcamera->position;

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


					//bumf++;						

					for( t = myObject->modelData->meshData.tcount; t--; )
					{

						currentmtl = triPtr->mtl;

						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					 
						pvPtr = triPtr->p.v;
						qvPtr = triPtr->q.v;
						rvPtr = triPtr->r.v;


						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &pvPtr->transformed, &pvPtr->geoCoord );
								catMat->multiplyV3By3x3mat( &tempvector , &pvPtr->vertexNormal);
								pvPtr->l = tempvector.DotProduct( &currentworld->sunLightDirection );//  * 0.05f;
								if( pvPtr->l < 0 ) pvPtr->l = 0;
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &qvPtr->transformed, &qvPtr->geoCoord );
								catMat->multiplyV3By3x3mat( &tempvector , &qvPtr->vertexNormal);
								qvPtr->l = tempvector.DotProduct( &currentworld->sunLightDirection );//  * 0.05f;
								if( qvPtr->l < 0 ) qvPtr->l = 0;
							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &rvPtr->transformed, &rvPtr->geoCoord );
								catMat->multiplyV3By3x3mat( &tempvector , &rvPtr->vertexNormal);
								rvPtr->l = tempvector.DotProduct( &currentworld->sunLightDirection );//  * 0.05f;
								if( rvPtr->l < 0 ) rvPtr->l = 0;
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
						   if( rvPtr->pcnt != globalpctr )		// not yet projected 
						   {	
								rvPtr->pcnt = globalpctr;
								project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
						   };
						   projectedr.v = rvPtr->projectedPtr;
						};
						
						zsplitting();
						triPtr++;
	
					};

					}	// if (ok)
				  
				}; // if (checkRboundboxVisibility

			};  // if(objPtr->modeldata != 0)
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
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
	static   vertex   * pvPtr, * qvPtr, * rvPtr;
	ulong t;

	static vector3 tempvector;

		
			// - examine the model (if not 0) and transform every point by the local transform matrix
			//   and add on the position in the world!


			
			
		myObject = objPtr;
		while( myObject)		// for object and each of its sister objects
		{
				
		  if( myObject->child != 0)
				processObjectUnlit(myObject->child);

		  if(myObject->visible)
          {
			catMat = &myObject->concatenatedTransform;

			catMat->translation -= currentcamera->position;
			WCtransform.multiply3x4mats( &currentcamera->RotMatrix, catMat );
			catMat->translation += currentcamera->position;

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


					//bumf++;						

					for( t = myObject->modelData->meshData.tcount; t--; )
					{

						currentmtl = triPtr->mtl;

						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
						// Initially transform the vertex geometric coordinate IF IT ISN'T ALREADY TRANSFORMED!!!
						//- - - - - - - - - - - - - - - - - - - - - - - - - - - -
					 
						pvPtr = triPtr->p.v;
						qvPtr = triPtr->q.v;
						rvPtr = triPtr->r.v;


						if( pvPtr->tcnt != globalpctr )		// not yet transformed 
						{
							pvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &pvPtr->transformed, &pvPtr->geoCoord );
							pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( qvPtr->tcnt != globalpctr )	
						{
							qvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &qvPtr->transformed, &qvPtr->geoCoord );
							qvPtr->SCmaskInZ = (qvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((qvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
						};
						if( rvPtr->tcnt != globalpctr )		
						{
							rvPtr->tcnt = globalpctr;
							WCtransform.multiplyV3By3x4mat( &rvPtr->transformed, &rvPtr->geoCoord );
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
						   if( rvPtr->pcnt != globalpctr )		// not yet projected 
						   {	
								rvPtr->pcnt = globalpctr;
								project( (rvPtr->projectedPtr = outNextVector2Pptr++), &rvPtr->transformed, rvPtr->l );
						   };
						   projectedr.v = rvPtr->projectedPtr;
						};
						
						zsplitting();
						triPtr++;
	
					};

					}	// if (ok)
				  
				}; // if (checkRboundboxVisibility

			};  // if(objPtr->modeldata != 0)
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


	slong firstXhunk = 0;
	slong lastXhunk = currentworld->xhunks-1; 
	slong firstZhunk = 0;
	slong lastZhunk = currentworld->zhunks-1; 

	zhunkcount= 1+lastZhunk - firstZhunk;


	hunk * thisHunkX = currentworld->hunkList + firstXhunk * currentworld->zhunks;		
		

	float MinY = currentworld->meshData.MinCoordY - currentcamera->position.y;
	float MaxY = currentworld->meshData.MaxCoordY - currentcamera->position.y;


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
	
    xhunkcoord = currentworld->meshData.MinCoordX  -  currentcamera->position.x;
    zhunkcoord = currentworld->meshData.MinCoordZ  -  currentcamera->position.z;

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
	process2DTriangleY( &Xsplit2, &Xsplit3, &Xsplit0);\
	process2DTriangleY( &Xsplit0, &Xsplit1, &Xsplit2);
#define SX4_B(a,b, c,d )\
	doubleSplitEdgeX2D( &Xsplit0, &Xsplit1,  c, d, displayWM1deltaX, displayDeltaX );\
	doubleSplitEdgeX2D( &Xsplit2, &Xsplit3,  a, b, displayDeltaX, displayWM1deltaX );\
	process2DTriangleY( &Xsplit2, &Xsplit3, &Xsplit0);\
	process2DTriangleY( &Xsplit0, &Xsplit1, &Xsplit2);

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

ulong combineColour( ulong a, ulong i, float l )
{
	slong t;

	FASTFTOL( &t,  (((float)(i)) * l) );
	t += a;
	if( t > 255) return(255);
	return(t);

}


			
void engine::performRender()
{ 

				//
				// ...... sorting/grouping operations on triangle list
				//

		outTriangle * tlist = currentOutListPtr->triangleList;
		outTriangle * llist = outTriangleNextPtr;
		sortlistentry * sortlistPtr = currentOutListPtr->sortlistPtr;
		ulong  zmask;
		ulong  count;

		if( tlist != llist)		// Not empty list
		{
		  if( engine_flags & EFLAGS_SOFTWARE  )
		  {

				// - - - - - - - - - - - - -
				//
				// SOFTWARE RENDERING : sort the triangles by mid-z
				//			note that the scaling value used in projection from 3d to 2d to
				//			get the appropriate z value for association with the 2d x,y coord.
				//			is ONE THIRD of the value we actually want for casting it to range.
				//			This means that we do not need to multiply the sum of the 'z' values
				//			by 0.333333 here!! (saves us a multiply...)
				//
				// - - - - - - - - - - - - -

					// first triangle.. zmask set to intial value

			  FASTFTOL(	(long * )&zmask,	tlist->p.v->z3d + 
							    tlist->q.v->z3d + 
							    tlist->r.v->z3d );

			  sortlistPtr->key = zmask;
			  (sortlistPtr++)->idx= (ulong) (tlist++);

					// other triangles

			  while( tlist != llist )
			  {
					FASTFTOL( (long *)&sortlistPtr->key, tlist->p.v->z3d + 
												 tlist->q.v->z3d + 
												 tlist->r.v->z3d );
					zmask |=  (zmask ^ sortlistPtr->key);
					(sortlistPtr++)->idx= (ulong) (tlist++);
			  };
			  
			 
		  }
		  else
		  {
					// - - - - - - - - - - - - -
					// HARDWARE RENDERING : sort the output list triangles according to material.
					// - - - - - - - - - - - - -


				zmask = sortlistPtr->key= tlist->mtl->sortingKey;
				(sortlistPtr++)->idx= (ulong) (tlist++);
			 

					// other triangles
			  while( tlist != llist )
			  {
						zmask |=  (zmask ^  (sortlistPtr->key = tlist->mtl->sortingKey) );
						(sortlistPtr++)->idx= (ulong) (tlist++);
			  }

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
			ulong total;
			colourComponents diffuseColour, ambientColour;

			vector2P * pvPtr, * qvPtr, * rvPtr;
			nongeodata * npvPtr, * nqvPtr, * nrvPtr;
				
			material * currentSelectedMaterial = NULL;

			tnum = 0;


if( lightSwitch )
{
			while( tnum < count )
			{
				total = 0;
				testVertex = vertexListD3D;

			    while( (tnum < count) && ( total < BATCHSIZEM3) )
				{

					tlist = (outTriangle * )slist->idx;		

					if( tlist->mtl != currentSelectedMaterial )	
					{
						currentSelectedMaterial = tlist->mtl;

						if(total)
							renderList(vertexListD3D, total, wframe);
				
						renderSetCurrentMaterial(&currentSelectedMaterial->bm);

						ambientColour = currentworld->sunAmbientColour;
						diffuseColour = currentworld->sunIncidentColour;

						if( currentSelectedMaterial->bm.textureHandle == NULL)
						{
							ambientColour = //currentSelectedMaterial->diffusec;
							diffuseColour = currentSelectedMaterial->diffusec;//currentworld->sunIncidentColour;
						};


						//specularColour = currentSelectedMaterial->specular;

						// Start a new list

						total = 0;
						testVertex = vertexListD3D;
					};


					pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
					qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
					rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;


					testVertex->sx  =  pvPtr->x ;
					testVertex->sy  =  pvPtr->y ;
					testVertex->sz  =  pvPtr->z3d;
					testVertex->rhw =  pvPtr->w;
					testVertex->tu  = npvPtr->u ;
					testVertex->tv  = npvPtr->v ;

					testVertex->color = 
							( combineColour(ambientColour.red,diffuseColour.red,pvPtr->l ) << 16) +
							( combineColour(ambientColour.grn,diffuseColour.grn,pvPtr->l) << 8 ) +
							( combineColour(ambientColour.blu,diffuseColour.blu,pvPtr->l ) );

					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;

					testVertex->sx  =  qvPtr->x ;
					testVertex->sy  =  qvPtr->y ;
					testVertex->sz  =  qvPtr->z3d ;
					testVertex->rhw =  qvPtr->w;
					testVertex->tu  = nqvPtr->u ;
					testVertex->tv  = nqvPtr->v ;

					testVertex->color = 
						( combineColour(ambientColour.red,diffuseColour.red,qvPtr->l ) << 16) +
						( combineColour(ambientColour.grn,diffuseColour.grn,qvPtr->l) << 8 ) +
						( combineColour(ambientColour.blu,diffuseColour.blu,qvPtr->l ) );

					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;

					testVertex->sx  =  rvPtr->x ;
					testVertex->sy  =  rvPtr->y ;
					testVertex->sz  =  rvPtr->z3d ;
					testVertex->rhw =  rvPtr->w;
					testVertex->tu  = nrvPtr->u ;
					testVertex->tv  = nrvPtr->v ;

					testVertex->color = 
						( combineColour(ambientColour.red,diffuseColour.red,rvPtr->l ) << 16) +
						( combineColour(ambientColour.grn,diffuseColour.grn,rvPtr->l) << 8 ) +
						( combineColour(ambientColour.blu,diffuseColour.blu,rvPtr->l ) );


					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;
			
					tnum++;
					slist++;
					total+=3;
				};

						// BATCHSIZE REACHES OR ALL TRIANGLES REACHED
				if(total)
					renderList(vertexListD3D, total, wframe);
			};

}
else
{
			ulong outcolour;

			while( tnum < count )
			{
				total = 0;
				testVertex = vertexListD3D;

			    while( (tnum < count) && ( total < BATCHSIZEM3) )
				{

					tlist = (outTriangle * )slist->idx;		

					if( tlist->mtl != currentSelectedMaterial )	
					{
						currentSelectedMaterial = tlist->mtl;

						if(total)
							renderList(vertexListD3D, total, wframe);
				
						renderSetCurrentMaterial(&currentSelectedMaterial->bm);
	
						outcolour = currentSelectedMaterial->diffuse;

							// Start a new list

						total = 0;
						testVertex = vertexListD3D;
					};


					pvPtr = tlist->p.v; npvPtr = tlist->p.nv;
					qvPtr = tlist->q.v; nqvPtr = tlist->q.nv;
					rvPtr = tlist->r.v; nrvPtr = tlist->r.nv;


					testVertex->sx  =  pvPtr->x ;
					testVertex->sy  =  pvPtr->y ;
					testVertex->sz  =  pvPtr->z3d;
					testVertex->rhw =  pvPtr->w;
					testVertex->tu  = npvPtr->u ;
					testVertex->tv  = npvPtr->v ;

					testVertex->color = outcolour;

					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;

					testVertex->sx  =  qvPtr->x ;
					testVertex->sy  =  qvPtr->y ;
					testVertex->sz  =  qvPtr->z3d ;
					testVertex->rhw =  qvPtr->w;
					testVertex->tu  = nqvPtr->u ;
					testVertex->tv  = nqvPtr->v ;

					testVertex->color = outcolour;

					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;

					testVertex->sx  =  rvPtr->x ;
					testVertex->sy  =  rvPtr->y ;
					testVertex->sz  =  rvPtr->z3d ;
					testVertex->rhw =  rvPtr->w;
					testVertex->tu  = nrvPtr->u ;
					testVertex->tv  = nrvPtr->v ;

					testVertex->color = outcolour;

					//testVertex->color = diffuseColour;
						//testVertex->specular = specularColour;
					testVertex++;
			
					tnum++;
					slist++;
					total+=3;
				};

						// BATCHSIZE REACHES OR ALL TRIANGLES REACHED
				if(total)
					renderList(vertexListD3D, total, wframe);
			};
}




	}  // if (tlist

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



void engine::render()
{

		//
		// render according to what has been set
		//


	
		// construct the geometry lists

		
	

		globalpctr++;


	
		outNextVector2Pptr =  currentOutListPtr->vector2PList;
		outNextNonGeoptr   =  currentOutListPtr->nongeoList;
		outTriangleNextPtr =  currentOutListPtr->triangleList;

		STARTSTAT(2, 0,255,0 );
		renderWorld();
		ENDSTAT(2);

		renderSpriteFXlist();

		STARTSTAT(4, 255,0,0);
		performRender();
		ENDSTAT(4);
		outNextVector2Pptr =  currentOutListPtr->vector2PList;
		outNextNonGeoptr   =  currentOutListPtr->nongeoList;
		outTriangleNextPtr =  currentOutListPtr->triangleList;

		STARTSTAT(3, 0,255,0 );
		renderObjects();
		ENDSTAT(3);

	
		STARTSTAT(5, 255,0,0);
		performRender();
		ENDSTAT(5);


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
	optr->prev = 0;
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
	if( optr->prev != 0)
		optr->prev->next = optr->next;
	if( optr->next != 0)
		optr->next->prev = optr->prev;
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


void engine::beginFrame( bool moved, LPARAM heldlParam)
{
		renderBeginScene( TRUE);
		if( moved )			
			renderMove(heldlParam);
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




void  engine::appendSpriteFX( spriteFX * sfx, vector3 * position )
{
    spriteFXentry * newEntry = new spriteFXentry;

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

	vertex   * pvPtr, * qvPtr, * rvPtr;

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
	
			
					currentmtl = &thisFXentry->spriteFXptr->mat;

					triPtr = &thisFXentry->triangleA; //new triangle;

					pvPtr = triPtr->p.v; 
					tempcoord = thisFXentry->worldPosition;
					tempcoord.x -= thisFrame->halfxlength;
					tempcoord.y -= thisFrame->halfylength;
					currentcamera->transformWorldVectorSprite( &pvPtr->transformed, &tempcoord );

					//pvPtr->transformed = tempcoord;// - currentcamera->position;


			//tempcoord -= currentcamera->position;
			//pvPtr->transformed = tempcoord;

					pvPtr->SCmaskInZ = (pvPtr->transformed.z < ZFRONTPLANE) ? 2 : ((pvPtr->transformed.z > currentcamera->zbackplane) ? 1: 0);
					
					qvPtr = triPtr->q.v;
					oldrvPtr = rvPtr = triPtr->r.v;

					rvPtr->transformed.x =
					qvPtr->transformed.x = pvPtr->transformed.x + thisFrame->xlength;

					qvPtr->transformed.y = pvPtr->transformed.y;

					rvPtr->transformed.y = pvPtr->transformed.y + thisFrame->ylength;

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
					//currentmtl = &thisFXentry->spriteFXptr->mat;

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

//================================================================================================================================
// END OF FILE
//================================================================================================================================
