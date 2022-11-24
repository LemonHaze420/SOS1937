// $Header$

// $Log$
// Revision 1.2  2000-01-17 12:37:07+00  jjs
// Latest version has corrected Alpha Tested polygons.
//
// Revision 1.1  2000-01-13 17:27:08+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

#define	VERTEXCOMBINE	0

char tempBuffer[128];
char tempBuffer1[128];
ulong GBLmaterialIndex;				// advanced once for every MATERIAL loaded, for every MODEL... this global
									// value is only used for sorting (hardware rendering)

void initialiseModelLoading( void )
{
	GBLmaterialIndex = 0;
}


ulong hexto32bit( char * ascii,long count)
{
	long i;
	ulong v;
	ulong total;

	total = 0;
	v = 0;
	for(i = count; i--; )
	{
		char c = *ascii;

		if ((c >= '0') && ( c <= '9' ))
			v = c-'0';
		else
			v = (c-'A')+10;
		
		total = (total<<4) + v;

		ascii++;
	}
	return( total );
}





LPD3DVERTEXC jjsVert;
int numjjsVert;

WORD Addvertex(pointdata3d &p)
{
	int i;
	D3DVERTEXC temp;
	
	temp.x = p.v->geoCoord.x;
	temp.y = p.v->geoCoord.y;
	temp.z = p.v->geoCoord.z;
	temp.nx = p.v->vertexNormal.x;
	temp.ny = p.v->vertexNormal.y;
	temp.nz = p.v->vertexNormal.z;
	temp.color = 0xffc0c0c0;
	temp.tu = p.nv.u;
	temp.tv = p.nv.v;
	
	for(i=0;i<numjjsVert; ++i)
		if(!memcmp(&jjsVert[i],&temp,sizeof(D3DVERTEXC)))
			return i;
	jjsVert[numjjsVert]=temp;
	return numjjsVert++;
}

