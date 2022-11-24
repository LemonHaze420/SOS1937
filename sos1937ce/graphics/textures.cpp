// $Header$

// $Log$
// Revision 1.5  2000-03-20 11:35:26+00  jjs
// Fixes PVR textures.
//
// Revision 1.4  2000-03-08 11:06:35+00  jjs
// Added support for large twiddled textures.
//
// Revision 1.3  2000-02-29 14:22:03+00  jjs
// Latest version with STRICT typing.
//
// Revision 1.2  2000-02-07 12:21:01+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.1  2000-01-13 17:29:22+00  jjs
// First version that supports the loading of objects.
//

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ddraw.h>
#include <d3d.h>
#include "MipRead.h"

extern LPDIRECT3DDEVICE3	g_pd3dDevice;
extern LPDIRECTDRAW4		g_pDD4;

class TextureFormats {
public:
	DWORD flags;
	DDSURFACEDESC2 surface565;
	DDSURFACEDESC2 surface1555;
	DDSURFACEDESC2 surface4444;
	DDSURFACEDESC2 surface555;
};

TextureFormats availableTextures;

#define	PVR_ARGB_1555			(0x00)
#define	PVR_ARGB_565			(0x01)
#define	PVR_ARGB_4444			(0x02)
#define PVR_COLOR_MASK			(0xFF)

#define PVR_TWIDDLED			(0x0100)
#define PVR_TWIDDLED_MM			(0x0200)
#define PVR_VQ					(0x0300)
#define PVR_VQ_MM				(0x0400)
#define PVR_TWIDDLED_RECTANGLE	(0x0D00)
#define PVR_TYPE_MASK			(0xFF00)

#define VQ_RGBA_5551            ( PVR_VQ | PVR_ARGB_1555)
#define VQ_RGBA_5650            ( PVR_VQ | PVR_ARGB_565)
#define VQ_RGBA_4444            ( PVR_VQ | PVR_ARGB_4444)

static void DeterminePackingShift (unsigned int mask, unsigned int *truncate_shift,
			    unsigned int *up_shift)
{
	unsigned int size;

	/* Determine how much the channel needs to be shifted up */

	if (mask == 0)
	{
		*up_shift = 0;
		*truncate_shift = 32;
	}
	else
	{
		*up_shift = 0;
		while ((mask & 1) == 0)
		{
			(*up_shift)++;
			mask >>= 1;
		}

		/* Determine how many bits the channel is */

		size = 0;
		while (mask & 1)
		{
			size++;
			mask >>= 1;
		}

		/* Calculate how many bits need to be chopped off */

		*truncate_shift = 8 - size;
	}
}


static HRESULT CALLBACK EnumPixelFormatsCallback (DDPIXELFORMAT * pddpf, LPVOID lpContext)
{
	unsigned long m;
	int a, r, g, b;

	TextureFormats *tf = (TextureFormats *)lpContext;

	// Setup the new surface desc
	DDSURFACEDESC2 ddsd;

	memset( &ddsd,0 ,sizeof(DDSURFACEDESC2) );
	ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
	ddsd.ddpfPixelFormat = *pddpf;
	ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwTextureStage  = 0;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED4)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXEDTO8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_RGB)
	{
		for (r = 0, m = pddpf->dwRBitMask; !(m & 1);
		     r++, m >>= 1);
		for (r = 0; m & 1; r++, m >>= 1);
		for (g = 0, m = pddpf->dwGBitMask; !(m & 1);
		     g++, m >>= 1);
		for (g = 0; m & 1; g++, m >>= 1);
			for (b = 0, m = pddpf->dwBBitMask; !(m & 1);
		     b++, m >>= 1);
		for (b = 0; m & 1; b++, m >>= 1);
	}
	a=0;
	if(pddpf->dwFlags &  DDPF_ALPHAPIXELS)
	{
		for (a = 0, m = pddpf->dwRGBAlphaBitMask; !(m & 1);
		     a++, m >>= 1);
		for (a = 0; m & 1; a++, m >>= 1);
	}
	if(a == 1 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface1555, &ddsd, sizeof(ddsd));
		tf->flags |= f1555;
	}
	if(a == 4 && r == 4 && g == 4 && b == 4)
	{
		memcpy(&tf->surface4444, &ddsd, sizeof(ddsd));
		tf->flags |= f4444;
	}
	if(a == 0 && r == 5 && g == 6 && b == 5)
	{
		memcpy(&tf->surface565, &ddsd, sizeof(ddsd));
		tf->flags |= f565;
	}
	if(a == 0 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface555, &ddsd, sizeof(ddsd));
		tf->flags |= f555;
	}
	return DDENUMRET_OK;
}

