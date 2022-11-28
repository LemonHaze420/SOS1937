//================================================================================================================================
// softTextures.h
// --------------
//
//================================================================================================================================

#ifndef _BS_softTextures
#define _BS_softTextures

#include <stdio.h>

void SOFTrenderStart3D();
void SOFTrenderSetCurrentMaterial(int r, int g, int b);
void SOFTrenderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2 = NULL);
void SOFTrenderTriangle(rTriangle* tri);		// REDUNDANT FUNCTION
void SOFTrenderList(D3DTLVERTEX* list,int numVert,BOOL wireframe=FALSE,BOOL clip=FALSE);
void SOFTrenderList(D3DTLVERTEX2* list,int numVert,BOOL wireframe=FALSE,BOOL clip=FALSE);
DWORD SOFTregisterTexture(unsigned char *pointerToFile,int TextureStage=0, int *width=NULL, int *height=NULL);
void SOFTunregisterTexture(DWORD handle);
DWORD SOFTisTextureAlpha(DWORD handle);
void SOFTSetFogColor(DWORD color);
void SOFTrenderFog(BOOL fog);
void SOFTrenderBeginScene( bool clearscreen );
#endif	// _BS_softTextures

//================================================================================================================================
//END OF FILE
//================================================================================================================================
