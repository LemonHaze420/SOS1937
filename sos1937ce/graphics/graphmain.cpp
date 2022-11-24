// $Header$

// $Log$
// Revision 1.20  2000-05-09 18:23:51+01  jjs
// Allows 50/60Hz switch.
//
// Revision 1.19  2000-05-08 18:25:03+01  jcf
// Lighting on cars, fixed.
//
// Revision 1.18  2000-05-07 17:31:29+01  jcf
// Lighting on cars
//
// Revision 1.17  2000-05-06 14:08:31+01  jjs
// Uses fixed D3D and adds FLIP to half frame rate.
//
// Revision 1.16  2000-04-20 09:47:31+01  jcf
// Zback added
//
// Revision 1.15  2000-04-13 15:23:07+01  jjs
// Adds support for movie player.
//
// Revision 1.14  2000-04-10 16:26:24+01  jjs
// PC compatible version.
//
// Revision 1.13  2000-04-03 15:05:50+01  jjs
// Adds DarkenScreen function.
//
// Revision 1.12  2000-03-27 11:27:16+01  jjs
// Removed renderReset.
//
// Revision 1.11  2000-03-20 11:35:39+00  jjs
// No longer needs renderRest.
//
// Revision 1.10  2000-03-09 11:13:55+00  jjs
// Added renderReset to fix Alpha problem.
//
// Revision 1.9  2000-02-29 14:22:02+00  jjs
// Latest version with STRICT typing.
//
// Revision 1.8  2000-02-07 12:20:59+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.7  2000-01-27 16:39:12+00  jjs
// Allow textures to be delted on mesh deletion.
//
// Revision 1.6  2000-01-21 14:23:02+00  jjs
// Fixed 4444 texture blending.
//
// Revision 1.5  2000-01-19 14:52:40+00  jjs
// Pass the Projection Matrix out.
//
// Revision 1.4  2000-01-17 14:27:55+00  jjs
// <>
//
// Revision 1.3  2000-01-17 12:39:35+00  jjs
// Alpha testing fixed.
//
// Revision 1.2  2000-01-13 17:29:20+00  jjs
// First version that supports the loading of objects.
//
// Revision 1.1  1999-12-04 16:30:47+00  jjs
// Graphics routines for Dreamcast.
//

#define D3D_OVERLOADS
#define STRICT
#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#if defined(UNDER_CE)
#include "FloatMathLib.h"
#endif // UNDER_CE
#include "rgraphics.h"
#include "mipread.h"
#include "textures.h"
#if defined(UNDER_CE)
#include <shsgintr.h>
#endif
#if defined(LIBVERSION) || !defined(UNDER_CE)
#undef __declspec
#define __declspec(x)
#endif
#include "errorlib.h"
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECTDRAW4				g_pDD4			 = NULL;
LPDIRECTDRAWSURFACE4		g_pddsPrimary	 = NULL;
LPDIRECTDRAWSURFACE4		g_pddsBackBuffer = NULL;
static LPDIRECT3D3			g_pD3D			 = NULL;
LPDIRECT3DDEVICE3			g_pd3dDevice	 = NULL;
static LPDIRECT3DVIEWPORT3	g_pvViewport	 = NULL;
static HWND 				g_hWnd;
LPDIRECT3DMATERIAL3			g_pmatWhite = NULL;       // Global white material
LPDIRECT3DMATERIAL3			g_pmatBlack = NULL;       // Global black material
D3DMATERIALHANDLE			g_hmatWhite = NULL;       // Global white material handle
D3DMATERIALHANDLE			g_hmatBlack = NULL;       // Global black material handle
LPDIRECT3DLIGHT				g_rglight;    // Global D3D lights.
static RECT 				g_rcViewportRect;
#if !defined(UNDER_CE)
static LPDIRECTDRAWSURFACE4 g_pddsZBuffer	 = NULL;
static RECT 				g_rcScreenRect;
#endif

