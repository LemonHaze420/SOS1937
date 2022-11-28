//================================================================================================================================
// rGraphics.h
// -----------
//
//================================================================================================================================

#ifndef _BS_rGraphics
#define _BS_rGraphics

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

class rTriangle {
public:
	float x0,y0,z0;
	float x1,y1,z1;
	float x2,y2,z2;
};

#define MAT_ENVIRONMENT (1<<25)
#define MAT_ALPHA		(1<<26)
#define MAT_COLLIDABLE	(1<<27)
#define MAT_DOUBLESIDED (1<<28)
#define MAT_WALL	    (1<<29)
#define MAT_GRASS	    (1<<30)
#define MAT_TRACK		(1<<31)


#define MAT_TCLAMP 1
#define MAT_SCLAMP 2
#define MAT_DECAL  4

class BorisMaterial {
public:
	DWORD flags;
	DWORD textureHandle;
};

#define D3DFVF_TLVERTEX2 ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX2)

typedef struct _D3DTLVERTEX2 {
    D3DVALUE    sx;
    D3DVALUE    sy;
    D3DVALUE    sz;
    D3DVALUE    rhw;        /* Reciprocal of homogeneous w */
    D3DCOLOR    color;          /* Vertex color */
    D3DCOLOR    specular;       /* Specular component of vertex */
    D3DVALUE    tu1;             /* Texture coordinates */
    D3DVALUE    tv1;
    D3DVALUE    tu2;             /* Texture coordinates */
    D3DVALUE    tv2;
} D3DTLVERTEX2, *LPD3DTLVERTEX2;

BOOL renderInitialise(HWND hWnd, BOOL b_fullScreen=FALSE, BOOL Voodoo=FALSE, BOOL softwareOnly=FALSE);
void *renderLockSurface(int *pitch);
void renderUnlockSurface(void);
void renderSetCurrentMaterial(int r, int g, int b);
void renderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2 = NULL);
void renderTriangle(rTriangle* tri);
void renderList(D3DTLVERTEX* list,int numVert);
void renderList(D3DTLVERTEX2* list,int numVert);
void renderFlipSurface(void);
void renderBeginScene(BOOL clear);
void renderEndScene(void);
void renderDeInitialise(void);
void renderMove(LPARAM lParam);
DWORD registerTexture(char *pointerToFile);
void unregisterTexture(DWORD handle);
BOOL isTextureAlpha(DWORD handle);


#endif	// _BS_rGraphics

//================================================================================================================================
//END OF FILE
//================================================================================================================================



