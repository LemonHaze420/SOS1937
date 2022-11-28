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

extern LPDIRECTDRAWSURFACE7 g_pddsPrimary;
extern LPDIRECTDRAWSURFACE7 g_pddsBackBuffer;
extern LPDIRECTDRAW7				g_pDD7;
extern LPDIRECT3DDEVICE7			g_pd3dDevice;

typedef enum {RGB_UNSUPPORTED,RGB_555,RGB_565} RGB_TYPE;
typedef enum {CLOSE_ALL=0xff,CLOSE_2D=1,CLOSE_3D=2} CLOSE_TYPE;
typedef enum {MAP_NOMAP, MAP_ENVIRONMENT, MAP_DETAIL} MAP_TYPE;
extern  RGB_TYPE Format16;

class rTriangle {
public:
	float x0,y0,z0;
	float x1,y1,z1;
	float x2,y2,z2;
};

#define DARKZ 0.0018f

#define MAT_TCLAMP (1<<0)
#define MAT_SCLAMP (1<<1)
#define MAT_DECAL  (1<<2)
#define	MAT_DARKENCAR	(1<<3)
#define	MAT_REVERBHI	(1<<4)
#define	MAT_REVERBLO	(1<<5)
#define	MAT_PITSTOP		(1<<6)
#define	MAT_INVISIBLE		(1<<7)
#define	MAT_REVLIMIT		(1<<8)
#define	MAT_BLACKFLAG		(1<<9)

#define MAT_NOBLEND		(1<<15)
#define MAT_DETAILMAP	(1<<16)
#define MAT_MAXZM1 (1<<17)
#define MAT_MAXZ (1<<18)
#define MAT_NOCLIP (1<<19)
#define MAT_OCCLUDE		(1<<20)
#define MAT_DRIVINGAIDS (1<<21)
#define MAT_WATER		(1<<23)
#define MAT_SHADOW		(1<<24)
#define MAT_ENVIRONMENT (1<<25)
#define MAT_ALPHA		(1<<26)
#define MAT_COLLIDABLE	(1<<27)
#define MAT_DOUBLESIDED (1<<28)
#define MAT_WALL	    (1<<29)    // used for identifying triangles for collision


class BorisMaterial {
public:
	DWORD flags;
	DWORD textureHandle;
	DWORD type;
};

#define	MATTYPE_CAR				0x1
#define	MATTYPE_MISC			0x2
#define	MATTYPE_TREE			0x3
#define	MATTYPE_BUSH			0x4
#define	MATTYPE_SIGN			0x5
#define	MATTYPE_BUILDING		0x6
#define	MATTYPE_METALFENCE		0x7
#define	MATTYPE_WOODFENCE		0x8
#define	MATTYPE_FOLIAGEFENCE	0x9
#define	MATTYPE_CONCRETEFENCE	0xa
#define	MATTYPE_KERB			0xb
#define	MATTYPE_ROCKFACE		0xc
#define	MATTYPE_CROWD			0xd
#define	MATTYPE_COBBLES			0xe
#define	MATTYPE_ROUGHCONCRETE	0xf
#define	MATTYPE_MEDIUMCONCRETE	0x10
#define	MATTYPE_SMOOTHCONCRETE	0x11
#define	MATTYPE_PLANKS			0x12
#define	MATTYPE_DUST			0x13
#define	MATTYPE_TARMAC			0x14
#define	MATTYPE_GRAVEL			0x15
#define	MATTYPE_SAND			0x16
#define	MATTYPE_GRAVELTRAP		0x17
#define	MATTYPE_SANDTRAP		0x18
#define	MATTYPE_DEEPSANDTRAP	0x19
#define	MATTYPE_SANDBAGS		0x1a
#define	MATTYPE_STRAWBALES		0x1b
#define	MATTYPE_RUBBER			0x1c
#define	MATTYPE_MUD				0x1d
#define	MATTYPE_ROCK			0x1e
#define	MATTYPE_SHORTGRASS		0x1f
#define	MATTYPE_LONGGRASS		0x20
#define	MATTYPE_ICE				0x21
#define	MATTYPE_FRESHSNOW		0x22
#define	MATTYPE_MELTEDSNOW		0x23
#define	MATTYPE_DRIFTEDSNOW		0x24
#define	MATTYPE_CLOTH			0x25
#define	MATTYPE_DRIVERAIDS		0x26