D3DTLVERTEXDARK darkvert[4];
D3DMATRIX ProjectionMatrix;
void SetTexture(int stage, LPDIRECT3DTEXTURE2 tex);
int mainHz = 0;

BOOL RealizeVQ (BorisTexture2 *tex, char* VQRowData);
#if !defined(UNDER_CE)
bool g_bAppUseFullScreen;
RGB_TYPE Format16;
DWORD Caps;
#endif

#define	PI	3.141592654f

#define TEXTURES_AVAILABLE 1000
BorisTexture2 textureHandles[TEXTURES_AVAILABLE]={NULL};
BorisTexture2 * Texture2 = NULL;

bool				CurrentAlphaTest=FALSE;
bool				CurrentAlphaBlend=FALSE;
bool				CurrentBlend = FALSE;
bool				CurrentZWrite = TRUE;
bool				CurrentTClamp[3]={FALSE,FALSE};
bool				CurrentSClamp[3]={FALSE,FALSE};
LPDIRECT3DTEXTURE2	CurrentTexture[3];
D3DFILLMODE			CurrentFillMode=D3DFILL_SOLID;

#if !defined(UNDER_CE)
void SetPixelFormat16(DDPIXELFORMAT *ddpixelformat)
{
	if(ddpixelformat->dwRBitMask == 0xf800 && ddpixelformat->dwGBitMask == 0x7e0)
		Format16= RGB_565;
	else
	if(ddpixelformat->dwRBitMask == 0x7c00 && ddpixelformat->dwGBitMask == 0x3e0)
		Format16= RGB_555;
	else
		Format16=RGB_UNSUPPORTED;
}
#endif

HRESULT Initialize2DEnvironment( HWND hWnd)
{
    LPDIRECTDRAW pdd;          // Temporary DirectDraw interface pointer

	g_hWnd = hWnd;

	DirectDrawCreate(NULL, &pdd, NULL);

	pdd->QueryInterface(IID_IDirectDraw4, (LPVOID*)&g_pDD4);

    pdd->Release();

	g_pDD4->QueryInterface( IID_IDirect3D3, (VOID**)&g_pD3D );

	return S_OK;
}

HRESULT InitializeSurfaces( HWND hWnd)
{
	DDSURFACEDESC2 ddsd;
	HRESULT hr;

#if defined(UNDER_CE)
	hr = g_pDD4->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
#else
	if(g_bAppUseFullScreen)
		hr = g_pDD4->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_FPUSETUP);
	else
		hr = g_pDD4->SetCooperativeLevel( hWnd, DDSCL_NORMAL|DDSCL_FPUSETUP);

	if(g_bAppUseFullScreen)
#endif
	{
	hr = g_pDD4->SetDisplayMode(640, 480, 16, mainHz, 0);

	SetRect( &g_rcViewportRect, 0, 0, 640, 480 );
#if !defined(UNDER_CE)
	memcpy( &g_rcScreenRect, &g_rcViewportRect, sizeof(RECT) );
#endif
	// Create the primary surface
    // Setup to create the primary surface with 1 back buffer
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    ddsd.dwBackBufferCount = 1;

    // Create the primary surface
	hr = g_pDD4->CreateSurface(&ddsd, &g_pddsPrimary, NULL);

    // Get a pointer to the back buffer
    ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    
	hr = g_pddsPrimary->GetAttachedSurface(&ddsd.ddsCaps, &g_pddsBackBuffer);
	}
