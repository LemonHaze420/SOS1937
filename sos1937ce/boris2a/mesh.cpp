// $Header$

// $Log$
// Revision 1.3  2000-03-06 15:17:00+00  jjs
// Removed texture deleter.
//
// Revision 1.2  2000-01-27 16:39:10+00  jjs
// Allow textures to be delted on mesh deletion.
//
// Revision 1.1  2000-01-13 17:27:05+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

mesh::mesh()
{
	indcount = NULL;
	indices  = NULL;
	vertBuf	 = NULL;
	tlistPtr = NULL;
	vlistPtr = NULL;
	mlistPtr = NULL;
	geoDataPtr=NULL;
}

mesh::~mesh()
{
	if (indcount) 
	{
		delete[] indcount;
		indcount=NULL;
	}
	if (indices) 
	{
		delete[] indices;
		indices=NULL;
	}
	if (vertBuf) 
	{
		delete[] vertBuf;
		vertBuf=NULL;
	}
	if (tlistPtr) 
	{
		delete[] tlistPtr;
		tlistPtr=NULL;
	}
	if (vlistPtr) 
	{	
		delete[] vlistPtr;
		vlistPtr=NULL;
	}
	if (mlistPtr) 
	{
//		for(int i = 0 ; i < mcount ; ++i)
//			if(mlistPtr[i].textureDataPtr)
//				removeTexture(mlistPtr[i].textureDataPtr);
		delete[] mlistPtr;
		mlistPtr=NULL;
	}
	if( geoDataPtr )
	{
		delete[] geoDataPtr;
		geoDataPtr=NULL;
	}
}

void mesh::copyfrom(mesh *m)
{
	if(m->tlistPtr)
	{
		tcount = m->tcount;
		tlistPtr = new triangle [tcount];
		memcpy((void *)tlistPtr,(const void *)m->tlistPtr,sizeof(triangle)*tcount);
	}

	if(m->vlistPtr)
	{
		vcount = m->vcount;
		vlistPtr = new vertex[vcount];
		memcpy((void *)vlistPtr,(const void *)m->vlistPtr,sizeof(vertex)*vcount);
	}

	if(m->mlistPtr)
	{
		mcount = m->mcount;
		mlistPtr = new material [mcount];
		memcpy((void *)mlistPtr,(const void *)m->mlistPtr,sizeof(material)*mcount);
	}


	// v3list copy todo 
}