/*
 * D3DAppIEnumTextureFormats
 * Get a list of available texture map formats from the Direct3D driver by
 * enumeration.  Choose a default format (paletted is prefered).
 */
BOOL EnumTextureFormats (void)
{
	HRESULT hr;

	hr =	g_pd3dDevice->EnumTextureFormats (EnumPixelFormatsCallback, (LPVOID) & availableTextures);
	if (hr != DD_OK)
		return FALSE;
	return TRUE;
}
/*This calculates the maximum number of mipmaps a texture can have
   given its dimensions */
static int CalculateMipMapLevels (int width, int height)
{
	int LOD = 0;

	while (width > 0 && height > 0)
	{
		width /= 2;
		height /= 2;
		LOD++;
	}

	return (LOD);
}

static int DetermineFormat (Mip * mip,LPDDSURFACEDESC2 * lplpFormat, int *srcBpp)
{
	int fmt;

	fmt = mip->format;

	if ((fmt == TEXFMT_ARGB_4444) && (!(availableTextures.flags & f4444)))
		fmt = TEXFMT_RGB_565;

	if ((fmt == TEXFMT_ARGB_1555) && (!(availableTextures.flags & f1555)))
		fmt = TEXFMT_RGB_565;

	switch (fmt)
	{
		case TEXFMT_RGB_565:
			*lplpFormat = &availableTextures.surface565;
			*srcBpp = 2;
			break;
		case TEXFMT_ARGB_1555:
			*lplpFormat = &availableTextures.surface1555;
			*srcBpp = 2;
			break;
		case TEXFMT_ARGB_4444:
			*lplpFormat = &availableTextures.surface4444;
			*srcBpp = 2;
			break;
	}

	return fmt;
}

BOOL PutImg (LPDIRECTDRAWSURFACE4 lpDDS, const Mip * mip, int LOD, int width, int height)
{
	unsigned char *dst, *src, *srcLine, *dstLine;
	size_t srcStride, dstStride;
	int srcBpp, dstBpp;
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	unsigned int r_up_shift, g_up_shift, b_up_shift, a_up_shift;
	unsigned int r_truncate_shift, g_truncate_shift, b_truncate_shift,
	  a_truncate_shift;
	unsigned int packed_pixel;
	unsigned int red, green, blue, alpha;
	WORD tmp16;
	int row, col;

	/* check for valid source format and determine pixel size */

	switch (mip->format)
	{
		case TEXFMT_RGB_565:
		case TEXFMT_ARGB_4444:
		case TEXFMT_ARGB_1555:
			srcBpp = 2;
			break;
	}

	srcStride = srcBpp * width;
	srcLine = (unsigned char *)mip->data[LOD];

	/* Build a descriptor to lock the surface */

	memset (&ddsd, 0, sizeof (DDSURFACEDESC2));
	ddsd.dwSize = sizeof (DDSURFACEDESC2);
	ddrval = lpDDS->Lock (NULL, &ddsd, 0, NULL);

	dstBpp = ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
	dstStride = ddsd.lPitch;

	dstLine = (unsigned char *) ddsd.lpSurface;

	/* Get packing shift information */

	DeterminePackingShift (ddsd.ddpfPixelFormat.dwRBitMask,
			       &r_truncate_shift, &r_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwGBitMask,
			       &g_truncate_shift, &g_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwBBitMask,
			       &b_truncate_shift, &b_up_shift);
	DeterminePackingShift (ddsd.ddpfPixelFormat.dwRGBAlphaBitMask,
			       &a_truncate_shift, &a_up_shift);

	/* Slow code is good, or something */

	for (row = 0; row < height; row++)
	{
		dst = dstLine;
		dstLine += dstStride;
		src = srcLine;
		srcLine += srcStride;
		for (col = 0; col < width; col++)
		{

			/* extract the pixel */

			switch (mip->format)
			{
				case TEXFMT_RGB_565:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					green = (tmp16 & 0x3f) << 2;
					tmp16 >>= 6;
					red = (tmp16 & 0x1f) << 3;
					tmp16 >>= 6;
					alpha = 0xff;
					break;
				case TEXFMT_ARGB_4444:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					green = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					red = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					alpha = (tmp16 & 0xf) << 4;
					tmp16 >>= 4;
					break;
				case TEXFMT_ARGB_1555:
					tmp16 = *(WORD *) src;
					blue = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					green = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					red = (tmp16 & 0x1f) << 3;
					tmp16 >>= 5;
					alpha = (tmp16 & 0xf) ? 0xff : 0x0;
					break;
			}

			/* Pack the pixel */

			packed_pixel = ((blue >> b_truncate_shift) << b_up_shift);
			packed_pixel |= ((green >> g_truncate_shift) << g_up_shift);
			packed_pixel |= ((red >> r_truncate_shift) << r_up_shift);

			/* TBD: compiler error!!, shifting by 32 is a NOOP */

			if (a_truncate_shift < 32)
				packed_pixel |= ((alpha >> a_truncate_shift) << a_up_shift);

			*((unsigned short *) dst) = (unsigned short) packed_pixel;
			dst += dstBpp;
			src += srcBpp;
		}
	}
	/* release the lock */
	lpDDS->Unlock (NULL);


	return TRUE;
}