#if !defined(UNDER_CE)
	else
	{
		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize 		= sizeof(DDSURFACEDESC2);
		ddsd.dwFlags		= DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		hr = g_pDD4->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
		if( FAILED( hr ) )
			return hr;

		// Setup a surface description to create a backbuffer. This is an
		// offscreen plain surface with dimensions equal to our window size.
		// The DDSCAPS_3DDEVICE is needed so we can later query this surface
		// for an IDirect3DDevice interface.
		ddsd.dwFlags		= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		// Set the dimensions of the backbuffer. Note that if our window changes
		// size, we need to destroy this surface and create a new one.
		GetClientRect( hWnd, &g_rcScreenRect );
		GetClientRect( hWnd, &g_rcViewportRect );
		ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.left );
		ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.right );
		ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;
	
		// Create the backbuffer. The most likely reason for failure is running
		// out of video memory. (A more sophisticated app should handle this.)
		hr = g_pDD4->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL );
		if( FAILED( hr ) )
			return hr;

		// Create a clipper object which handles all our clipping for cases when
		// our window is partially obscured by other windows. This is not needed
		// for apps running in fullscreen mode.
		LPDIRECTDRAWCLIPPER pcClipper;
		hr = g_pDD4->CreateClipper( 0, &pcClipper, NULL );
		if( FAILED( hr ) )
			return hr;
	
		// Associate the clipper with our window. Note that, afterwards, the
		// clipper is internally referenced by the primary surface, so it is safe
		// to release our local reference to it.
		pcClipper->SetHWnd( 0, hWnd );
		g_pddsPrimary->SetClipper( pcClipper );
		pcClipper->Release();
	}
	DDPIXELFORMAT ddpixelformat;

	memset( &ddpixelformat, 0, sizeof(ddpixelformat) );

	ddpixelformat.dwSize = sizeof( ddpixelformat );
	g_pddsPrimary->GetPixelFormat(&ddpixelformat);
	SetPixelFormat16(&ddpixelformat);
#endif
	return S_OK;
}

LPDIRECT3DMATERIAL3 
CreateMaterial(float rRed,   float rGreen, float rBlue, float rAlpha, float rPower, 
               float rSpecR, float rSpecG, float rSpecB, 
               float rEmisR, float rEmisG, float rEmisB)
{
    D3DMATERIAL         mat;        // Holds Material Information
    LPDIRECT3DMATERIAL3 pmat;       // Points to created material
	HRESULT hr;

    // Create the material
	hr = g_pD3D->CreateMaterial(&pmat, NULL);

    // Initialize the Material
    memset(&mat, 0, sizeof(D3DMATERIAL));
    mat.dwSize     = sizeof(D3DMATERIAL);
    mat.diffuse.r  = rRed;
    mat.diffuse.g  = rGreen;
    mat.diffuse.b  = rBlue;
    mat.diffuse.a  = rAlpha;
    mat.ambient.r  = rEmisR;
    mat.ambient.g  = rEmisG;
    mat.ambient.b  = rEmisB;
    mat.ambient.a  = 1.0f;
    mat.specular.r = rSpecR;
    mat.specular.g = rSpecG;
    mat.specular.b = rSpecB;
    mat.power      = rPower;
    mat.hTexture   = NULL;
    // Don't need to set mat.dwRampSize since Dreamcast is RGB-only.

    // Set the material's data
    pmat->SetMaterial(&mat);

    return pmat;
}

HRESULT SetProjectionMatrix(D3DMATRIX &mat, FLOAT fFOV, FLOAT fAspect,
                                     FLOAT fNearPlane, FLOAT fFarPlane )
{
	float cs,sn;

#if defined(UNDER_CE)
	_SinCosA(&sn, &cs, fFOV/2);
#else
	sn = (float)sin(fFOV/2);
	cs = (float)cos(fFOV/2);
#endif

	FLOAT w = fAspect * cs / sn;
	FLOAT h =	1.0f  * cs / sn;
	FLOAT Q = fFarPlane / ( fFarPlane - fNearPlane );
	
	memset( &mat, 0, sizeof(D3DMATRIX) );
	mat._11 = w;
	mat._22 = h;
	mat._33 = Q;
	mat._34 = 1.0f;
	mat._43 = -Q*fNearPlane;

	return S_OK;
}

