//================================================================================================================================
// world.cpp
// ---------
//
//================================================================================================================================

#include "BS2all.h"



world::world( char * filename, float scale,
			  float hunksizex, float hunksizez  )
{
	hunk * hunkPtr;
	slong i;
	slong loHunkX, hiHunkX,  loHunkZ, hiHunkZ; 

	slong xx, zz;
	hunk  * hunkPtrX,  * hunkPtrZ;
	float hx, hz;
	vector3 * pcoord, * qcoord, * rcoord;
//	overlaytriangle ** OvlPtrX, ** OvlPtrZ;

	triangle * thisTrianglePtr;


	xhunksize = hunksizex * scale; 
	zhunksize = hunksizez * scale;
	oneoverxhunksize = RECIPROCAL( xhunksize );
	oneoverzhunksize = RECIPROCAL( zhunksize );

	//loadMulderFile( filename, &meshData, scale );
	worldModel = getModelPtr( filename, scale, false, "" );

	if(boris2->useRacingline)
		rLine = new racingLine( filename, scale );


		//
		// each overlay point (px,pz) has a coord given by:
		//
		//				worldMinCoordX +  px * mapXsize,
		//				worldMinCoordZ +  pz * mapZsize;
		// 
		// and a pointer to the appropriate triangle by:
		//
		// overlayMap[pz * mapXcount + px] 
		//
		//
		// in overlaytriangle:  Avalue = -A/B   
		//						Cvalue = -C/B
		//						Dvalue = -D/B




		//
		// create the hunk lists of triangles
		//

	float minx, maxx, minz, maxz;

		// allocate a sufficient number of hunks for the world

		// calculate number of XZ hunks required to cover the world

	mesh * meshData = &worldModel->meshData;

	xhunks =  (slong)( (meshData->MaxCoordX - meshData->MinCoordX) * oneoverxhunksize) + 1;
	zhunks =  (slong)( (meshData->MaxCoordZ - meshData->MinCoordZ) * oneoverzhunksize) + 1;
	
	hunkPtr = hunkList = new hunk[ totalhunks = xhunks * zhunks ];
					// clear the counts in all hunks to 0 initially

	for( i = totalhunks; i--; )
	{
			hunkPtr->tcount_W = hunkPtr->tcount_N = 0;
			hunkPtr++;
	};

		//
		// PASS 1:
		// examine each triangle, increment the count of every appropriate hunk list
		//

	thisTrianglePtr = meshData->tlistPtr;
	for (i = meshData->tcount; i--; )
	{
		bool wallTriangle = ( (thisTrianglePtr->mtl->bm.flags & MAT_WALL) != 0);

		pcoord = &thisTrianglePtr->p.v->geoCoord;
		qcoord = &thisTrianglePtr->q.v->geoCoord;
		rcoord = &thisTrianglePtr->r.v->geoCoord;

			// Find the XZ bound of this triangle
	
		minmax3( pcoord->x,
				 qcoord->x,
				 rcoord->x, &minx, &maxx );

		minmax3( pcoord->z,
				 qcoord->z,
				 rcoord->z, &minz, &maxz );

			// get range of hunks whuch encompass this triangles bounds

	
		loHunkX = (slong)((minx - meshData->MinCoordX) * oneoverxhunksize );
		hiHunkX = (slong)((maxx - meshData->MinCoordX) * oneoverxhunksize );

		loHunkZ = (slong)((minz - meshData->MinCoordZ) * oneoverzhunksize) ;
		hiHunkZ = (slong)((maxz - meshData->MinCoordZ) * oneoverzhunksize) ;
		
		if( hiHunkX >= xhunks) hiHunkX = xhunks-1;
		if( hiHunkZ >= zhunks) hiHunkZ = zhunks-1;


				// test current triangle against every hunk in its bounding rectangle, if it is visible then
				// simply increment the count here....
		
		hx = loHunkX * xhunksize + meshData->MinCoordX ; //minx;
		hunkPtrX = hunkList + loHunkX*zhunks;
		for( xx = 1 + hiHunkX - loHunkX; xx--; )
		{
		
		    if((((pcoord->x < hx) ? 1 : ((pcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
		  	    ((qcoord->x < hx) ? 1 : ((qcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
				((rcoord->x < hx) ? 1 : ((rcoord->x >= (hx+xhunksize)) ? 2 : 0 )) ) == 0)
			{
				
				hz = loHunkZ * zhunksize + meshData->MinCoordZ; //minz;
				hunkPtrZ = hunkPtrX + loHunkZ;
				for(  zz = 1 + hiHunkZ - loHunkZ; zz--; )
				{
				   if((((pcoord->z < hz) ? 1 : ((pcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
		  			   ((qcoord->z < hz) ? 1 : ((qcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
					   ((rcoord->z < hz) ? 1 : ((rcoord->z >= (hz+zhunksize)) ? 2 : 0 )) ) == 0)	
					{
								// hunkPtrZ = pointer to hunk which (partially) contains the current triangle
								// so increment its counter.

							if( wallTriangle )
								hunkPtrZ->tcount_W++;
							else
								hunkPtrZ->tcount_N++;
					};
					hz += zhunksize;
					hunkPtrZ++;
				}

			};
			hx += xhunksize;
			hunkPtrX += zhunks;
		};

		thisTrianglePtr++;
	};


		//
		//	Allocate local hunk lists
		//

	hunkPtr = hunkList;
	for( i = totalhunks; i--; )
	{
		//if (hunkPtr->tcount)
			//DBGR.Dprintf("%f ... %f ",i, (float)hunkPtr->tcount);

		hunkPtr->tlistPtr_N = new triangle *[hunkPtr->tcount_N];
		hunkPtr->tlistPtr_W = new triangle *[hunkPtr->tcount_W];

		hunkPtr->nextTriangle_N = hunkPtr->tlistPtr_N;// point one after end (for loop algorithms)
		hunkPtr->nextTriangle_W = hunkPtr->tlistPtr_W;// point one after end (for loop algorithms)
		hunkPtr++;
	};


		//
		// PASS2:
		//			As PASS1 but actually attach the triangle
		//
	

	thisTrianglePtr = meshData->tlistPtr;
	for (i = meshData->tcount; i--; )
	{

		bool wallTriangle = ( (thisTrianglePtr->mtl->bm.flags & MAT_WALL) != 0);

		pcoord = &thisTrianglePtr->p.v->geoCoord;
		qcoord = &thisTrianglePtr->q.v->geoCoord;
		rcoord = &thisTrianglePtr->r.v->geoCoord;

			// Find the XZ bound of this triangle
	
		minmax3( pcoord->x,
				 qcoord->x,
				 rcoord->x, &minx, &maxx );

		minmax3( pcoord->z,
				 qcoord->z,
				 rcoord->z, &minz, &maxz );

			// get range of hunks whuch encompass this triangles bounds

		loHunkX = (slong)((minx - meshData->MinCoordX) * oneoverxhunksize) ;
		hiHunkX = (slong)((maxx - meshData->MinCoordX) * oneoverxhunksize ) + 1;

		loHunkZ = (slong)((minz - meshData->MinCoordZ) * oneoverzhunksize) ;
		hiHunkZ = (slong)((maxz - meshData->MinCoordZ) * oneoverzhunksize) + 1;

		if( hiHunkX >= xhunks) hiHunkX = xhunks-1;
		if( hiHunkZ >= zhunks) hiHunkZ = zhunks-1;


				// test current triangle against every hunk in its bounding rectangle, if it is visible then
				// simply increment the count here....


		hx = loHunkX*xhunksize + meshData->MinCoordX; // minx
		hunkPtrX = hunkList + loHunkX*zhunks;
		for( xx = 1 + hiHunkX - loHunkX; xx--; )
		{
		
		    if((((pcoord->x < hx) ? 1 : ((pcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
		  	    ((qcoord->x < hx) ? 1 : ((qcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
				((rcoord->x < hx) ? 1 : ((rcoord->x >= (hx+xhunksize)) ? 2 : 0 )) ) == 0)
			{
				
				hz = loHunkZ*zhunksize + meshData->MinCoordZ; //minz;
				hunkPtrZ = hunkPtrX + loHunkZ;
				for( zz = 1 + hiHunkZ - loHunkZ; zz--; )
				{
				   if((((pcoord->z < hz) ? 1 : ((pcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
		  			   ((qcoord->z < hz) ? 1 : ((qcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
					   ((rcoord->z < hz) ? 1 : ((rcoord->z >= (hz+zhunksize)) ? 2 : 0 )) ) == 0)	
					{
					
								// hunkPtrZ = pointer to hunk which (partially) contains the current triangle
								// so place the pointer to the triangle in the hunks list of pointers to triangles

							if(wallTriangle)
								*(hunkPtrZ->nextTriangle_W++) = thisTrianglePtr;
							else
								*(hunkPtrZ->nextTriangle_N++) = thisTrianglePtr;
					};
					hz += zhunksize;
					hunkPtrZ++;
				}

			};
			hx += xhunksize;
			hunkPtrX += zhunks;
		};

			//
			// Phew..
			//

		thisTrianglePtr++;
	};

	sunOn = false;

}

//
//
//

bool  yfound;
float bestycoord;
uchar bestyside;
triangle * bestyTri;


#define UPTOLERANCE 6.0f

bool world::scanTrianglesForXZ( triangle **thistlist, ulong total, vector3 * v, float * yresult, triangle ** resulttriPtr)
{
	//static   vertex   * pvPtr, * qvPtr, * rvPtr;
	long kk;

	triangle * triPtr;
	vector3 * pcoord, *qcoord, * rcoord;
	bool  tcondn;
	float ycoord;
	uchar yside;


	for (kk = total; kk--; thistlist++)
	{
	    triPtr = *thistlist;

		if(  (triPtr->minLimit.x <= v->x) && (triPtr->maxLimit.x >= v->x) &&
			 (triPtr->minLimit.z <= v->z) && (triPtr->maxLimit.z >= v->z)   
		)
		{

				if( (triPtr->faceNormal.y )	&& ( (triPtr->mtl->bm.flags & MAT_WATER) == 0 )) // triangle plane is NOT vertical, nor water
				{

					
						// point is within bounding box of triangle so now lets test with 2x2 determinants to see if the
						// XZ point is within the droppped XZ triangle.

					pcoord = &triPtr->p.v->geoCoord;
					qcoord = &triPtr->q.v->geoCoord;
					rcoord = &triPtr->r.v->geoCoord;

					float Xtp = pcoord->x - v->x;
					float Ztp = pcoord->z - v->z;
					float Xup = qcoord->x - v->x;
					float Zup = qcoord->z - v->z;
					float Xvp = rcoord->x - v->x;
					float Zvp = rcoord->z - v->z;
			
					if( Xtp*Zup  >  Xup*Ztp )
					{
						tcondn = ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
					} else
					{
						tcondn = ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
					}

					if(tcondn)
					{
						
					
						ycoord    = (float)(triPtr->minusOneOverB * (
								    triPtr->A * v->x +
								    triPtr->C * v->z +
								    triPtr->planeD ));

	

			yside = ( (v->y + UPTOLERANCE) >= ycoord ) ? 1 : 2;
						if( yfound )
						{			
										// .. we already have a value held.
							if( yside == bestyside )
							{
										// both on same side 
										
								 if( yside == 1)
								 {
										if(  ycoord > bestycoord )
										{
											bestycoord = ycoord;
											bestyside  = yside;
											bestyTri   = triPtr;
											yfound     = true;		
										}
								 } else
								 {
										if(  ycoord < bestycoord )
										{
											bestycoord = ycoord;
											bestyside  = yside;
											bestyTri   = triPtr;
											yfound     = true;		
										}
								 }
							} else
							{
								if( yside == 1 )		// new coord ABOVE, ( and old coord below)
								{
										bestycoord = ycoord;
										bestyside  = yside;
										bestyTri   = triPtr;
										yfound     = true;
								}		// else (old coord ABOVE new below so no change
							}
						} else
						{
							bestycoord = ycoord;
							bestyside  = yside;
							bestyTri   = triPtr;
							yfound = true;
						}
					}
				}
		}
		
	};

	if(yfound)
	{

		*resulttriPtr = bestyTri;//->mtl->bm.flags;
		*yresult = bestycoord;
		return( true );
	};


	return( false );
};

//================================================================================================================================
//================================================================================================================================

bool world::isPointUnderwater( vector3 * v, float * depth, triangle ** resulttriPtr )
{
//return(true);

	slong hunkX, hunkZ;
	hunk * hunkPtr;
	
		// Find the hunk which contains this XZ point..

	mesh * meshData = &worldModel->meshData;

	hunkX = (slong)((v->x - meshData->MinCoordX) * oneoverxhunksize);
	hunkZ = (slong)((v->z - meshData->MinCoordZ) * oneoverzhunksize);
		
	if( hunkX >= xhunks) hunkX = xhunks-1;
	if( hunkZ >= zhunks) hunkZ = zhunks-1;

	hunkPtr = hunkList + hunkX*zhunks + hunkZ;


	long kk;

	triangle * triPtr;
	vector3 * pcoord, *qcoord, * rcoord;
	bool  tcondn;
	float ycoord;

	triangle ** thistlist = hunkPtr->tlistPtr_N;

	if(hunkPtr->tcount_N)
	for (kk = hunkPtr->tcount_N; kk--; thistlist++)
	{
	    triPtr = *thistlist;

		if(  (triPtr->minLimit.x <= v->x) && (triPtr->maxLimit.x >= v->x) &&
			 (triPtr->minLimit.z <= v->z) && (triPtr->maxLimit.z >= v->z)   
		)
		{

				if( (triPtr->mtl->bm.flags & MAT_WATER) != 0 )	// triangle is water!!!!
				{

					
						// point is within bounding box of triangle so now lets test with 2x2 determinants to see if the
						// XZ point is within the droppped XZ triangle.

					pcoord = &triPtr->p.v->geoCoord;
					qcoord = &triPtr->q.v->geoCoord;
					rcoord = &triPtr->r.v->geoCoord;

					float Xtp = pcoord->x - v->x;
					float Ztp = pcoord->z - v->z;
					float Xup = qcoord->x - v->x;
					float Zup = qcoord->z - v->z;
					float Xvp = rcoord->x - v->x;
					float Zvp = rcoord->z - v->z;
			
					if( Xtp*Zup  >  Xup*Ztp )
					{
						tcondn = ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
					} else
					{
						tcondn = ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
					}

					if(tcondn)
					{
						ycoord = (float)(triPtr->minusOneOverB * (
								    triPtr->A * v->x +
								    triPtr->C * v->z +
								    triPtr->planeD ) - v->y);
														
						if	( ycoord >= 0.0f )
						{
							*depth = ycoord;
							*resulttriPtr = triPtr;
							return( true );
						}

					}
				}
		}
		
	};

	return( false );



		// Now examine every triangle in the hunk to find the triangle which this point sits above.
		//
		//

	//yfound = false;
	//if( scanTrianglesForXZ( hunkPtr->tlistPtr_N, hunkPtr->tcount_N, v, yresult, resulttriPtr) )
	//{
	//	return(true) ;
	//};
    //return( scanTrianglesForXZ( hunkPtr->tlistPtr_W, hunkPtr->tcount_W, v, yresult, resulttriPtr) );

};

//================================================================================================================================
//================================================================================================================================

bool world::getShadowCoord( vector3 * v, vector3 * dirn, vector3 * shadowCoord, vector3 * triNormal )
{

	//getY( v,  yresult, resulttriPtr ); 
	//float vlen = (*yresult) - v->y;			// length of direction vector
	//vlen*=3;
	//v->x = v->x + vlen*dirn->x;
	//v->z = v->z + vlen*dirn->z;
	//return(getY( v,  yresult, resulttriPtr )); 
	//*yresult += 2;

 // if( abs(triNormal->y) > 0.7f) return (false );

  bool foundit = false;
	#define vlen 30.0f					// distance to consider for shadow		


  static vector3 r;
  triangle * triPtr;

  static vector3 pulledDirn;

  pulledDirn = *dirn;


  //pulledDirn.y = -0.99f;//*= 100
 pulledDirn.x *= 0.1f;
 pulledDirn.z *= 0.1f;
 pulledDirn.Normalise();
//pulledDirn = vector3(0,-1,0);
  float f = vlen * pulledDirn.x;
  float g = vlen * pulledDirn.y;
  float h = vlen * pulledDirn.z;

  float x0 = v->x;
  float y0 = v->y;
  float z0 = v->z;

 // if( triNormal->y < 0.5f ) return( false );

 // if( dirn->DotProduct( triNormal ) >= 0) 
  {

	

		// initial test - consider only the current hunk!  (should consider whole line of hunks)

	slong hunkX, hunkZ;
	hunk * hunkPtr;


		// Find the hunk which contains this XZ point..

	mesh * meshData = &worldModel->meshData;

	hunkX = (slong)((v->x - meshData->MinCoordX) * oneoverxhunksize);
	hunkZ = (slong)((v->z - meshData->MinCoordZ) * oneoverzhunksize);
		
	if( hunkX >= xhunks) hunkX = xhunks-1;
	if( hunkZ >= zhunks) hunkZ = zhunks-1;

	hunkPtr = hunkList + hunkX*zhunks + hunkZ;

		// hunkPtr now points to the appropriate hunk .. examine both wall and non-wall lists
	vector3 * pcoord, *qcoord, * rcoord;

	bool tcondn;
	
	triangle ** tlist = hunkPtr->tlistPtr_N;
	long total = hunkPtr->tcount_N;
	long ii;
	float bestt;

	bestt = 999999.0f;

	//double fsgshs = f*f + g*g + h*h;

	do{
		triangle ** actualtlist = tlist;

		for(ii = total;ii--;)
		{

			triPtr = *(actualtlist++);

			#define ACCY 0.000001f


			float denom = triPtr->A * f + triPtr->B * g + triPtr->C * h;
if( abs(triPtr->p.v->geoCoord.y) < 0.7f )
			if( (denom >= ACCY) || (denom <= -ACCY) )					// NOT PARALLEL TO PLANE
			{

				float t  = -( triPtr->A * x0 + triPtr->B * y0 + triPtr->C * z0 + triPtr->planeD) * RECIPROCAL(denom);

				if(( t >= 0.0f ) && (t <= 1.0f) )
				{
					float xi = x0 + f * t;
					float yi = y0 + g * t;
					float zi = z0 + h * t;

				//
				//	(xi, yi, zi ) at point on plane of triangle where intersection occurs
				//			

					pcoord = &triPtr->p.v->geoCoord;
					qcoord = &triPtr->q.v->geoCoord;
					rcoord = &triPtr->r.v->geoCoord;

					float Xtp = pcoord->x - xi;
					float Ztp = pcoord->z - zi;
					float Xup = qcoord->x - xi;
					float Zup = qcoord->z - zi;
					float Xvp = rcoord->x - xi;
					float Zvp = rcoord->z - zi;
			
					if( Xtp*Zup  >  Xup*Ztp )
					{
						tcondn = ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
					} else
					{
						tcondn = ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
					}

					if(tcondn)
					{
					
						if( t < bestt)
						{
							r.x = xi;
							r.y = yi+2;
							r.z = zi;
							bestt = t;
						};
						foundit = true;
					};
				};
			} else
				return(false);

		};
		//if( tlist == hunkPtr->tlistPtr_N )
		//{
		//	tlist = hunkPtr->tlistPtr_W;
		//	total = hunkPtr->tcount_W;
		//}
		//else
			break;
	} while(1);
  };

	if( foundit )
	{
		*shadowCoord = r;

		//shadowCoord->y += 2;

		shadowCoord->z -= 1;
	} else
	{
		v->x += f;
		v->y += g;
		v->z += h;

		float yresult;

		getY( v, &yresult, &triPtr, false );
	
		shadowCoord->x = v->x;
		shadowCoord->y = yresult;//+1;
		shadowCoord->z = v->z - 1;
	};

	return(true);
	
};


//================================================================================================================================
// getY
// ----
//		get height at point (or close to). 
//
//		- Used to retrieve the height of an XZ point above the world.
//
//		returns 'true' iff the point(XZ) is on the world.
//
//================================================================================================================================
//debugger DBGR;
double sum = 0;
double cnt = 0;

bool world::getY( vector3 * v, float * yresult, triangle ** resulttriPtr, bool wallsallowed)     // Assumes v->x, v->z inrange (above world)
{
		
	slong hunkX, hunkZ;
	hunk * hunkPtr;

	
		// Find the hunk which contains this XZ point..

	mesh * meshData = &worldModel->meshData;

	hunkX = (slong)((v->x - meshData->MinCoordX) * oneoverxhunksize);
	hunkZ = (slong)((v->z - meshData->MinCoordZ) * oneoverzhunksize);
		
	if( hunkX >= xhunks) hunkX = xhunks-1;
	if( hunkZ >= zhunks) hunkZ = zhunks-1;

	hunkPtr = hunkList + hunkX*zhunks + hunkZ;


		//
		// Now examine every triangle in the hunk to find the triangle which this point sits above.
		//		- examine both Non-Wall triangles, and wall triangles
		//


//int total = hunkPtr->tcount_N + hunkPtr->tcount_W;
//static char zzzz[200];
//cnt = cnt + 1;
//sum += (float)(total);
//sprintf(zzzz,"nearby = %d  average = %f\n", total, sum / cnt );
//DBGR.Dprintf(zzzz);

	yfound = false;
	if( scanTrianglesForXZ( hunkPtr->tlistPtr_N, hunkPtr->tcount_N, v, yresult, resulttriPtr) )
	{
		return(true) ;
	};
	if(wallsallowed)
		return( scanTrianglesForXZ( hunkPtr->tlistPtr_W, hunkPtr->tcount_W, v, yresult, resulttriPtr) );
	else
		return(false);

}



world::~world()
{
	//delete[] worldModel->meshData->vlistPtr;
	//delete[] worldModel->meshData->tlistPtr;
	delete worldModel;
	delete[] hunkList;

};

//
//  for all the world...
//	find the triangle which is intersected by vector from 'p' in direction 'd' for which
//  the point of intersection is the closest.
//  This involves firstly finding the hunks along the direction vector, and testing the
//  triangles in each hunk WITHOUT RE-EXAMINING TRIANGLES occuring in more than one hunk.
//  ( returns true iff intersection found )

bool world::findClosestIntersectedTriangle( vertex * p, vertex * d, triangle * t, float * distance)
{
		
	return(true);
};

//
//  for all the world...
//  find the triangle which is closest (at nearest point) to a coordinate 'p'
//  ( involves examining firstly the hunk containing 'p' and then the hunks
//    surrounding it.  Note that because hunks are rectangular the closest triangle
//    is not necessarily in the same hunk as the point!
//  )
//

void world::findClosestTriangle( vertex * p, float * distance )
{


};
 

void world::updateLforvertex( vertex * v)
{

	v->l =  sunLightDirection.DotProduct( &v->vertexNormal ) ;
	if (v->l < 0 ) v->l = 0;

}

//-------------------------------------------------------------------------------------------
// initialise the solar light source
//-------------------------------------------------------------------------------------------

void world::setSun( vector3 * lightDirection,
	 			  uchar  ambientR, uchar ambientG, uchar ambientB,
				  uchar  incidentR, uchar incidentG, uchar incidentB)
{

	sunAmbientColour.red  = ambientR;
	sunAmbientColour.grn  = ambientG;
	sunAmbientColour.blu  = ambientB;
	sunIncidentColour.red = incidentR;
	sunIncidentColour.grn = incidentG;
	sunIncidentColour.blu = incidentB;

		//
		// Calculate 'l' for each WORLD co-ordinate now, since the world foes NOT move in world space!
		//

	sunLightDirection = *lightDirection;
	sunLightDirection.Normalise();
	updateWorldForSun();
};


void world::updateWorldForSun()
{
	ulong t;
	if( sunOn && ( sunLightDirection != vector3(0,0,0) ) )
	{
		mesh * meshData = &worldModel->meshData;

		if( lastSunLightDirection != sunLightDirection)
		{
			vertex * vlist = meshData->vlistPtr;

			for( t = meshData->vcount; t--; )
			{
				updateLforvertex( vlist );
				vlist++;
			};

			lastSunLightDirection = sunLightDirection;
		};
	}
}

void world::setSunDirection( vector3 * d )
{
	sunLightDirection = *d;
	sunLightDirection.Normalise();
	updateWorldForSun();
}

void world::turnSunOn()
{
	if( !sunOn )
	{
		sunOn = true;
		updateWorldForSun();
	}
};

void world::turnSunOff()
{
	sunOn = false;
};

//================================================================================================================================
// END OF FILE
//================================================================================================================================
