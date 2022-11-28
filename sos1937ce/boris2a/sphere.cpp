//================================================================================================================================
// Sphere.cpp
// ----------
//			Implementation of the sphere and sphereset class.
//================================================================================================================================

#include "BS2all.h"

#include <malloc.h>

extern engine * boris2;
//extern debugger DBGR;


//static char zzz[200];

SphereSet::SphereSet(char *filename, float scale)
{
	char *area = (char *)_alloca(32768);

	data=NULL;

	unsigned long size = archivestack.ExtractIntoArea(filename,FMT_SPHERESET,area,32768);

	if(size)
	{
		num = *((int *)area);
		area+=sizeof(int);

		data = new Sphere [num];

		for(int i=0;i<num;i++)
		{
			data[i].type = *((SphereType *)area);
			area+=sizeof(SphereType);
			data[i].v = (*((vector3 *)area)) * scale;
			area+=sizeof(vector3);
			data[i].v2 = *((vector3 *)area);
			area+=sizeof(vector3);
			data[i].rad = (*((float *)area)) * scale ;
			area+=sizeof(float);

			data[i].radsquared = data[i].rad*data[i].rad;
		}
	}
}

SphereSet::~SphereSet()
{
	if(data)
		delete [] data;
}


//================================================================================================================================
//  create 'this.transformedSpheres' from the parameters:
//									
//================================================================================================================================

void SphereSet::transformSpheres( SphereSet * ss, matrix3x4 * transf )
{
	Sphere *st,*s,*end;

	ss->num  = this->num;
	s        = this->data;
	st       = ss->data;

	end      = s + this->num;

	for(;s!=end;s++,st++)
	{
		*st = *s;

		switch(s->type)
		{
			case SPHERE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				break;
			case LINE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				transf->multiplyV3By3x3mat(&st->v2, &s->v2);
				break;
			case CIRCLE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				transf->multiplyV3By3x3mat(&st->v2, &s->v2);

				st->offset =	-(st->v2.x * st->v.x +
								st->v2.y * st->v.y +
								st->v2.z * st->v.z);

				// here we get the rotation of the wheel so we can find where it has
				// the low friction

				break;
		}
	}
}

//================================================================================================================================
// IsTriColliding
// --------------
//				is a triangle colliding with the sphereSet ??
//
//				Uses untransformed triangle world co-ordinates - ie. suitable for WORLD only!
//================================================================================================================================

int collnum=0;
vector3 ipoint;
Sphere *collss;
float colldepth;
vector3 collvector(0,0,0);
vector3 collpoint;


//------------------------------------------------------------------------------------------------
// edgeIntersectSphere
// -------------------
//
//------------------------------------------------------------------------------------------------

int Sphere::edgeIntersectSphere( vector3 * p, vector3 * q, vector3 * fv)
{
	float t;

	float f = q->x - p->x;
	float g = q->y - p->y;
	float h = q->z - p->z;

	//
	// the parametric line co-efficient on pq at the nearest point is expressed
	// as 'num / den' where  'num' and 'den' are calculated below.
	// However since we only wish to calculate 't' when   0 <= t <= 1 we can be a bit
	// clever about avoiding the division unless absolutely necessary
	//

	float num = f * (fv->x - p->x) + g * (fv->y - p->y ) + h * (fv->z - p->z);
	float den = f*f + g*g + h*h;


	float deltax = fv->x -  p->x;
	float deltay = fv->y -  p->y;
	float deltaz = fv->z -  p->z;

	bool nless = ( num < 0.0f );

	if( nless == ( den < 0.0f ))		//  otherwise ( n/d ) < 0  therefore 't' = 0 conceptually.
	{
		// num and den both have the same sign
		if( nless )
		{
			if (num < den)				// then  num/den  < -1
			{
				deltax -= f;
				deltay -= g;
				deltaz -= h;
			}
			else
			{
				t = num * RECIPROCAL( den );
				deltax -= f*t;
				deltay -= g*t;
				deltaz -= h*t;
			}
		}
		else
		{
			if (num > den)				//  then num / den > 1
			{
				deltax -= f;
				deltay -= g;
				deltaz -= h;
			}
			else
			{
				t = num * RECIPROCAL( den );
				deltax -= f*t;
				deltay -= g*t;
				deltaz -= h*t;
			}
		}
	}

	if( (deltax * deltax + deltay * deltay + deltaz * deltaz) < radsquared )
	{
		return 0;	
	}

	static vector3 pv,qv,rv;
	// so we work out which side we're on. If all calls to this routine return non-zero,
	// then the calling routine checks to see if the sphere's centre is on the same side
	// of all the edges. If so, we're inside the triangle.

	pv = *q - *p;
	qv.x = fv->x - p->x;
	qv.y = fv->y - p->y;
	qv.z = fv->z - p->z;

	rv = pv*qv;


	return rv.y < 0 ? 1 : -1;
}



