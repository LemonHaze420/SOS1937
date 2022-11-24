// $Header$

// $Log$
// Revision 1.4  2000-01-19 14:53:17+00  jjs
// Added getWorldFromScreen.
//
// Revision 1.3  2000-01-18 14:47:42+00  jjs
// Changed to use intrinsics for speed.
//
// Revision 1.2  2000-01-17 12:37:06+00  jjs
// Latest version has corrected Alpha Tested polygons.
//
// Revision 1.1  2000-01-13 17:27:03+00  jjs
// First version that supports the loading of objects.
//

#include "bs2all.h"

engine  * boris2;			// The global engine

engine::engine(long ttotal, long vtotal, long ntotal, float physicsTicks)
{
#if 0
	Caps = renderGetCaps();

	runsprites=true;

	lod_threshold_scale=1.0;

	dumpframe=false;

	cameraangle = 0;
	cameraLoft = 0;
#endif
	uniqueTid = 0;

	initialiseArcHandles();
#if 0
	outLists ** myolPtr;
#endif
	currentdisplay    = NULL;
    currentcamera     = NULL;
    currentworld      = NULL;
	objectList		  = NULL;
#if 0
    spriteFXlist	  = NULL; 
#endif

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
#if 0
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

	dprintf("allocating pools");

	spritePool = new Pool<spriteFXentry> (1024);
	vertexPool = new Pool<spriteVertexGroup> (1024);

#endif
}

engine::~engine()
{
}

int engine::GetLod(model *m,float z)
{
	z *= lod_threshold_scale;

	for(int i=0;i<m->meshct;i++)
		if(z<m->meshthresholds[i])
		{
			if(i<m->minimum_lod)i=m->minimum_lod;
			return i;
		}

	i--;
	if(i)
		return i-1;
	else
		return 0;
}

void engine::processObjectUnlit( object * objPtr )
{
//	static vector3 transformedFnormal;
	static matrix3x4 WCtransform;
	matrix3x4 * catMat;
	object * myObject;
//	static   vertex   * pvPtr, * qvPtr, * rvPtr;
//	ulong t;
	HRESULT hr;

//	static vector3 tempvector;
//	bool potentiallyVisible;
//	bool shadowTri;
		
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

			D3DMATRIX mat;

			mat._11 = WCtransform.E00;
			mat._12 = WCtransform.E10;
			mat._13 = WCtransform.E20;
			mat._14 = 0.0f;
			mat._21 = WCtransform.E01;
			mat._22 = WCtransform.E11;
			mat._23 = WCtransform.E21;
			mat._24 = 0.0f;
			mat._31 = WCtransform.E02;
			mat._32 = WCtransform.E12;
			mat._33 = WCtransform.E22;
			mat._34 = 0.0f;
			mat._41 = WCtransform.translation.x;
			mat._42 = WCtransform.translation.y;
			mat._43 = WCtransform.translation.z;
			mat._44 = 1.0f;

		    g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &mat );

			if( myObject->modelData != 0)		// ie not a NULL model 
			{
				// check bounding box is onscreen ....
				
				if(1)//checkRboundboxVisibility( &myObject->transformedBoundingBox  ) )
				{
					// calculate which LOD to use
					
					vector3 opos,rpos;
					
					opos = myObject->concatenatedTransform.translation - currentcamera->position;
					currentcamera->RotMatrix.multiplyV3By3x3mat(&rpos,&opos);
					
					int lod = GetLod(myObject->modelData,rpos.z);
					
					if(lod>myObject->modelData->highest_visible_mesh)
						goto donotrender;
					
					mesh *lodmesh = myObject->modelData->meshData+lod;

					// Now we render the model as it finds it
					for(int i = 0; i<lodmesh->mcount; ++i)
					{
						if(lodmesh->indcount[i])
						{
							renderSetCurrentMaterial( &lodmesh->mlistPtr[i].bm);
//							if(lodmesh->mlistPtr[i].bm.flags & MAT_DOUBLESIDED)
//								hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
//							else
								hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
							hr = g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEXC, lodmesh->vertBuf,lodmesh->vbcount,lodmesh->indices[i],lodmesh->indcount[i],D3DDP_DONOTCLIP );
						}
					}
					hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
					
				}	// if (ok)
				
			} // if (checkRboundboxVisibility
				
donotrender:
			
			if( myObject->definedInCameraSpace )
				myObject->transform.translation.z -= zoomz;
		}		  
		myObject = myObject->sister;
	}  // while( myObject)
}

void engine::setCurrentCamera( camera * c )
{
	currentcamera = c;

}

void engine::getWorldFromScreen(vector3 *v,float x,float y,float z)
{
	// convert screen coordinates to camera coordinates

	vector3 v2;

	v2.z = z * 30000.0f;
	v2.x = (x - 320.0f) / 320.0f / ProjectionMatrix._11 * v2.z;
	v2.y = (240.0f - y) / 240.0f / ProjectionMatrix._22 * v2.z;
	// now convert from camera coordinates to world coordinates

	currentcamera->inverseRotMatrix.multiplyV3By3x3mat(v,&v2);

	v->x += currentcamera->position.x;
	v->y += currentcamera->position.y;
	v->z += currentcamera->position.z;
}