BOOL RealizeImg (BorisTexture2 *tex, Mip* mip, int TextureStage)
{
	LPDDSURFACEDESC2 lpFormat;
	LPDIRECTDRAWSURFACE4 lpDDS;
	LPDIRECTDRAWSURFACE4 lpSurface = NULL;
	LPDIRECT3DTEXTURE2 lpTexture = NULL;
	LPDIRECTDRAWPALETTE lpPalette = NULL;
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	DDSCAPS2 ddscaps;
	int LOD, MAX_LOD;
	int target_width, target_height, target_format,width,height;
	int srcBpp;

	/* determine what the final width, height and format the
	   image should be that can be supported by D3D
	 */

	target_width = mip->width;
	target_height = mip->height;
	target_format = DetermineFormat (mip, &lpFormat, &srcBpp);

	MAX_LOD = CalculateMipMapLevels (target_width, target_height);

	/* honor the input images # of LOD's */
	MAX_LOD = min (MAX_LOD, mip->depth);

	/* Setup the descriptor to create the surface */

	memcpy (&ddsd, lpFormat, sizeof (DDSURFACEDESC2));
	ddsd.dwSize = sizeof (DDSURFACEDESC2);

//	if(target_height != target_width)
		MAX_LOD = 1;
	if (MAX_LOD != 1)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
			| DDSD_MIPMAPCOUNT;
		ddsd.dwMipMapCount = MAX_LOD;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_COMPLEX |
			DDSCAPS_MIPMAP | DDSCAPS_VIDEOMEMORY;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;
	}
	ddsd.ddsCaps.dwCaps2 = 0;
	ddsd.dwHeight = target_height;
	ddsd.dwWidth = target_width;

	ddrval = g_pDD4->CreateSurface (&ddsd, &lpDDS, NULL);
	if(ddrval != DD_OK)
		return FALSE;
	lpSurface = lpDDS;

	/* fill the MipMaps by looping through each Level of Detail */

	ddscaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

	width = mip->width;
	height= mip->height;

	for (LOD = 0; LOD < MAX_LOD; LOD++)
	{
		PutImg (lpDDS, mip, LOD, width, height);

		width >>= 1;
		height >>= 1;

		/* Get the next mipmap level */
		if(LOD != MAX_LOD-1)
		{
			ddrval = lpDDS->GetAttachedSurface (&ddscaps, &lpDDS);

			if (ddrval != DD_OK && lpDDS != NULL)
			{
				lpDDS->Release ();
				return FALSE;
			}
		}
	}

	ddrval = lpSurface->QueryInterface (IID_IDirect3DTexture2, (void **) &lpTexture);

	tex->lpSurface = lpSurface;
	tex->lpTexture = lpTexture;
	tex->format = target_format;

	return TRUE;
}

