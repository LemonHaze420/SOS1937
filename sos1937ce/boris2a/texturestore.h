// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:27:51+00  jjs
// First version that supports the loading of objects.
//

#ifndef _BS_textureStore
#define _BS_textureStore


class textureData{

public:
	char * filename,*origfilename;
	textureData * next;
	DWORD textureHandle;
	ulong tindex;
	ulong flags;
	int	usageCount;			// Number of instances of texture.
};

void startTextureStore();
//textureData * getTexturePtr( char * filename );
textureData * getTexturePtr( char * filename, bool environmentMap, bool uniqueTextures = false, bool Bump = false);
void removeTexture(textureData* tex);
void removeTextureStore();


#endif	// _BS_textureStore