#define	MATTYPE_DEFAULT	MATTYPE_SHORTGRASS


#define D3DFVF_TLVERTEX2 ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX2)
#define D3DFVF_TLVERTEX3 ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | \
                          D3DFVF_TEX3)

#define D3DFVF_TLVERTEXDARK ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

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

typedef struct _D3DTLVERTEX3 {
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
    D3DVALUE    tu3;             /* Texture coordinates */
    D3DVALUE    tv3;
} D3DTLVERTEX3, *LPD3DTLVERTEX3;

typedef struct _D3DTLVERTEXDARK {
    D3DVALUE    sx;
    D3DVALUE    sy;
    D3DVALUE    sz;
    D3DVALUE    rhw;        /* Reciprocal of homogeneous w */
    D3DCOLOR    color;          /* Vertex color */
} D3DTLVERTEXDARK, *LPD3DTLVERTEXDARK;

typedef struct _VIDEOMODES {
	int width,height,depth;
} VIDEOMODES, *LPVIDEOMODES;

BOOL renderInitialise(HWND hWnd, BOOL b_fullScreen=FALSE, BOOL Voodoo=FALSE, BOOL softwareOnly=FALSE);
BOOL renderStart3D(void);
BOOL renderSetupSurfaces(void);
void *renderLockSurface(int *pitch);
void renderUnlockSurface(void);
void renderSetCurrentMaterial(int r, int g, int b);
void renderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2 = NULL, MAP_TYPE maptype = MAP_NOMAP,BorisMaterial *m3 = NULL);
void renderTriangle(rTriangle* tri);
void renderList(D3DTLVERTEX* list,int numVert,BOOL wireframe=FALSE,BOOL clip=FALSE);
void renderLine(D3DTLVERTEX* list,int numVert);
void renderList(D3DTLVERTEX2* list,int numVert,BOOL wireframe=FALSE,BOOL clip=FALSE);
void renderIndexedList(D3DTLVERTEX* list,int numVert,LPWORD Indices, int numIndices,BOOL wireframe=FALSE,BOOL clip=FALSE);
void renderIndexedList(D3DTLVERTEX2* list,int numVert,LPWORD Indices, int numIndices,BOOL wireframe=FALSE,BOOL clip=FALSE);
void renderFlipSurface(void);
void renderBeginScene(BOOL clear,DWORD rgb=0x649dc4);
void renderEndScene(void);
void renderDeInitialise(CLOSE_TYPE flag=CLOSE_ALL);
void renderMove(LPARAM lParam);
DWORD registerTexture(char *pointerToFile,int TextureStage=0, int *width=NULL, int *height=NULL);
DWORD registerTextureFile(char *pointerToFile,int TextureStage=0, int *width=NULL, int *height=NULL);
DWORD registerBump(char *pointerToFile, int *width=NULL, int *height=NULL);
void unregisterTexture(DWORD handle);
DWORD isTextureAlpha(DWORD handle);
void EnableZBuffer(BOOL flag);
void EnableZWrite(BOOL flag);
LPVIDEOMODES GetVideoModes(void);
BOOL SetVideoMode(int num);
BOOL ResetVideoMode(int num);
void SetFogColor(DWORD color);
void renderFog(BOOL fog);
void DarkenScreen(int sx,int sy, int width, int height, D3DCOLOR val=0xa0000040,float zval=DARKZ);
void MirrorMask(int sx,int sy, int width, int height);
void MirrorMask(int* vert);
LPDIRECTDRAWSURFACE7 LoadSSB(char *filename, int *width=NULL, int *height=NULL, BOOL systemMemory = FALSE);
LPDIRECTDRAWSURFACE7 renderCreateSurface(int width,int height);
void renderFan(D3DTLVERTEX* list,int numVert);
void renderFan(D3DTLVERTEX2* list,int numVert);
void ClearZ(void);
#endif	// _BS_rGraphics

//================================================================================================================================
//END OF FILE
//================================================================================================================================



