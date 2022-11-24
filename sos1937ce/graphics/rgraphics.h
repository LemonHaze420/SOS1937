// $Header$

// $Log$
// Revision 1.15  2000-05-08 18:25:04+01  jcf
// Lighting on cars, fixed.
//
// Revision 1.14  2000-05-07 17:31:29+01  jcf
// Lighting on cars
//
// Revision 1.13  2000-05-06 14:08:43+01  jjs
// Ads new Flip function.
//
// Revision 1.12  2000-04-19 09:30:21+01  jcf
// Zback support
//
// Revision 1.11  2000-04-13 15:23:07+01  jjs
// Adds support for movie player.
//
// Revision 1.10  2000-04-10 16:26:25+01  jjs
// PC compatible version.
//
// Revision 1.9  2000-04-03 15:05:51+01  jjs
// Adds DarkenScreen function.
//
// Revision 1.8  2000-03-20 11:35:13+00  jjs
// New prototypes added.
//
// Revision 1.7  2000-03-09 11:14:23+00  jjs
// Added renderReset to fix Alpha problem.
//
// Revision 1.6  2000-03-06 12:53:04+00  jjs
// Modified to add renderDeinitialise to Dreamcast version.
//
// Revision 1.5  2000-02-29 14:22:05+00  jjs
// Latest version with STRICT typing.
//
// Revision 1.4  2000-02-07 12:21:01+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.3  2000-01-19 14:52:55+00  jjs
// Pass the Projection matrix out.
//
// Revision 1.2  2000-01-17 12:39:50+00  jjs
// Alpha test fixed.
//
// Revision 1.1  2000-01-13 17:29:22+00  jjs
// First version that supports the loading of objects.
//

#ifndef _BS_rGraphics
#define _BS_rGraphics

extern LPDIRECT3DDEVICE3		g_pd3dDevice;
extern LPDIRECTDRAW4			g_pDD4;
extern LPDIRECTDRAWSURFACE4		g_pddsPrimary;
extern LPDIRECTDRAWSURFACE4		g_pddsBackBuffer;
extern D3DMATRIX ProjectionMatrix;

typedef enum {MAP_NOMAP, MAP_ENVIRONMENT, MAP_DETAIL} MAP_TYPE;
typedef enum {CLOSE_ALL=0xff,CLOSE_2D=1,CLOSE_3D=2} CLOSE_TYPE;

typedef enum {RGB_UNSUPPORTED,RGB_555,RGB_565} RGB_TYPE;

class BorisMaterial {
public:
	DWORD flags;
	DWORD textureHandle;
	DWORD type;
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
#define	MAT_GROUND		(1<<10)

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

#define D3DFVF_VERTEXC ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define D3DFVF_TLVERTEXDARK ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

typedef struct _D3DVERTEXC {
    D3DVALUE    x;
    D3DVALUE    y;
    D3DVALUE    z;
    D3DVALUE    nx;
    D3DVALUE    ny;
    D3DVALUE    nz;
    D3DCOLOR    color;          /* Vertex color */
    D3DVALUE    tu;             /* Texture coordinates */
    D3DVALUE    tv;
} D3DVERTEXC, *LPD3DVERTEXC;

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

#if defined(UNDER_CE)
void renderInitialise(HWND hwnd, bool Only3D=false);
#else
void renderInitialise(HWND hwnd, bool b_fullscreen = false);
#endif
void renderDeInitialise(CLOSE_TYPE flag=CLOSE_ALL);
DWORD isTextureAlpha(DWORD handle);
DWORD registerTexture(char *pointerToFile,int TextureStage=0, int *width=NULL, int *height=NULL);
void unregisterTexture(DWORD handle);
void renderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2 = NULL, MAP_TYPE maptype = MAP_NOMAP,BorisMaterial *m3 = NULL);
void renderBeginScene(bool clear,DWORD rgb=0x649dc4);
void renderEndScene(void);
void renderFlipSurface(bool halfRate=false);
BOOL AddLight(float rRed, float rGreen, float rBlue, float rX, float rY, float rZ);
DWORD renderGetCaps(void);
void renderList(D3DTLVERTEX* list,int numVert,BOOL clip=FALSE);
void renderList(D3DTLVERTEX2* list,int numVert,BOOL clip=FALSE);
void renderLine(D3DTLVERTEX* list,int numVert);
void renderReset(void);
bool ReleaseLight(void);
void SetLightDirection(float x,float z);
void SetLightState(int r, int g, int b);
void SetLightColour(float r,float g,float b);
void DarkenScreen(int sx,int sy, int width, int height, D3DCOLOR val=0xa0202040,float zval=DARKZ);
LPDIRECTDRAWSURFACE4 LoadSSB(char *pointer);
LPDIRECTDRAWSURFACE4 getSurface(DWORD handle);
float SetZBack(float zback);
void SetDisplayHz(bool Hz50);
#endif