#if !defined(UNDER_CE)
//-----------------------------------------------------------------------------
// Name: EnumZBufferCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
										   VOID* pddpfDesired )
{
	if( pddpf->dwFlags == DDPF_ZBUFFER )
	{
		memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

		// Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
	}

	// Return with D3DENUMRET_OK to continue the search.
	return D3DENUMRET_OK;
}
#endif

HRESULT Initialize3DEnvironment( HWND hWnd)
{
	HRESULT hr;
#if !defined(UNDER_CE)
	DDSURFACEDESC2 ddsd;

	//-------------------------------------------------------------------------
	// Create the z-buffer AFTER creating the backbuffer and BEFORE creating
	// the d3ddevice.
	//
	// Note: before creating the z-buffer, apps may want to check the device
	// caps for the D3DPRASTERCAPS_ZBUFFERLESSHSR flag. This flag is true for
	// certain hardware that can do HSR (hidden-surface-removal) without a
	// z-buffer. For those devices, there is no need to create a z-buffer.
	//-------------------------------------------------------------------------

	DDPIXELFORMAT ddpfZBuffer;
	g_pD3D->EnumZBufferFormats( IID_IDirect3DHALDevice, EnumZBufferCallback, (VOID*)&ddpfZBuffer );

	// If we found a good zbuffer format, then the dwSize field will be
	// properly set during enumeration. Else, we have a problem and will exit.
	if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
		return E_FAIL;

	// Get z-buffer dimensions from the render target
	// Setup the surface desc for the z-buffer.
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize 				= sizeof(DDSURFACEDESC2);
	ddsd.dwFlags		= DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
	ddsd.dwWidth		= g_rcScreenRect.right - g_rcScreenRect.left;
	ddsd.dwHeight		= g_rcScreenRect.bottom - g_rcScreenRect.top;
	memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

	// Create and attach a z-buffer. Real apps should be able to handle an
	// error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this 
	// tutorial, though, we are simply going to exit ungracefully.
	if( FAILED( hr = g_pDD4->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
		return hr;

	// Attach the z-buffer to the back buffer.
	if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
		return hr;
#endif
	hr = g_pD3D->CreateDevice(IID_IDirect3DHALDevice, g_pddsBackBuffer, &g_pd3dDevice, NULL);

    D3DVIEWPORT2 viewData;   // Viewport Data Structure to fill

    // Fill the D3DVIEWPORT data structure
    memset(&viewData, 0, sizeof(D3DVIEWPORT2));
    viewData.dwSize   = sizeof(D3DVIEWPORT2);
    viewData.dwWidth  = 640;
    viewData.dwHeight = 480;

    viewData.dvClipWidth  = 2.0f;
    viewData.dvClipHeight = 2.0f;

    viewData.dvClipX = -1.0f;
    viewData.dvClipY = 1.0f;

    viewData.dvMaxZ = 1.0f;

	// Create the viewport
	hr = g_pD3D->CreateViewport( &g_pvViewport, NULL );

	// Associate the viewport with the D3DDEVICE object
	hr = g_pd3dDevice->AddViewport( g_pvViewport );

	// Set the parameters to the new viewport
	hr = g_pvViewport->SetViewport2( &viewData );

	// Set the viewport as current for the device
	hr = g_pd3dDevice->SetCurrentViewport( g_pvViewport );

    // Create the global materials
    g_pmatWhite = CreateMaterial(1.0f, 1.0f, 1.0f, 1.0f, 10.0f, 0.25f, 0.25f, 0.25f, 0.75f, 0.75f, 0.75f);
    g_pmatBlack = CreateMaterial(0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    
    // Get a handle to the materials for later use
    g_pmatWhite->GetHandle(g_pd3dDevice, &g_hmatWhite);

    g_pmatBlack->GetHandle(g_pd3dDevice, &g_hmatBlack);

    // Set the viewport's background material (This is necessary for the Viewport->Clear that occurs elsewhere).
    hr = g_pvViewport->SetBackground(g_hmatBlack);

	EnumTextureFormats();

	hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF, (DWORD)0x01 );
	hr = g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); 
	hr = g_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
	hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	hr = g_pd3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE );
	hr = g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	hr = g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	hr = g_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
	hr = g_pd3dDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(0xc0, 0xc0, 0xc0, 255));
	hr = g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	hr = g_pd3dDevice->SetLightState (D3DLIGHTSTATE_MATERIAL,           g_hmatWhite);
    hr = g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,    FALSE);
	
	D3DMATRIX mat;
	mat._11=1.0;  mat._12=0.0; mat._13=0.0; mat._14=0.0;
	mat._21=0.0;  mat._22=1.0; mat._23=0.0; mat._24=0.0;
	mat._31=0.0;  mat._32=0.0; mat._33=1.0; mat._34=0.0;
	mat._41=0.0;  mat._42=0.0; mat._43=0.0; mat._44=1.0;

    g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,      &mat );
    g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &mat );

	SetProjectionMatrix(ProjectionMatrix, PI/5.5f, 3.0f/4.0f, 1.0f, 30000.0f );
