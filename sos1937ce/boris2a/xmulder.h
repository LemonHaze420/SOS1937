// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:27:53+00  jjs
// First version that supports the loading of objects.
//

//================================================================================================================================
// Xmulder.h
// --------
//
//		file will always be:
//							mulder_Header
//		followed by:		mulder_VertexGEO ....			(all geometric vertices)
//		followed by:		mulder_Normal ...				(all normals)
//		followed by:		mulder_Texture ...				(all textures)
//		followed by			mulder_Material ...				(all materials)
//		followed by:		mulder_Triangle ...				(all triangles)
//		
//
//		All geometric co-ordinates ( mulder_VertexGEO ) are shared, therefore the same values can NEVER occur in two
//		'mulder_vertexGEO' items.
//================================================================================================================================

#ifndef _BS_Xmulder
#define _BS_Xmulder

struct MaterialCode{
	char identifier[64];
	ulong flags;
	MaterialCode * next;
};


struct Xmulder_Header {
	unsigned long magic;
	char name[64];
	float minx,miny,minz,maxx,maxy,maxz;
	long tricount;							// total number of triangles
	long vertexcount;						// total number of geometric vertices
	long texturecount;						// total number of textures
	long materialcount;					// total number of materials
	long normalcount;
};

struct Xmulder_VertexGEO {
	float x,y,z;
};

struct Xmulder_VertexNONGEO {
	float u,v;
};

struct Xmulder_Normal {
	float x,y,z;
};

struct Xmulder_Texture {
	char filename[32];
	unsigned long usize, vsize;			// hold the texture dimensions
};

struct Xmulder_Material {
	unsigned long ambient;
	unsigned long diffuse;
	unsigned long specular;
	unsigned long emissive;
	unsigned short power;
	unsigned short texture;
	char name[64];
};


struct Xmulder_Triangle {
	unsigned long v[3];					// indices to array of mulder_vertexGEO		( geometric coordinates )
	unsigned long n[3];					// indices to array of mulder_Normal		( vertex normals )
	struct Xmulder_VertexNONGEO ng[3];			// nongeometric coordinates are NOT shared 
	struct Xmulder_Normal faceNormal;
	unsigned short mat;
};

#endif	// _BS_Xmulder


#define	FHEADER_MAGIC	0x23786542



void loadMulderFile( char * filename, mesh * meshdata, float scale, char * SSname, bool uniqueTextures);

void initialiseModelLoading( void );
