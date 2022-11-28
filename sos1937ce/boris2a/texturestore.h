//================================================================================================================================
// textureStore.h
// --------------
//
//		Holding of all the textures in the system. Each texture filename stored for the appropriate index,
//================================================================================================================================

#ifndef _BS_textureStore
#define _BS_textureStore


class textureData{

public:
	char * filename;
	textureData * next;
	DWORD textureHandle;
	ulong tindex;
	ulong flags;
};

void startTextureStore();
//textureData * getTexturePtr( char * filename );
textureData * getTexturePtr( char * filename, bool environmentMap, bool uniqueTextures = false);
void removeTextureStore();


#endif	// _BS_textureStore

//================================================================================================================================
//END OF FILE
//================================================================================================================================