//	SetProjectionMatrix(ProjectionMatrix, PI/5.5f, 9.0f/24.0f, 1.0f, 30000.0f );

    g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &ProjectionMatrix );
	return S_OK;
}

float SetZBack(float zback)
{
	static float lastZback=0.0f;
	float retval;

	SetProjectionMatrix(ProjectionMatrix, PI/5.5f, 3.0f/4.0f, 1.0f, zback );
//	SetProjectionMatrix(ProjectionMatrix, PI/5.5f, 9.0f/24.0f, 1.0f, zback );
    g_pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &ProjectionMatrix );
	retval = lastZback;
	lastZback = zback;
	return retval;
}

__declspec(dllexport) BOOL AddLight(float rRed, float rGreen, float rBlue, float rX, float rY, float rZ)
{
    D3DLIGHT2 light;
	HRESULT hr;

    memset(&light, 0, sizeof(D3DLIGHT2));
    light.dwSize        = sizeof(D3DLIGHT2);

    light.dltType       = D3DLIGHT_DIRECTIONAL;
    light.dcvColor.r    = rRed;
    light.dcvColor.g    = rGreen;
    light.dcvColor.b    = rBlue;
    light.dcvColor.a    = 1.0f;
    light.dvDirection.x = rX;
    light.dvDirection.y = rY;
    light.dvDirection.z = rZ;
	light.dwFlags = D3DLIGHT_ACTIVE;

    // Create the global light
    hr = g_pD3D->CreateLight(&g_rglight, NULL);
    
    // Set the global light's information
    hr = g_rglight->SetLight((D3DLIGHT *)&light);

    // Add the global light to the global viewport
    hr = g_pvViewport->AddLight(g_rglight);
 
    return TRUE;
}

bool ReleaseLight(void)
{
	HRESULT hr;

    hr = g_pvViewport->DeleteLight(g_rglight);
	hr = g_rglight->Release();
	return true;
}

void SetLightDirection(float x,float z)
{
	D3DLIGHT2 light;

	g_rglight->GetLight((D3DLIGHT *)&light);

	light.dvDirection.dvX = x;
	light.dvDirection.dvY = -0.5f;
	light.dvDirection.dvZ = z;
    
	g_rglight->SetLight((D3DLIGHT *)&light);
}

void SetLightColour(float r,float g,float b)
{
	D3DLIGHT2 light;

	g_rglight->GetLight((D3DLIGHT *)&light);

    light.dcvColor.r    = r;
    light.dcvColor.g    = g;
    light.dcvColor.b    = b;
    
	g_rglight->SetLight((D3DLIGHT *)&light);
}