void loadMulderFile( char * filename, mesh * meshData, float scale, char * SSname, bool uniqueTextures)
{
	Xmulder_Header mulderHeader;
	Xmulder_VertexGEO mulderVertexGEO;
	Xmulder_Texture mulderTexture;
	Xmulder_Material mulderMaterial;
	Xmulder_Triangle mulderTriangle;
	triangle * thisTrianglePtr;
	vertex   * thisVertexPtr;
	ulong texhandle;
	long i;


	texhandle = arcExtract_mul( filename );

	// - - - - - - - - - - - - - -	
	// read in the header
	// - - - - - - - - - - - - - -

	arcRead(  (char * )&mulderHeader, sizeof(Xmulder_Header), texhandle );

//	if (mulderHeader.magic != FHEADER_MAGIC)
//		fatalError("Mulder file magic number is WRONG!!");

	meshData->tcount   = mulderHeader.tricount;	
	meshData->vcount   = mulderHeader.vertexcount;

	// allocate enough space for the world triangle list and world vertex list

	thisTrianglePtr = meshData->tlistPtr = new triangle[ meshData->tcount ];
	thisVertexPtr   = meshData->vlistPtr = new vertex[ meshData->vcount ];

	vector3 *geoPtr = meshData->geoDataPtr = new vector3 [meshData->vcount];
				
	// - - - - - - - - - - - - - -	
	// read in all the geo vertexes and put the data into the world vertex list
	// - - - - - - - - - - - - - -

	meshData->MinCoordX = 	meshData->MinCoordY = 	meshData->MinCoordZ = 99999;
	meshData->MaxCoordX = 	meshData->MaxCoordZ = 	meshData->MaxCoordY = -99999;


	for (i = meshData->vcount; i--; )
	{
		// read a vertex

		arcRead(  (char * )&mulderVertexGEO, sizeof(Xmulder_VertexGEO), texhandle);

		// scale it

		static vector3 newvector;
		newvector.x = mulderVertexGEO.x * scale;
		newvector.y = mulderVertexGEO.y * scale;
		newvector.z = mulderVertexGEO.z * scale;

		thisVertexPtr->geoCoord = newvector;

		if( thisVertexPtr->geoCoord.x < meshData->MinCoordX) meshData->MinCoordX = thisVertexPtr->geoCoord.x;
		if( thisVertexPtr->geoCoord.x > meshData->MaxCoordX) meshData->MaxCoordX = thisVertexPtr->geoCoord.x;
		if( thisVertexPtr->geoCoord.y < meshData->MinCoordY) meshData->MinCoordY = thisVertexPtr->geoCoord.y;
		if( thisVertexPtr->geoCoord.y > meshData->MaxCoordY) meshData->MaxCoordY = thisVertexPtr->geoCoord.y;
		if( thisVertexPtr->geoCoord.z < meshData->MinCoordZ) meshData->MinCoordZ = thisVertexPtr->geoCoord.z;
		if( thisVertexPtr->geoCoord.z > meshData->MaxCoordZ) meshData->MaxCoordZ = thisVertexPtr->geoCoord.z;
															// calculating  vertexnormals.
		thisVertexPtr++;
	}

	// - - - - - - - - - - - - - - - - - - - - - 
	// read the normals .. 
	// - - - - - - - - - - - - - - - - - - - - - 

	Xmulder_Normal  mulderNormal;

	vector3 * vertexNormalList = new vector3 [ mulderHeader.normalcount ];
	vector3 * thisvertexNormal = vertexNormalList;

	for (i = mulderHeader.normalcount; i--; )
	{
		//MYfread(&mulderNormal, sizeof(Xmulder_Normal), 1, mulderFP );
		arcRead(  (char * )&mulderNormal, sizeof(Xmulder_Normal), texhandle);

		thisvertexNormal->x = mulderNormal.x;
		thisvertexNormal->y = mulderNormal.y;
		thisvertexNormal->z = mulderNormal.z;
		thisvertexNormal++;
	}


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//  TEXTURE AND MATERIAL
	//
	//	- we maintain a global store of textures so that every texture file is held once only in memory, even when
	//	  the texture is used from different materials in different mesh models.
	//
	//  - a particular model's material's local texture index is mapped to the appropriate ptr. in the global
	//	  texture store.
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	textureData ** indexToPtr    = new textureData * [mulderHeader.texturecount];
	textureData ** myindexToPtr = indexToPtr;


	for (i = mulderHeader.texturecount; i--; )
	{
		//MYfread(&mulderTexture, sizeof(Xmulder_Texture), 1, mulderFP );
		arcRead(  (char * )&mulderTexture, sizeof(Xmulder_Texture), texhandle);

		*(myindexToPtr++) = getTexturePtr( mulderTexture.filename, false, uniqueTextures );

	}

	meshData->mcount = mulderHeader.materialcount;
	meshData->mlistPtr = new material[mulderHeader.materialcount]; 
	material * thisMatPtr = meshData->mlistPtr;

	for (i = 0; i<mulderHeader.materialcount; i++ )
	{
		arcRead(  (char * )&mulderMaterial, sizeof(Xmulder_Material), texhandle);

		thisMatPtr->ambient = mulderMaterial.ambient;
		thisMatPtr->diffuse = mulderMaterial.diffuse;
		thisMatPtr->specular = mulderMaterial.specular;
		thisMatPtr->emissive = mulderMaterial.emissive;
		thisMatPtr->power  = (ulong) mulderMaterial.power;

		//
		// So now lets scan for the material identifier string and update flags in BorisMaterial ready for
		// the renderer to use!!
		//
			
		static int debugcount=0;


		thisMatPtr->bm.textureHandle = 0;

		thisMatPtr->bm.flags = 0;
		thisMatPtr->bm.type = MATTYPE_DEFAULT;

		{
			// - no material file so let's interpret the name....
				
			long l = strlen(mulderMaterial.name);
			long nl;

			if( l ) //>= 6)
			{
				char * subptr = mulderMaterial.name;	// don't ask.
				nl = 0;
				while( *subptr) //endptr )
				{
					if(*subptr == '_' && (subptr - mulderMaterial.name)>2)
					{
						*subptr = 0;
						l = nl;
						break;
					}
					
					subptr++;
					nl++;
				}
			}

			if( l >= 7 )
			{
				ulong flagmask = hexto32bit( &mulderMaterial.name[l - 11], 8);

				if( flagmask & (1<<19) )
				{
					thisMatPtr->bm.flags |= MAT_BLACKFLAG;
				}
				if( flagmask & (1<<18) )
				{
					thisMatPtr->bm.flags |= MAT_REVLIMIT;
				}

				if( flagmask & (1<<26) ) thisMatPtr->bm.flags |= MAT_DARKENCAR;
				if( flagmask & (1<<25) ) thisMatPtr->bm.flags |= MAT_INVISIBLE;
				if( flagmask & (1<<24) ) thisMatPtr->bm.flags |= MAT_REVERBLO;
				if( flagmask & (1<<23) ) thisMatPtr->bm.flags |= MAT_REVERBHI;
				if( flagmask & (1<<22) )
				{
					thisMatPtr->bm.flags |= MAT_PITSTOP;
				}


				if( flagmask & (1<<21) ) thisMatPtr->bm.flags |= MAT_GROUND;

				if( flagmask & (1<<15) ) thisMatPtr->bm.flags |= MAT_DOUBLESIDED;
				if( flagmask & (1<<14) ) thisMatPtr->bm.flags |= MAT_SCLAMP;
				if( flagmask & (1<<13) ) thisMatPtr->bm.flags |= MAT_TCLAMP;
				if( flagmask & (1<<12) ) thisMatPtr->bm.flags |= MAT_ENVIRONMENT;
				if( flagmask & (1<<11) ) thisMatPtr->bm.flags |= MAT_DECAL;
				if( flagmask & (1<<10) ) 
				{
					thisMatPtr->bm.flags |= MAT_COLLIDABLE;
					thisMatPtr->bm.flags |= MAT_WALL;

				}
				if( flagmask & (1<<9) ) thisMatPtr->bm.flags |= MAT_WATER;
				if( flagmask & (1<<8) ) thisMatPtr->bm.flags |= MAT_SHADOW;
				if( flagmask & (1<<7) ) thisMatPtr->bm.flags |= MAT_DETAILMAP;

				if( flagmask & (1<<5) ) thisMatPtr->bm.flags |= MAT_OCCLUDE;

				if( flagmask & (1<<4) ) 
					thisMatPtr->bm.flags |= MAT_NOCLIP;
					
				if( flagmask & (1<<3) ) thisMatPtr->bm.flags |= MAT_MAXZ;
				if( flagmask & (1<<2) ) thisMatPtr->bm.flags |= MAT_MAXZM1;

				if( flagmask & (1<<0) ) thisMatPtr->bm.flags |= MAT_WALL;

			}

			// the last three digits contain the major type of the material

			if ( l >= 3)
				thisMatPtr->bm.type = hexto32bit(&mulderMaterial.name[l-3],3);

		}
		
		if( mulderMaterial.texture == 32000 )		// No texture
		{
			thisMatPtr->textureDataPtr = NULL;
			thisMatPtr->sortingKey     = GBLmaterialIndex << 13;			// No texture part (0)

		}
		else
		{

			// this '=' IS correct
			if( thisMatPtr->textureDataPtr = indexToPtr[  mulderMaterial.texture ])
			{
				thisMatPtr->bm.textureHandle = thisMatPtr->textureDataPtr->textureHandle;
	
				if( isTextureAlpha(thisMatPtr->bm.textureHandle) > 1 )	// If the materials file is OK we dont' need this!!
					thisMatPtr->bm.flags |= MAT_ALPHA;

				thisMatPtr->sortingKey     = ((thisMatPtr->textureDataPtr->tindex) << (9+13) ) + (GBLmaterialIndex << 13);

			} 
			else
			{
				thisMatPtr->bm.textureHandle = 0;
			}
		}

		// hack to fix the sky

		if(!strncmp("sky",filename,3))
			thisMatPtr->bm.flags |= (MAT_TCLAMP|MAT_SCLAMP);

		if(thisMatPtr->textureDataPtr)
			thisMatPtr->bm.flags |= thisMatPtr->textureDataPtr->flags;

		if( thisMatPtr->bm.flags & MAT_SHADOW ) //thisMatPtr->bm.flags &= (0x0FFFFFFFF ^ MAT_ALPHA);
		{
			thisMatPtr->bm.flags &= (0x0FFFFFFFF ^ ( MAT_ALPHA | MAT_ENVIRONMENT ) );
		}

		if( thisMatPtr->textureDataPtr)
			thisMatPtr->diffuse = 0x0C0C0C0;

		GBLmaterialIndex++;
		thisMatPtr++;

//DBGR.Dprintf(" loadMulderFile 9z\n");

	}



	delete[] indexToPtr;		// no longer need this array  ... all indices mapped to the appropriate texture data ptrs.

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	// read in all triangles ... only initialise the fields which need to be
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

	vector3  tempvectorr, tempvectorq;

	LPWORD *indBuf;
	WORD* indCount;
	WORD ind0, ind1, ind2;

	// allocate temporary memory for vertices - worst case is tricount*3
	jjsVert = new D3DVERTEXC[meshData->tcount * 3];
	numjjsVert=0;

	indBuf = new LPWORD[meshData->mcount];
	for(i=0; i<meshData->mcount; ++i)
		indBuf[i] = new WORD[meshData->tcount * 3];
	indCount = new WORD[meshData->mcount];
	memset(indCount,0,sizeof(WORD) * meshData->mcount);

	for (i = 0; i < meshData->tcount; i++ )
	{
		//MYfread(&mulderTriangle, sizeof(Xmulder_Triangle), 1, mulderFP );
		arcRead(  (char * )&mulderTriangle, sizeof(Xmulder_Triangle), texhandle);

		thisTrianglePtr->p.v    = meshData->vlistPtr + mulderTriangle.v[0];	// points to correct vertex in allocated list
		thisTrianglePtr->p.nv.u = mulderTriangle.ng[0].u;
		thisTrianglePtr->p.nv.v = mulderTriangle.ng[0].v;
		
		
		thisTrianglePtr->q.v    = meshData->vlistPtr + mulderTriangle.v[1];	// points to correct vertex in allocated list
		thisTrianglePtr->q.nv.u = mulderTriangle.ng[1].u;
		thisTrianglePtr->q.nv.v = mulderTriangle.ng[1].v;
		
		
		thisTrianglePtr->r.v    = meshData->vlistPtr + mulderTriangle.v[2];	// points to correct vertex in allocated list
		thisTrianglePtr->r.nv.u = mulderTriangle.ng[2].u;
		thisTrianglePtr->r.nv.v = mulderTriangle.ng[2].v;
		
		
		thisTrianglePtr->mtl = &meshData->mlistPtr[mulderTriangle.mat];
				
		thisTrianglePtr->faceNormal.x = mulderTriangle.faceNormal.x;
		thisTrianglePtr->faceNormal.y = mulderTriangle.faceNormal.y;
		thisTrianglePtr->faceNormal.z = mulderTriangle.faceNormal.z;
		thisTrianglePtr->faceNormal.Normalise();
		
		//
		// Calculate D value by substituting one vertex into the plane equation using the facenormal for (A,B,C)
		//

		vector3 *j, *k, *l;

		j = &thisTrianglePtr->p.v->geoCoord;
 		k = &thisTrianglePtr->q.v->geoCoord;
		l = &thisTrianglePtr->r.v->geoCoord;

		float xkj = k->x - j->x;
		float ykj = k->y - j->y;
		float zkj = k->z - j->z;
		float xlj = l->x - j->x;
		float ylj = l->y - j->y; 
		float zlj = l->z - j->z;
	
		thisTrianglePtr->A = ykj * zlj - zkj * ylj;
		thisTrianglePtr->B = zkj * xlj - xkj * zlj;
		thisTrianglePtr->C = xkj * ylj - ykj * xlj;

		static vector3 temp;
		static char zzz[200];
		temp.x = thisTrianglePtr->A; temp.y = thisTrianglePtr->B; temp.z = thisTrianglePtr->C;
		temp.Normalise();
		//sprintf(zzz,"???? %f %f %f      %f %f %f",thisTrianglePtr->faceNormal.x, thisTrianglePtr->faceNormal.y, thisTrianglePtr->faceNormal.z,
		//									 temp.x, temp.y, temp.z );
//DBGR.Dprintf(zzz);		
		thisTrianglePtr->faceNormal = temp;

		thisTrianglePtr->planeD = -(float)(( thisTrianglePtr->A * k->x +
											 thisTrianglePtr->B * k->y +
											 thisTrianglePtr->C * k->z));

		if( thisTrianglePtr->B ) 
			thisTrianglePtr->minusOneOverB = - 1.0f/ thisTrianglePtr->B ;//mulderTriangle.faceNormal.y); 

		thisTrianglePtr->ABC   =
			1.0f / ( thisTrianglePtr->A * thisTrianglePtr->A + 
						thisTrianglePtr->B * thisTrianglePtr->B + 
						thisTrianglePtr->C * thisTrianglePtr->C );

		minmax3( thisTrianglePtr->p.v->geoCoord.x,
				 thisTrianglePtr->q.v->geoCoord.x,
				 thisTrianglePtr->r.v->geoCoord.x, &thisTrianglePtr->minLimit.x, &thisTrianglePtr->maxLimit.x );
		minmax3( thisTrianglePtr->p.v->geoCoord.y,
				 thisTrianglePtr->q.v->geoCoord.y,
				 thisTrianglePtr->r.v->geoCoord.y, &thisTrianglePtr->minLimit.y, &thisTrianglePtr->maxLimit.y );
		minmax3( thisTrianglePtr->p.v->geoCoord.z,
				 thisTrianglePtr->q.v->geoCoord.z,
				 thisTrianglePtr->r.v->geoCoord.z, &thisTrianglePtr->minLimit.z, &thisTrianglePtr->maxLimit.z );

//		thisTrianglePtr->faceNormalOctant = normalOctantCode(&thisTrianglePtr->faceNormal);

		thisTrianglePtr->p.v->vertexNormal = vertexNormalList[ mulderTriangle.n[0] ];
		thisTrianglePtr->q.v->vertexNormal = vertexNormalList[ mulderTriangle.n[1] ];
		thisTrianglePtr->r.v->vertexNormal = vertexNormalList[ mulderTriangle.n[2] ];

		ind0 = Addvertex(thisTrianglePtr->p);
		ind1 = Addvertex(thisTrianglePtr->q);
		ind2 = Addvertex(thisTrianglePtr->r);

		indBuf[mulderTriangle.mat][indCount[mulderTriangle.mat]++] = ind0;
		indBuf[mulderTriangle.mat][indCount[mulderTriangle.mat]++] = ind1;
		indBuf[mulderTriangle.mat][indCount[mulderTriangle.mat]++] = ind2;

		thisTrianglePtr->pcnt = 0;

		thisTrianglePtr->centroid = ( *j + *k + *l ) * 0.3333333f;
		float tempf;
		thisTrianglePtr->boundingRadiusSquared = j->distanceSquared(&thisTrianglePtr->centroid),
		tempf = k->distanceSquared(&thisTrianglePtr->centroid);
		if( tempf > thisTrianglePtr->boundingRadiusSquared ) thisTrianglePtr->boundingRadiusSquared = tempf;
		tempf = l->distanceSquared(&thisTrianglePtr->centroid);
		if( tempf > thisTrianglePtr->boundingRadiusSquared ) thisTrianglePtr->boundingRadiusSquared = tempf;
					
		thisTrianglePtr->boundingRadius = sqrtf(thisTrianglePtr->boundingRadiusSquared);

		thisTrianglePtr++;
	}
//DBGR.Dprintf("OH HO 9");
	
//DBGR.Dprintf("B8");

//DBGR.Dprintf(" loadMulderFile 12\n");


	//
	// Finished loading the Mulder File World in.
	//

	delete[] vertexNormalList;

	arcDispose( texhandle );

	//
	// Now load in the sphere set file IF ONE EXISTS!!!!!!
	//
//DBGR.Dprintf(" loadMulderFile 13\n");

	meshData->spheres = NULL;	
	if( SSname && strlen(SSname ) )
	{
		meshData->spheres = new SphereSet(SSname, scale);
		
	}
	
	meshData->indices = new LPWORD[meshData->mcount];
	meshData->indcount = new WORD[meshData->mcount];
	
	meshData->vertBuf = new D3DVERTEXC[numjjsVert];
	
	memcpy(meshData->vertBuf,jjsVert,sizeof(D3DVERTEXC) * numjjsVert);
	
	meshData->vbcount = numjjsVert;
	
	for(i=0;i<meshData->mcount; ++i)
	{
		meshData->indices[i] = new WORD[indCount[i]];
		memcpy(meshData->indices[i], indBuf[i], indCount[i] * sizeof(WORD));
		meshData->indcount[i] = indCount[i];
		delete indBuf[i];
	}
	
	// Clear up temporary buffers
	delete[] jjsVert;
	delete[] indBuf;
	delete[] indCount;
}