bool  SphereSet::isTriColliding( triangle *tri ) //, BroadVertex *verts)
{
	// get the triangle data

	float f,g,h,t;
	static vector3 v0, v1, v2, p,q,intersects[2];
	vector3 *pn;	// plane normal

	Sphere *s,*end;
	int e1,e2,e3;

	v0 = *tri->p.v->geoCoord;
	v1 = *tri->q.v->geoCoord;
	v2 = *tri->r.v->geoCoord;

	pn = &(tri->faceNormal);

	// work out bounds of triangle

	p=v0+v1+v2;
	p/=3;
	float d,d1;
	q=p-v0;
	d=q.MagSquared();
	q=p-v1;
	d1=q.MagSquared();
	if(d1>d)
		d=d1;
	q=p-v2;d1=q.MagSquared();
	if(d1>d)
		d=d1;


	// iterate through the spheres (some of which may be cylindrical)

	s = this->data;
	end = s+this->num;

	collnum=0;
	for(;s!=end;s++)
	{
		collss=s;
		switch(s->type)
		{
			case SPHERE:

				// do tri-sphere collision
				// non-optimal, probably... 
				// 1) does the plane of the triangle cut the sphere?

				colldepth = s->radsquared - tri->DistanceSquared(&s->v);//tri->plane.Distance(s->v);

				if(colldepth>=0)
				{
					// the triangle could still be cutting the sphere, but with all
					// its points outside it.

			 
					// first test on the bounding sphere of the triangle

					q=p - s->v;
					d1=q.MagSquared();
					if(d1>d)
						return false;


					// 2)
					// now we test to see if any of the edges are intersecting the sphere
					// we use "distance-squared of a point from a line segment" - if this is less
					// than r-squared, then we have an intersect. Also, the return value of EdgeIntersect
					// is 0 for an intersect, -1 for left hand side, 1 for left hand side.
	
					collvector = *pn;

					if(!(e1=EdgeIntersectSphere(s,&v0,&v1)))
						return true;
					if(!(e2=EdgeIntersectSphere(s,&v1,&v2)))
						return true;
					if(!(e3=EdgeIntersectSphere(s,&v2,&v0)))
						return true;

					if(e1==e2 && e2==e3)
					{
						// in this case, to work out the collision point, we project the normal of the plane
						// from the centre of the sphere to its surface

						collpoint.x = s->v.x - pn->x * s->rad;
						collpoint.y = s->v.y - pn->y * s->rad;
						collpoint.z = s->v.z - pn->z * s->rad;
						return true;
					}
				}

				break;
			case LINE:
				{
				vector3 ipos,edge,cline;

				f = s->v2.x - s->v.x;	// get coeffs of line
				g = s->v2.y - s->v.y;
				h = s->v2.z - s->v.z;

				// get parameter of intersection of line with plane
				
				
				float numerator = tri->planeD - (pn->x * s->v.x + pn->y * s->v.y + pn->z * s->v.z);
				float denominator = (pn->x * f + pn->y * g + pn->z * h);

				bool nless = (numerator < 0.0f);
				bool dless = (denominator < 0.0f);

				if( nless != dless  )
					break;				// opposite signs mean   numerator/denominator < 0

				if(nless)		// numerator and denomnator BOTH negative
				{
					if ( numerator <= denominator )		// ie  numerator/denominator >  1.0
						break;
				} 
				else			// numerator and denomnator BOTH positive
				if ( numerator >= denominator )		// ie  numerator/denominator >  1.0
					break;

				t = numerator * RECIPROCAL( denominator );	// if 't' is outside [0,1] then we've got a bug!

				// get the coordinates of the intersection and check to see which side of
				// the triangle they're on.

				ipos.x = f*t + s->v.x;
				ipos.y = g*t + s->v.y;
				ipos.z = h*t + s->v.z;

				//then for each edge get the side of the edge that this point is on 

				vector3 edge1;

				// edge 1
				edge = v0 - v1;	cline = ipos - v0;
				edge1 = edge * cline;

				// edge 2
				edge = v1 - v2;	cline = ipos - v1;

				vector3 vvv = edge * cline;
				edge = vvv;

				// if this line is >90 away from the previous one, it's not on the same side.
				if(edge.DotProduct(&edge1)<0)
					break;

				// edge 3
				edge = v2 - v0;	cline = ipos - v2;

				vvv = edge * cline;
				edge = vvv;

				// if this line is >90 away from the previous one, it's not on the same side.
				if(edge.DotProduct(&edge1)<0)
					break;

				// the collision depth is the shorter of the distances from each end point to
				// the intersection point

				p = s->v - ipos;	
				f=p.MagSquared();
				q = s->v2 - ipos;	
				g=q.MagSquared();

				colldepth = (f>g)?g:f;

				collpoint = ipos;
				collvector = *pn;
				return true;
				}
			case CIRCLE:
				// intersect the plane of the circle with each edge. We should get two or zero points,
				// which will be p and q

				e1=0;	// counter of intersections got.
			
				// first edge, v0 to v1.
				// get line parameters and intersection parameter

				float denom;

				f = v1.x - v0.x;	
				g = v1.y - v0.y;	
				h = v1.z - v0.z;
				denom = (s->v2.x * f + s->v2.y * g + s->v2.z * h);

				if(denom)
				{
					t = -(s->v2.x*v0.x + s->v2.y*v0.y + s->v2.z*v0.z + s->offset) / denom;


					if(t>=0 && t<=1)
					{
						// it's in our line segment
						intersects[0].x = v0.x + f*t;
						intersects[0].y = v0.y + g*t;
						intersects[0].z = v0.z + h*t;
						e1++;
					}
				}

				// second edge, v1 to v2.
				// get line parameters and intersection parameter

				f = v2.x - v1.x;	
				g = v2.y - v1.y;	
				h = v2.z - v1.z;
				denom = (s->v2.x * f + s->v2.y * g + s->v2.z * h);
				if(denom)
				{
					t = -(s->v2.x*v1.x + s->v2.y*v1.y + s->v2.z*v1.z + s->offset) / denom;


					if(t>=0 && t<=1)
					{
						// it's in our line segment
						intersects[e1].x = v1.x + f*t;
						intersects[e1].y = v1.y + g*t;
						intersects[e1].z = v1.z + h*t;

						e1++;
					}
				}

				// if e1 is zero at this point, we're not going to get any more intersections
				// so just quit - the triangle does not intersect the plane. If e1 is 2, then
				// we have both intersections, so continue. Otherwise, we need to get the other
				// intersection.

				if(!e1)
					break;

				if(e1==1)
				{
					// third edge, v2 to v0.
					// get line parameters and intersection parameter
	
					f = v0.x - v2.x;	
					g = v0.y - v2.y;	
					h = v0.z - v2.z;
					denom = (s->v2.x * f + s->v2.y * g + s->v2.z * h);

					t = -(s->v2.x*v2.x + s->v2.y*v2.y + s->v2.z*v2.z + s->offset) / denom;

					if(t>=0 && t<=1)
					{
						// it's in our line segment
						intersects[1].x = v2.x + f*t;
						intersects[1].y = v2.y + g*t;
						intersects[1].z = v2.z + h*t;
					}
				}

				// at this point, we have two points in the circle's plane which intersect
				// the triangle. We now join these points with a line segment, and find the
				// distance of this line segment from the centre point of the circle.

				f = intersects[1].x - intersects[0].x;
				g = intersects[1].y - intersects[0].y;
				h = intersects[1].z - intersects[0].z;

				// get the parameter for the closest point

				float tt;

				tt=t = (	f*(s->v.x - intersects[0].x) +
							g*(s->v.y - intersects[0].y) +
							h*(s->v.z - intersects[0].z)
						) / (f*f + g*g +h*h);

				if(t<0)
					t=0;
				if(t>1)
					t=1;


				// get the point and calculate the distance squared

				float x,y,z;

				x = s->v.x - (intersects[0].x + f*t);
				y = s->v.y - (intersects[0].y + g*t);
				z = s->v.z - (intersects[0].z + h*t);

				x = x*x + y*y + z*z;

				if(x>s->radsquared)
					break;	// and leave if we're not in the circle

				colldepth = s->radsquared-x;	// approximate collision depth

				ipoint.x = intersects[0].x + f*t;
				ipoint.y = intersects[0].y + g*t;
				ipoint.z = intersects[0].z + h*t;

				// right, we have a collision. Where is it? We'll assume it's whichever of
				// the two intersection points is closest to the centre. Easy.

				t = (t<0.5) ? 0.0f : 1.0f;

				collpoint=ipoint;

				collvector =*pn;
				return true;
			}
			collnum++;
	}

	return false;
}
 