BOOL RealizeVQ (BorisTexture2 *tex, char* VQRowData)
{
	HRESULT					hres;
	LPDIRECTDRAWSURFACE4	lpDDS=NULL;
	DDSURFACEDESC2			ddsd;
	DDPIXELFORMAT			PixelFormat;
	WORD					x, y;
	DWORD					i = 0, dwMemoryUsedByTexture=0, dwDataSize, dwFormat;
	int tformat;
	bool twiddled = false;

    /* Get the size of the image data */
	dwDataSize = (DWORD)*(((LPDWORD)VQRowData) + 1);
    dwDataSize -= 8;

	/* Get the pixel format of the image */
    dwFormat = *(((LPDWORD)VQRowData) + 2);

    /* Get the dimensions of the image */
	x = (WORD)*(((LPWORD)VQRowData) + 6);
	y = (WORD)*(((LPWORD)VQRowData) + 7);
   
 
	switch(dwFormat & PVR_TYPE_MASK)
	{
		case PVR_TWIDDLED:
		case PVR_TWIDDLED_MM:
		case PVR_TWIDDLED_RECTANGLE:
			twiddled = true;
			break;
	}
	/* Initialise PixelFormat structure */
	memset(&PixelFormat, 0, sizeof(DDPIXELFORMAT));
	PixelFormat.dwSize	  = sizeof(DDPIXELFORMAT);
	if((dwFormat & PVR_TYPE_MASK) == PVR_VQ)
		PixelFormat.dwFlags	  = DDPF_RGB | DDPF_COMPRESSED;
	else
		PixelFormat.dwFlags	  = DDPF_RGB;

	switch (dwFormat & PVR_COLOR_MASK)
	{
	default:
	case PVR_ARGB_565:
        PixelFormat.dwRGBBitCount = 16;
        PixelFormat.dwRBitMask = 0x0000F800;
        PixelFormat.dwGBitMask = 0x000007E0;
        PixelFormat.dwBBitMask = 0x0000001F;
		tformat = TEXFMT_VQ;
		break;

	case PVR_ARGB_4444:
        PixelFormat.dwFlags |= DDPF_ALPHAPIXELS;
        PixelFormat.dwRGBBitCount = 16;
        PixelFormat.dwRBitMask = 0x00000F00;
        PixelFormat.dwGBitMask = 0x000000F0;
        PixelFormat.dwBBitMask = 0x0000000F;
        PixelFormat.dwRGBAlphaBitMask = 0x0000F000;
		tformat = TEXFMT_VQ4;
		break;

	case PVR_ARGB_1555:
        PixelFormat.dwFlags |= DDPF_ALPHAPIXELS;
        PixelFormat.dwRGBBitCount = 16;
        PixelFormat.dwRBitMask	= 0x00007C00;
		PixelFormat.dwGBitMask	= 0x000003E0;
		PixelFormat.dwBBitMask	= 0x0000001F;
		PixelFormat.dwRGBAlphaBitMask	= 0x00008000;
		tformat = TEXFMT_VQ1;
		break;
     }
 
	/* Create texture surface */
 	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize=sizeof(DDSURFACEDESC2);
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	if((dwFormat & PVR_TYPE_MASK) == PVR_VQ || !twiddled)
	    ddsd.ddsCaps.dwCaps=DDSCAPS_TEXTURE |  DDSCAPS_VIDEOMEMORY;
	else
	    ddsd.ddsCaps.dwCaps=DDSCAPS_TEXTURE |  DDSCAPS_VIDEOMEMORY | DDSCAPS_OPTIMIZED;
    ddsd.dwHeight=y;
    ddsd.dwWidth=x;
	ddsd.ddpfPixelFormat=PixelFormat;

	hres=g_pDD4->CreateSurface(&ddsd, &lpDDS, NULL);
	if (hres!=DD_OK) 
	{
		return 0;
	}

    /* Initialise surface */
	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
    ddsd.dwSize=sizeof(DDSURFACEDESC2);
	
	/* Lock texture surface to fill it */
#if defined(UNDER_CE)
	if((dwFormat & PVR_TYPE_MASK) == PVR_VQ)
	    hres=lpDDS->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_COMPRESSED, NULL); 
	else
		if(twiddled)
		    hres=lpDDS->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_OPTIMIZED, NULL); 
		else
#endif
			hres=lpDDS->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT , NULL); 

	if (hres!=DD_OK) 
	{
		return 0;
	}
	/* Copy the image data into the surface. */
	memcpy(ddsd.lpSurface, (char *)(((LPDWORD)VQRowData) + 4), dwDataSize);

    /* Unlock texture surface because we're done filling it */
	hres=lpDDS->Unlock(NULL);
  
	hres=lpDDS->QueryInterface(IID_IDirect3DTexture2,(LPVOID*)&	tex->lpTexture);
	
	tex->lpSurface = lpDDS;
	tex->format = tformat;

	/* Return OK */
    return TRUE;
}