#if defined(UNDER_CE)
__declspec(dllexport) void renderInitialise(HWND hwnd, bool Only3D)
#else
void renderInitialise(HWND hwnd, bool b_fullScreen)
#endif
{
#if !defined(UNDER_CE)
	g_bAppUseFullScreen = b_fullScreen;
#endif

#if defined(UNDER_CE)
	if(!Only3D)
#endif
	{
		Initialize2DEnvironment(hwnd);
		InitializeSurfaces(hwnd);
	}
	Initialize3DEnvironment(hwnd);
}

__declspec(dllexport) DWORD isTextureAlpha(DWORD handle)
{
	BorisTexture2 *bt = &textureHandles[handle - 0x1000];

	switch(bt->format)
	{
		case TEXFMT_ARGB_1555:
		case TEXFMT_VQ1:
			return 1;
		case TEXFMT_ARGB_4444:
		case TEXFMT_VQ4:
			return 4;
		default:
			return 0;
	}
}

LPDIRECTDRAWSURFACE4 getSurface(DWORD handle)
{
	BorisTexture2 *bt = &textureHandles[handle - 0x1000];
	return bt->lpSurface;
}

__declspec(dllexport) DWORD registerTexture(char *pointerToFile, int TextureStage,int *w,int *h)
{
	Mip mip;
	int myHandle=0;

	bool VQ = false;

	if(!strncmp(pointerToFile,"GBIX",4))
		pointerToFile += 16;
	if(!strncmp(pointerToFile,"PVRT",4))
		VQ = true;
	else

		MipReadFromMem(&mip,pointerToFile,w,h);
	for(myHandle=0;myHandle<TEXTURES_AVAILABLE;++myHandle)
		if(textureHandles[myHandle].lpSurface == NULL)
			break;
	if(myHandle==TEXTURES_AVAILABLE)
		exit(0);

	if(VQ)
	{
		RealizeVQ(&textureHandles[myHandle],pointerToFile);
	}
	else

	{
		RealizeImg(&textureHandles[myHandle],&mip,TextureStage);
		free(mip.data[0]);
	}
	return myHandle+0x1000;
}

__declspec(dllexport) void unregisterTexture(DWORD handle)
{
	HRESULT hr;
	int offset = handle - 0x1000;
	if(g_pd3dDevice)
		g_pd3dDevice->SetTexture (0,NULL);
	CurrentTexture[0] = NULL;
	if(textureHandles[offset].lpSurface)
	{
		hr=textureHandles[offset].lpTexture->Release();
		textureHandles[offset].lpTexture=NULL;
		hr=textureHandles[offset].lpSurface->Release();
		textureHandles[offset].lpSurface=NULL;
	}
}

__declspec(dllexport) void renderBeginScene(bool clear,DWORD rgb)
{
	DWORD flags;
	HRESULT hr;

	if(clear)
		flags = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER;
	else
		flags = D3DCLEAR_ZBUFFER;
	hr = g_pvViewport->Clear2( 1UL,(D3DRECT *)&g_rcViewportRect, flags ,rgb,1.0f,0);
	hr = g_pd3dDevice->BeginScene();
}

void SetAlphaTest(bool flag)
{
	if(CurrentAlphaTest != flag)
	{
		g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, flag );
		CurrentAlphaTest = flag;
	}
}

BOOL SetAlphaBlend(bool flag)
{
	BOOL tf = CurrentAlphaBlend;
	if(CurrentAlphaBlend != flag)
	{
		g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, flag );
		CurrentAlphaBlend = flag;
	}
	return tf;
}

__declspec(dllexport) void renderEndScene(void)
{
	HRESULT hr;

	hr = g_pd3dDevice->EndScene();
}

void SetBlend(bool flag)
{
	if(CurrentBlend != flag)
	{
		if(flag)
		{
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_POINT);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		}
		else
		{
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
		}
		CurrentBlend = flag;
	}
}