//================================================================================================================================
// IsCollidingWithWorld
// --------------------
//		Return number of collisions between the object sphereset and the world.  0 = no collisions
//	
//		We use a point in world space (v) and a distance 'd' to decide which of the hunks to examine.	This will almost certainly
//		be the object centre in world space.
//================================================================================================================================

static triangle * outList[3000];


ulong SphereSet::isCollidingWithWorld( vector3 * v,  float d)
{

	long kk;
	slong lohunkX, lohunkZ, hihunkX, hihunkZ;
	hunk * hunkPtrX, * hunkPtr;
	
	// Find the hunk which contains this XZ point..

	// note - only the zeroth mesh contains spheres

	mesh * meshData = boris2->currentworld->worldModel->meshData;

	FASTFTOL(&lohunkX , (v->x - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
	FASTFTOL(&lohunkZ , (v->z - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);
	FASTFTOL(&hihunkX , (d + v->x - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
	FASTFTOL(&hihunkZ , (d + v->z - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);
	
	if( lohunkX >= boris2->currentworld->xhunks) 
		lohunkX = boris2->currentworld->xhunks-1;
	if( lohunkZ >= boris2->currentworld->zhunks) 
		lohunkZ = boris2->currentworld->zhunks-1;
	if( hihunkX >= boris2->currentworld->xhunks) 
		hihunkX = boris2->currentworld->xhunks-1;
	if( hihunkZ >= boris2->currentworld->zhunks) 
		hihunkZ = boris2->currentworld->zhunks-1;

	hunkPtrX = boris2->currentworld->hunkList + lohunkX*boris2->currentworld->zhunks + lohunkZ;

	ulong hx, hz;

	triangle ** outListPtr =  outList;
	triangle * triPtr;

	ulong collisions = 0;
	ulong pcntvalue = boris2->globalpctr;
	boris2->globalpctr++;
	for( hx = 1+ hihunkX-lohunkX; hx--; )
	{
		hunkPtr = hunkPtrX;
		for( hz = 1+ hihunkZ-lohunkZ; hz--; )
		{

			triangle ** thistlist;

			if(hunkPtr->tcount_W)
			{
				thistlist =hunkPtr->tlistPtr_W;
				for (kk = hunkPtr->tcount_W; kk--; thistlist++)
				{
				    triPtr = *thistlist;

					if(triPtr->pcnt != pcntvalue )
					{
						triPtr->pcnt = pcntvalue;

						if(triPtr->faceNormal.y < 0.7f)
						if( isTriColliding( triPtr ) )
						{
								*outListPtr++ = triPtr;	
								collisions++;
						}
					}
				}
			}

			hunkPtr++;
		}
		hunkPtrX += boris2->currentworld->zhunks;
	}

	return( collisions );
}

vector3 maxDistance;
float bestt;
float bestDistanceSquared;
vector3 bestDeltaVector;
triangle * bestTriangle;

//extern debugger DBGR;

void Sphere::lineProtrusion( vector3 * v0, vector3 * deltaVector	)
{
	float minx, minz, maxx, maxz;
	hunk * hunkPtrX, * hunkPtr;

					//
					// set minx,minz,  maxx, maxz  to the hunk bounds. To do this we look at either end of the line and
					// find bounds which are easy to calculate. Note that 'rad' is always positive, so we only need to
					// examine the sign of the distance in an axis to know how to get the bounds.
					//
	
//static char zzz[222];
//sprintf(zzz,"%f %f %f    %f %f %f\n",v.x, v.y, v.z,v2.x, v2.y, v2.z);
//DBGR.Dprintf(zzz);

	 static vector3 vv;

	 vv = *v0 + (*deltaVector);

						if ( maxDistance.x >= 0 )
						{
							maxx = vv.x + maxDistance.x + rad;
							minx = vv.x - maxDistance.x - rad;
						} else
						{
							maxx = vv.x - maxDistance.x + rad;
							minx = vv.x + maxDistance.x - rad;
						};

						if ( maxDistance.z >= 0 )
						{
							maxz = vv.z + maxDistance.z + rad;
							minz = vv.z - maxDistance.z - rad;
						} else
						{
							maxz = vv.z - maxDistance.z + rad;
							minz = vv.z + maxDistance.z - rad;
						};

					//
					// parse the square bound set of hunks 
					//

					mesh * meshData = boris2->currentworld->worldModel->meshData;
  
					slong lohunkX;
					slong lohunkZ;
					slong hihunkX;
					slong hihunkZ;

					FASTFTOL(&lohunkX , (minx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
					FASTFTOL(&lohunkZ , (minz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);
					FASTFTOL(&hihunkX , (maxx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
					FASTFTOL(&hihunkZ , (maxz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);

					if( lohunkX >= boris2->currentworld->xhunks) lohunkX = boris2->currentworld->xhunks-1;
					if( lohunkZ >= boris2->currentworld->zhunks) lohunkZ = boris2->currentworld->zhunks-1;
					if( hihunkX >= boris2->currentworld->xhunks) hihunkX = boris2->currentworld->xhunks-1;
					if( hihunkZ >= boris2->currentworld->zhunks) hihunkZ = boris2->currentworld->zhunks-1;

					if( lohunkX < 0 ) lohunkX = 0;
					if( hihunkX < 0 ) hihunkX = 0;
					if( lohunkZ < 0 ) lohunkZ = 0;
					if( hihunkZ < 0 ) hihunkZ = 0;
   
					hunkPtrX = boris2->currentworld->hunkList + lohunkX*boris2->currentworld->zhunks + lohunkZ;

					ulong hx, hz;	
					long kk;

					triangle ** outListPtr =  outList;
					triangle * triPtr;

		 			//ulong collisions = 0;
					//ulong pcntvalue = boris2->globalpctr;
				
					boris2->globalpctr++;
					for( hx = 1+ hihunkX-lohunkX; hx--; )
					{
						hunkPtr = hunkPtrX;
						for( hz = 1+ hihunkZ-lohunkZ; hz--; )
						{

							triangle ** thistlist;
							if(hunkPtr->tcount_W)
							{

								//
		 						// - reject this hunk if the XZ time projection of the t-cylinder does not intersect
								//

								//
								// - examine all triangles of this hunk against the t-cylinder
								//

								thistlist =hunkPtr->tlistPtr_W;
								for (kk = hunkPtr->tcount_W; kk--; thistlist++)
								{
								    triPtr = *thistlist;
									if(!(triPtr->mtl->bm.flags & MAT_COLLIDABLE ))
										continue;

									//if(triPtr->pcnt != pcntvalue )
									//{
									//	triPtr->pcnt = pcntvalue;
									//	if(triPtr->faceNormal.y < 0.7f)	// considered as collidable
										{
											
											//
											// Find the point where the triangle intersects the t-cylinder
											// - this is done in stages:
											//		(i)  dose the  t-cylinder intersect the plane?
											//		(ii) is the area of intersection within the triangle ?
											//

											//
											// Test to see if the cylinder t-axis cuts into the plane of the triangle.
											// - use the good 'ol formula:   
											//							t =  - (Ax0 + By0 + Cz0 + D) / (Af + Bg + Ch )
											// where = f,g,h are the parametrics of the line:
											//				f = (x1-x0)  therefore on the line  f = maxDistance.x
											//				g = (y1-y0)  therefore on the line  g = maxDistance.y
											//				h = (z1-z0)  therefore on the line  h = maxDistance.z
											//
										

											float num = -(  triPtr->A * vv.x + 
															triPtr->B * vv.y + 
															triPtr->C * vv.z +
															triPtr->planeD);
											float den =     triPtr->A * maxDistance.x + 
															triPtr->B * maxDistance.y + 
															triPtr->C * maxDistance.z;

									
											/*
											static vector3 vi;
											float t = num / den;
											vi = v + maxDistance*t;		// point of intersection with plane


											float psqr = (-num ) * triPtr->ABC;	 // ABC = 1/(a^2 + b^2 + c^2)

											float qsqr = ( vi.x - v.x ) * ( vi.x - v.x ) +
																	( vi.y - v.y ) * ( vi.y - v.y ) +
																	( vi.z - v.z ) * ( vi.z - v.z );

											float j = rad * sqrt( qsqr / psqr );
													
											 t = j / sqrt(qsqr);

											 if(( t < 0) || (t > 1))
												 t = num / den;
										
											bool ok = true;
											*/
											
											bool  nless = ( num < 0.0f );
											bool  dless = ( den < 0.0f );
	
											bool  ok = false;

											if( nless == dless )
											{							// both numerator and denominator have the same sign
												if( nless )
													ok = ( num >= den );
												else
													ok = ( num <= den );

											};
											
//float t = num/den;
//ok = ((t>=0)&&(t<=1));
											if(ok)		// t is in range if we do  t = num/den
											{
												//
												// t = parametric point on t-axis at intersection...
												//
												float t = num / den;

												//
												// Evaluate the actual point on the line
												//

												static vector3 intersect;
												
												intersect = v + ( maxDistance * t );	//intersect = intersection point.

												//
												// Containment test for point in triangle is now required 
												//

												bool containedXZ = intersect.isCoordOnXZtriangle( triPtr );
												bool containedXY = intersect.isCoordOnXYtriangle( triPtr );
												bool containedYZ = intersect.isCoordOnYZtriangle( triPtr );

											
												bool ncontainedXZ = intersect.isCoordNearlyOnXZtriangle( triPtr );
												bool ncontainedXY = intersect.isCoordNearlyOnXYtriangle( triPtr );
												bool ncontainedYZ = intersect.isCoordNearlyOnYZtriangle( triPtr );

												if( (containedXZ && containedXY &&  ncontainedYZ) ||  
													(containedXZ && ncontainedXY && containedYZ) ||  
												    (ncontainedXZ && containedXY && containedYZ) 
												  )
												{

													  //
													  // Containment test completed. - 
													  //	collision will occur within the time interval required
													  // therefore keep the minimum 't' parametric value to find the
													  // earliest collision point. ( we shall only resolve for time AFTER
													  // the smallest 't' is found.

													  if ( t < bestt ) 
													  {
														  bestt = t;
														  bestDeltaVector = *deltaVector;
														  bestTriangle = triPtr;
													  };
																									  

												};


											}
				
										}
											
									//}
								};
							};
							hunkPtr++;
						};
	 					hunkPtrX += boris2->currentworld->zhunks;
					};
				
};

//-------------------------------------------------------------------------------------------------------------------------------

bool Sphere::getTimeFromBestt(vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime )
{
	static vector3 distancevector = (v - *collisionPoint);
	float  dist = distancevector.Mag() - rad;
	float  det  = acceleration->x * acceleration->x + acceleration->y * acceleration->y + acceleration->z * acceleration->z;
	if(det < 0.0001f)
	{
		float vmag = velocity->Mag();
		if( vmag > 0.0001f)
		{
			*collisionTime = dist / vmag; 
		}
		else
			*collisionTime = 9999.0f;
	} else
	{
		float b = dist;
		float a = (float)sqrt(det);
		float c = velocity->Mag();

		float j = 4 * a * c;
		if( j > 0)
		{
			j = (float)sqrt(j);
			float oneover2a = RECIPROCAL( a * 2.0f );

			*collisionTime = ( -b + j ) * oneover2a;
			if( *collisionTime < 0)
				*collisionTime = ( -b - j ) * oneover2a;

			if(( *collisionTime < 0 ) || (*collisionTime > maximumTime ))
				*collisionTime = 9999.0f;

		} else
			*collisionTime = 9999.0f;
	};
	return(true);
};

//-------------------------------------------------------------------------------------------------------------------------------
// sphereCollidesWithWorld
// -----------------------
//
//	Project a collision hull forward in time and test against the world for collision.
//
//	If a collision exists for the closed interval  [0,maximumTime] then the method returns 'true' and fills in the
//  parameters 'collisionVector', 'collisionPoint', 'collisionTime'.   Otherwise 'false' is returned to indicate no
//  collision within this time span.	
//
//
//
//-------------------------------------------------------------------------------------------------------------------------------

extern debugger DBGR;

#define BIGFLOAT 9999999.0f

bool Sphere::sphereCollidesWithWorld( vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime )
{
	vector3 velocityNormalised;
	int e1,e2,e3;

	float m = velocity->Mag();
	if( m > 0.000001f )
	{
			velocityNormalised = *velocity * (RECIPROCAL(m));
	}
	else
	{
			velocityNormalised = vector3(0,0,0);
	};
	maxDistance =  ( (*velocity) * maximumTime + (*acceleration) * 0.5f * maximumTime * maximumTime );
	float dmag = maxDistance.Mag();


	switch(type)
	{

		case CIRCLE:
		case SPHERE:

				{
					
					float dmagOverRad = (dmag / rad);
					long iterations;
					FASTFTOL(&iterations , dmagOverRad);
					++iterations;

					static vector3 displacement;
					if( dmag < 0.000001f )
						displacement = vector3(0,0,0);
					else
						displacement = 	maxDistance * RECIPROCAL( dmagOverRad );	// so that magnitude of displacement = rad

					static vector3 finalv;									
					finalv = v;
					for( long i = iterations; i--;  finalv += displacement  )
					{

						float minx = finalv.x - rad;
						float maxx = finalv.x + rad;
						float minz = finalv.z - rad;
						float maxz = finalv.z + rad;

						mesh * meshData = boris2->currentworld->worldModel->meshData;
  
						slong lohunkX;
						slong lohunkZ;
						slong hihunkX;
						slong hihunkZ;

						FASTFTOL(&lohunkX , (minx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
						FASTFTOL(&lohunkZ , (minz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);
						FASTFTOL(&hihunkX , (maxx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize - 0.5f);
						FASTFTOL(&hihunkZ , (maxz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize - 0.5f);

						if( lohunkX >= boris2->currentworld->xhunks) lohunkX = boris2->currentworld->xhunks-1;
						if( lohunkZ >= boris2->currentworld->zhunks) lohunkZ = boris2->currentworld->zhunks-1;
						if( hihunkX >= boris2->currentworld->xhunks) hihunkX = boris2->currentworld->xhunks-1;
						if( hihunkZ >= boris2->currentworld->zhunks) hihunkZ = boris2->currentworld->zhunks-1;

						if( lohunkX < 0 ) lohunkX = 0;
						if( hihunkX < 0 ) hihunkX = 0;
						if( lohunkZ < 0 ) lohunkZ = 0;
						if( hihunkZ < 0 ) hihunkZ = 0;
   
						hunk * hl = boris2->currentworld->hunkList;
						hunk * hunkPtrX = hl + lohunkX*boris2->currentworld->zhunks + lohunkZ;

						ulong hx, hz;	
						long kk;

						triangle ** outListPtr =  outList;
						triangle * triPtr;

						//boris2->globalpctr++;
						for( hx = 1+ hihunkX-lohunkX; hx--; )
						{
							hunk * hunkPtr = hunkPtrX;
							for( hz = 1+ hihunkZ-lohunkZ; hz--; )
							{

								triangle ** thistlist;


								if(((hunkPtr - hl) < boris2->currentworld->totalhunks) &&  hunkPtr->tcount_W)
								{

									thistlist =hunkPtr->tlistPtr_W;
									for (kk = hunkPtr->tcount_W; kk--; thistlist++)
									{
									    triPtr = *thistlist;
										if(!(triPtr->mtl->bm.flags & MAT_COLLIDABLE ))
											continue;
										if( (triPtr->faceNormal.y > 0.9f) )//|| (triPtr->faceNormal.y > 0.9f) )
											continue;

								
										// Firstly see if the plane of the triangle passes within the bounding sphere

											if(this->radsquared >= triPtr->DistanceSquared(&finalv))
											{	

											// Now see whether the bounding sphere of the triangle intersects the
											// collision hull sphere
								
												float dr = (this->rad + triPtr->boundingRadius);	//maximum distance between centroids
																							//if spheres are in collision

												if( (dr * dr) >=   triPtr->centroid.distanceSquared(&finalv) )
												{
												// it is possible that the two spheres intersect. If this condition
												// was not true then it would not be possible.
								
												// Let us see if any edge of the triangle intersects the sphere
												// ... test each edge until one intersects.

													bool tempCflag = true;
											
													if( ( (e1=edgeIntersectSphere( triPtr->p.v->geoCoord, 
																				   triPtr->q.v->geoCoord, &finalv) ) ) )
													if( ( (e2=edgeIntersectSphere( triPtr->q.v->geoCoord,
																			     triPtr->r.v->geoCoord, &finalv	) ) ) )
													if( ( (e3=edgeIntersectSphere( triPtr->r.v->geoCoord,
																		          triPtr->p.v->geoCoord, &finalv ) ) ) )
													{
														// No edge actually intersects the sphere 
														// so we now need to test whether the triangle intersects the sphere
														if(0)//e1 == e2 && e2 == e3)
														{
															*collisionVector   = triPtr->faceNormal;
															collisionPoint->x = finalv.x - velocityNormalised.x * this->rad;
															collisionPoint->y = finalv.y - velocityNormalised.y * this->rad;
															collisionPoint->z = finalv.z - velocityNormalised.z * this->rad;
			
															*collisionTime = maximumTime;//0;
															return(true);
														}
														else
															tempCflag = false;

													};
													if( tempCflag )
													{
														*collisionVector   = triPtr->faceNormal;
														collisionPoint->x = finalv.x - velocityNormalised.x * this->rad;
														collisionPoint->y = finalv.y - velocityNormalised.y * this->rad;
														collisionPoint->z = finalv.z - velocityNormalised.z * this->rad;
														*collisionTime = maximumTime;//0;
														return(true);
													};
																					
											};
										};
									};
									hunkPtr++;
								};
	 							hunkPtrX += boris2->currentworld->zhunks;
						
							};
						};	
					};
				};
				return( false );
				break;

		case LINE:


				return( false );
				break;

		default:
				return(false);
	};



/*
	if ( (velocity->x == 0.0f ) && (velocity->y == 0.0f ) && (velocity->z == 0.0f ) )
		return( false );
				
	switch(type)
	{



		case CIRCLE:			// t-projection of circle becomes an elliptical cylinder anlong t-axis of projection
				
			//	{
			//	
			//		return(false); //break;
			//	};

		case SPHERE:			// t-projection of sphere becomes a cylinder along the local t-axis of projection

				bestt = 2.0f;// any value greater than '1' will do!!

				// calc. maximum distance covered in 't'-projection 
				{

					// s = ut + a(t^2)/2
			
					// - velocity is in world space

						maxDistance =  ( (*velocity) * maximumTime + (*acceleration) * 0.5f * maximumTime * maximumTime ) ;//* 300;
						static vector3 normvel;

						normvel = *velocity;
						normvel.Normalise();
					//
					// Add the collisionhull sphere radius to the t-axis of the time projected cylinder
					//

						maxDistance.x += rad * normvel.x;
						maxDistance.y += rad * normvel.y;//=0;//+= rad;
						maxDistance.z += rad * normvel.z;


//sprintf(zzz,"SPHERE.. rad = %f maxdistance = %f %f %f\n",rad,	maxDistance.x,	maxDistance.y,	maxDistance.z);	
//DBGR.Dprintf(zzz);
//sprintf(zzz,"v = %f %f %f\n",v.x,v.y,v.z);
//DBGR.Dprintf(zzz);
					//
					// create a list of all triangle hunks through which the cylinder extruded by the sphere through
					// time must pass. Since we are using hunks of infinite 'y' we can consider the flat projection onto
					// XZ of the cylinder. We shall use a mathod of bounds for ascertaining this, which should work well 
					// provided the hunk sizes are reasonably large (so we don't have too many to eliminate further)
					//  - firstly we need to find the XZ corners of the projection...


						float mag;
						static vector3 maxd;
						maxd = maxDistance; 

						maxd.NormaliseWithCheckAndMag(&mag);
						maxd *= rad;

						lineProtrusion( &v, &vector3(0,0,0)	);

//						if( type != CIRCLE )
						{
							lineProtrusion( &v, &vector3( maxd.x,0,0));
							lineProtrusion( &v, &vector3( -maxd.x,0,0));
							lineProtrusion( &v, &vector3( 0,0,maxd.z));
							lineProtrusion( &v, &vector3( 0,0,-maxd.z));
							lineProtrusion( &v, &vector3( maxd.x,0,maxd.z));
							lineProtrusion( &v, &vector3( -maxd.x,0,maxd.z));
							lineProtrusion( &v, &vector3( maxd.x,0,-maxd.z));
							lineProtrusion( &v, &vector3( -maxd.x,0,-maxd.z));
						};
						
						//lineProtrusion( &vector3(rad,0,0)	);
						//lineProtrusion( &vector3(-rad,0,0)	);
						//lineProtrusion( &vector3(0,0,rad)	);
						//lineProtrusion( &vector3(0,0,-rad)	);

					//
					// bestt is the smallest parametric for an intersection along the time-axis of the time extruded
					// spherical collision hull ( t-cylinder ) - dont confuse 'bestt' with time!!!
					//

					if( bestt <= 1.0f )
					{
//sprintf(zzz,"bestt = %f",bestt);
//DBGR.Dprintf(zzz);

//`return(true);
						//
						// find the time from 'bestt' along the local time-axis of the t-cylinder.
						//
						// the distance (each axis) at intersection is of course given by
						//		'bestt * maxDistance'	(a vector, one component for x,y,z)
						// 
						// Therefore we only need to solve
						//		bestt * maxDistance = velocity * time + acceleration * 0.5f * time*time
						// for 'time'.
						//
						// Although this uses vector quantities ( velocity, acceleration ) we know that
						// the solution of all three derivative quadratics must have a single 'time' value
						// on the closed interval [0, maximumTime] which is a common solution for all three
						// (think about it!!!). Because time is a linear axis it follows that only one solution
						// will occur on this interval and therefore we only need to solve ONE of the quadratics
						// and we know that the quadratic determinant will exist in the Real domain.
						//
						//
						// However a particular axis may be insoluble if it has no velocity. Therefore only use 
						// an axis with a velocity. If an axis has no acceleration then we cannot solve as a quadratic
						// but must solve simply as    t = s / u
						//

						//
						// collisionPoint is taken as   v + maxDistance * bestt
						//

						static vector3 tvec =  bestDeltaVector + maxDistance * bestt;
						//if( tvec.Mag() >= maxDistance.Mag()*0.8f )
						//	return(false);
						
						*collisionPoint = v + bestDeltaVector + maxDistance * bestt;

						
						*collisionVector = vector3(bestTriangle->A,bestTriangle->B,bestTriangle->C);

						return(getTimeFromBestt(velocity,  acceleration, maximumTime,
										 collisionVector, collisionPoint,collisionTime ) );

					//	getTimeFromBestt( collisionTime );

					} else
						return( false );			// No collision occured with this hull....

				};

				//break;
		case LINE:				// t-projection of line becomes a plane along the local t-axis of projection
return(false);
				{
					static vector3 cp;
					float bestDistanceSquared = BIGFLOAT;

					maxDistance =  ( (*velocity) * maximumTime + (*acceleration) * 0.5f * maximumTime * maximumTime ) ;//* 300;
				
					//
					// from the two points  'v' and 'v2' we take the point 'v + maxDistance' as a third point and calculate 
					// the t-extended plane
					//
				
					static vector3 j,k;
					j =  v + maxDistance;
					k =  v2 + maxDistance;

									
					float fvv2 = v.x - v2.x;		// for parametric representation of [v,v2] for later use...
					float gvv2 = v.y - v2.y;
					float hvv2 = v.z - v2.z;


					float xkj = v.x - j.x;
					float ykj = v.y - j.y;
					float zkj = v.z - j.z;
					float xlj = v2.x - j.x;
					float ylj = v2.y - j.y; 
					float zlj = v2.z - j.z;
	
					float planeA = ykj * zlj - zkj * ylj;
					float planeB = zkj * xlj - xkj * zlj;
					float planeC = xkj * ylj - ykj * xlj;

					float planeD = -(float)( planeA * j.x + planeB * j.y + planeC * j.z);	

					//
					// Now examine all the triangle hunks potentially intersected by the portion of this plane within
					// the quad-region  v.v2,j,k
					//

					float minx, maxx, minz, maxz;

					minmax4( v.x, v2.x, j.x, k.x, &minx, &maxx );
					minmax4( v.z, v2.z, j.z, k.z, &minz, &maxz );

					hunk * hunkPtrX, * hunkPtr;
					mesh * meshData = boris2->currentworld->worldModel->meshData;
  
					slong lohunkX = (slong)((minx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize);
					slong lohunkZ = (slong)((minz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize);
					slong hihunkX = (slong)((maxx - meshData->MinCoordX) * boris2->currentworld->oneoverxhunksize);
					slong hihunkZ = (slong)((maxz - meshData->MinCoordZ) * boris2->currentworld->oneoverzhunksize);

					if( lohunkX >= boris2->currentworld->xhunks) lohunkX = boris2->currentworld->xhunks-1;
					if( lohunkZ >= boris2->currentworld->zhunks) lohunkZ = boris2->currentworld->zhunks-1;
					if( hihunkX >= boris2->currentworld->xhunks) hihunkX = boris2->currentworld->xhunks-1;
					if( hihunkZ >= boris2->currentworld->zhunks) hihunkZ = boris2->currentworld->zhunks-1;

					if( lohunkX < 0 ) lohunkX = 0;
					if( hihunkX < 0 ) hihunkX = 0;
					if( lohunkZ < 0 ) lohunkZ = 0;
					if( hihunkZ < 0 ) hihunkZ = 0;
   
					hunkPtrX = boris2->currentworld->hunkList + lohunkX*boris2->currentworld->zhunks + lohunkZ;

					ulong hx, hz;	
					long kk;

					triangle ** outListPtr =  outList;
					triangle * triPtr;

		 			//ulong collisions = 0;
					//ulong pcntvalue = boris2->globalpctr;
				
					boris2->globalpctr++;
					for( hx = 1+ hihunkX-lohunkX; hx--; )
					{
						hunkPtr = hunkPtrX;
						for( hz = 1+ hihunkZ-lohunkZ; hz--; )
						{

							triangle ** thistlist;
							if(hunkPtr->tcount_W)
							{

								//
								// - examine all triangles of this hunk against the t-plane
								//

								thistlist =hunkPtr->tlistPtr_W;
								for (kk = hunkPtr->tcount_W; kk--; thistlist++)
								{
								    triPtr = *thistlist;
									
									//
									// calculate the line of intersection of the two planes....
									//				x = x0 + f * t
									//				y = y0 + g * t
									//				z = z0 + h * t
									//


									#define a1 planeA
									#define b1 planeB
									#define c1 planeC
									#define d1 planeD

									#define a2 triPtr->A
									#define b2 triPtr->B
									#define c2 triPtr->C
									#define d2 triPtr->planeD

									float f = b1 * c2 - b2 * c1;
									float g = c1 * a2 - c2 * a1;
									float h = a1 * b2 - a2 * b1;

									float det = f*f + g*g + h*h;

									if( !( det < 0.0001f ) )		//  planes are not considered parallel
									{		
										float detinv = RECIPROCAL( det );
										float dc = d1 * c2 - d2 * c1;
										float db = d1 * b2 - b1 * d2;
										float ad = a1 * d2 - a2 * d1;

										float x0 = ( g * dc - h * db ) * detinv;
										float y0 = - ( f * dc + h * ad ) * detinv;
										float z0 = ( f * db + g * ad ) * detinv;

										//
										// Now find where the line [v,j] intersects the line of intersection of planes
										// and find where the line [v2,k] intersects the line of intersection of planes
										//

										bool nless, dless;
										float num, den, Tj, Tk, tj, tk;
											
										// find the parametric coeeficient on the line  of intersection of planes where
										// the line 'vj' meets it. If this value is > 1 then clip it to 1. If the value is < 0
										// then clip it to 0.  this is 'tj'

										num = maxDistance.x * ( y0 - v.y ) - maxDistance.y * ( x0 - v.x );
										den = RECIPROCAL( ( f * maxDistance.y - g * maxDistance.x ) );

									
										Tj = num * den;
										if (Tj < 0 ) 
											tj = 0;
										else if (Tj > 1)
												tj = 1;
												
										//
										// Now do the same for the link [v2,k], call this parametric co-efficient 'tk'
										//	( note that 'den' is unchanged so we don't recalculate it )
										//

										num = maxDistance.x * ( y0 - v.y ) - maxDistance.y * ( x0 - v.x );
									
										Tk = num * den;
										if (Tk < 0 ) 
											tk = 0;
										else if (Tk > 1)
												tk = 1;

										//
										// Now find which of 'tj' and 'tk' is closest to the line [v,v2] since this represents
										// the first point to become in collision!!! 
										// Since we are dealing with two (probably non-parallel) lines we only need to ascertain
										// the distance from  poimt 'tj' to 'v'  and from point 'tk' to v2, find the minimum
										// and use that point as the collision point. 
										// To do this we need to find the corresponding points on the line [v,v2]
										// (phew!, my brain 'urts!  game programming never used to be this 'ard...)
										//

										float  oneOverTjTk = RECIPROCAL( Tj - Tk );

										float xj = x0 + f * tj;		// point on line of intersection of planes
										float yj = y0 + g * tj;
										float zj = z0 + h * tj;

										float t = ( Tj - tj ) * oneOverTjTk;
										
										float Xj = v2.x + fvv2 * t;		// corresponding point on line [v,v2]
										float Yj = v2.y + gvv2 * t;
										float Zj = v2.z + hvv2 * t;



										float xk = x0 + f * tk;		// point on line of intersection of planes
										float yk = y0 + g * tk;
										float zk = z0 + h * tk;

										t = ( Tk - tk ) * oneOverTjTk;
										
										float Xk = v2.x + fvv2 * tk;		// corresponding point on line [v,v2]
										float Yk = v2.y + gvv2 * tk;
										float Zk = v2.z + hvv2 * tk;

										float dksqr = (Xk-xk)*(Xk-xk) + (Yk-yk)*(Yk-yk) + (Zk-zk)*(Zk-zk);
										float djsqr = (Xj-xj)*(Xj-xj) + (Yj-yj)*(Yj-yj) + (Zj-zj)*(Zj-zj);

										if( dksqr > djsqr )
										{
												// tj..Tj represents the closest

												if( djsqr < bestDistanceSquared )
												{
													bestDistanceSquared = djsqr;
													collisionPoint->x = xj;
													collisionPoint->y = yj;
													collisionPoint->z = zj;
													bestTriangle = triPtr;
												};

										} else
										{
												if( dksqr < bestDistanceSquared )
												{
													bestDistanceSquared = dksqr;
													cp.x = xk;
													cp.y = yk;
													cp.z = zk;
													bestTriangle = triPtr;
												};

										};
									};
								};
							};
							hunkPtr++;
						};
						hunkPtrX += boris2->currentworld->zhunks;
					};

					if( bestDistanceSquared < BIGFLOAT )
					{
						*collisionVector = vector3(bestTriangle->A,bestTriangle->B,bestTriangle->C);
							*collisionPoint = cp;
						
*collisionTime = 0.0f;
						return(true);
					};
				
					return(false); //break;
				};
		
				
	};
return(false);	// dummy holder value
*/


};






bool SphereSet::collidesWithWorld( vector3 * velocity, vector3 * acceleration, float maximumTime,
								  vector3 * collisionVector, vector3 * collisionPoint, float * collisionTime )
{
	Sphere *s = data;
	bool collided = false;
//return false;
	for( long i = num; i--; )
	{
		if( s->sphereCollidesWithWorld( velocity, acceleration, maximumTime,
								    collisionVector, collisionPoint, collisionTime ) )
		{
			//if(*collisionTime < maximumTime)
			{
				maximumTime = *collisionTime;		// so that next hull has a smaller interval to examine.
				collided = true;
			};
		};
		s++;
	};
	*collisionTime = maximumTime;
	return(collided);
};


//================================================================================================================================
// END OF FILE
//================================================================================================================================