void SetTexture(int stage, LPDIRECT3DTEXTURE2 tex)
{
	if(CurrentTexture[stage] != tex)
	{
		g_pd3dDevice->SetTexture (stage,tex);
		CurrentTexture[stage] = tex;
	}
}

void SetTClamp(int stage, bool flag)
{
	if(CurrentTClamp[stage] != flag)
	{
		g_pd3dDevice->SetTextureStageState(stage, D3DTSS_ADDRESSV,flag ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
		CurrentTClamp[stage] = flag;
	}
}

void SetSClamp(int stage, bool flag)
{
	if(CurrentSClamp[stage] != flag)
	{
		g_pd3dDevice->SetTextureStageState(stage, D3DTSS_ADDRESSU,flag ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP);
		CurrentSClamp[stage] = flag;
	}
}

__declspec(dllexport) void renderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2, MAP_TYPE maptype,BorisMaterial *m3)
{
	static bool OneTextureLastTime = true;
	static bool VoodooTexture = false;

	if(m1 && m1->textureHandle)
	{
		BorisTexture2 *bt = &textureHandles[m1->textureHandle - 0x1000];
		
		SetBlend(m1->flags & MAT_NOBLEND ? true:false);
		if(bt->format == TEXFMT_ARGB_1555 || bt->format == TEXFMT_VQ1)
		{
			// Set Punch thru on Dreamcast
			SetAlphaTest(true);
		}
		else
		{
			SetAlphaTest(false);
			if(bt->format == TEXFMT_ARGB_4444 || bt->format == TEXFMT_VQ4)
				SetAlphaBlend(true);
			else
				SetAlphaBlend(false);
		}
		SetSClamp(0,m1->flags & MAT_SCLAMP ? true : false);
		SetTClamp(0,m1->flags & MAT_TCLAMP ? true : false);
		SetTexture (0,bt->lpTexture);
	}
	else
	{
		SetTexture (0,NULL);
		SetAlphaTest(false);
		SetAlphaBlend(true);
	}
}

void renderFlipSurface(bool halfRate)
{
#if !defined(UNDER_CE)
	if(!g_bAppUseFullScreen)
		g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer, 
							   &g_rcViewportRect, DDBLT_WAIT, NULL );
	else
#endif
		g_pddsPrimary->Flip( NULL,	halfRate ? DDFLIP_WAIT | DDFLIP_INTERVAL2 : DDFLIP_WAIT);
}

void renderList(D3DTLVERTEX* list,int numVert,BOOL clip)
{
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX , list, numVert, clip?D3DDP_DONOTUPDATEEXTENTS:D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
}

void renderLine(D3DTLVERTEX* list,int numVert)
{
	g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, D3DFVF_TLVERTEX , list, numVert, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
}

void renderList(D3DTLVERTEX2* list,int numVert,BOOL clip)
{
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX2 , list, numVert, clip?D3DDP_DONOTUPDATEEXTENTS:D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
}

HRESULT Cleanup3DEnvironment()
{
	// Release the DDraw and D3D objects used by the app
	if( g_pvViewport )	   
		g_pvViewport->Release();

	// Do a safe check for releasing the D3DDEVICE. RefCount should be zero.
	if( g_pd3dDevice )
		if( 0 < g_pd3dDevice->Release() )
			return E_FAIL;
	g_pd3dDevice = NULL;
#if !defined(UNDER_CE)
	if( g_pddsZBuffer )
	{
		g_pddsBackBuffer->DeleteAttachedSurface(0, g_pddsZBuffer );
		g_pddsZBuffer->Release();
	}
#endif
	g_pvViewport	 = NULL;
#if !defined(UNDER_CE)
	g_pddsZBuffer	 = NULL;
#endif
	g_pd3dDevice	 = NULL;
	return S_OK;
}

HRESULT CleanupSurfaces()
{
#if !defined(UNDER_CE)
	if(!g_bAppUseFullScreen)
		if( g_pddsBackBuffer ) 
			g_pddsBackBuffer->Release();
#endif
	if( g_pddsPrimary )    
		g_pddsPrimary->Release();

	g_pddsBackBuffer = NULL;
	g_pddsPrimary	 = NULL;
	return S_OK;
}

HRESULT Cleanup2DEnvironment()
{
	if( g_pD3D )
		g_pD3D->Release();
	if( g_pDD4 )
	{
#if !defined(UNDER_CE)
		g_pDD4->SetCooperativeLevel( g_hWnd, DDSCL_NORMAL );
#endif
		g_pDD4->Release();
	}

 
	g_pD3D			 = NULL;
	g_pDD4			 = NULL;
	return S_OK;
}

void renderDeInitialise(CLOSE_TYPE flag)
{
	if(flag & CLOSE_3D)
		Cleanup3DEnvironment();
	if(flag & CLOSE_2D)
	{
		CleanupSurfaces();
		Cleanup2DEnvironment();
	}
}

DWORD renderGetCaps(void)
{
#if defined(UNDER_CE)
	return 0;
#else
	return Caps;
#endif
}

void SetLightState(int r, int g, int b)
{
	g_pd3dDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(r, g, b, 255));
}

void DarkenScreen(int sx,int sy, int width, int height, D3DCOLOR val,float zval)
{
	darkvert[0].sx = sx;
	darkvert[0].sy = sy;
	darkvert[0].rhw = 1.0f/zval;
	darkvert[0].color = val;
	darkvert[1].sx = sx+width;
	darkvert[1].sy = sy;
	darkvert[1].rhw = 1.0f/zval;
	darkvert[1].color = val;
	darkvert[2].sx = sx+width;
	darkvert[2].sy = sy+height;
	darkvert[2].rhw = 1.0f/zval;
	darkvert[2].color = val;
	darkvert[3].sx = sx;
	darkvert[3].sy = sy+height;
	darkvert[3].rhw = 1.0f/zval;
	darkvert[3].color = val;

	renderSetCurrentMaterial(NULL,NULL);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEXDARK , darkvert, 4, D3DDP_DONOTCLIP | D3DDP_DONOTUPDATEEXTENTS);
}

void recode(char *from, LPDIRECTDRAWSURFACE4 theSurface, int width, int height)
{
	DDSURFACEDESC2   Ddsd;
	BYTE *ptr;
	USHORT w,*w2,*f;
	HRESULT	ddrval;
	int RealBackPitch,i,j;

	memset(&Ddsd,0,sizeof(Ddsd));
	Ddsd.dwSize = sizeof(Ddsd);

	ddrval = theSurface->Lock(NULL, &Ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

	RealBackPitch = Ddsd.lPitch;
	ptr = (LPBYTE) Ddsd.lpSurface;

	for(i=0;i<height;++i,ptr+=RealBackPitch,from+=width*2)
		memcpy(ptr,from,width*2);
	theSurface->Unlock( NULL);
}

LPDIRECTDRAWSURFACE4 LoadSSB(char *pointer)
{
	ULONG filesize;
	LPDIRECTDRAWSURFACE4 lpDDS;
	DDSURFACEDESC2 ddSurf;
	HRESULT ddrval;

	pointer += 4;

	ddSurf.dwSize = sizeof(ddSurf);
	g_pddsPrimary->GetSurfaceDesc(&ddSurf);
	ddSurf.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	ddSurf.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	ddSurf.dwHeight = 480;
	ddSurf.dwWidth = 640;

	ddrval = g_pDD4->CreateSurface (&ddSurf, &lpDDS, NULL);
	
	recode(pointer, lpDDS, 640, 480);
	
	return lpDDS;
}

void SetDisplayHz(bool Hz50)
{
	CleanupSurfaces();
	mainHz = Hz50?25:30;
	InitializeSurfaces(g_hWnd);

}
