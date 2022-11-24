// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:29:21+00  jjs
// First version that supports the loading of objects.
//

#define MAX_LEVEL		16
class  Mip {
public:
	int 	format; 			// TEXFMT_...
	int 	width;				// pixels
	int 	height; 			// pixels
	int 	depth;				// mipmap levels
	int 	size;				// bytes
	void	*data[MAX_LEVEL];
};

#define f565 1
#define f1555 2
#define f4444 4
#define f555 8

class BorisTexture2 {
public:
	LPDIRECTDRAWSURFACE4 lpSurface;
	LPDIRECT3DTEXTURE2 lpTexture;
	DWORD format;
};

typedef DWORD GrTextureFormat_t;
#define TEXFMT_8BIT                  0x0
#define TEXFMT_RGB_332               TEXFMT_8BIT
#define TEXFMT_YIQ_422               0x1
#define TEXFMT_ALPHA_8               0x2 /* (0..0xFF) alpha     */
#define TEXFMT_INTENSITY_8           0x3 /* (0..0xFF) intensity */
#define TEXFMT_ALPHA_INTENSITY_44    0x4
#define TEXFMT_P_8                   0x5
#define TEXFMT_VQ	                 0x6
#define TEXFMT_VQ1	                 0x7
#define TEXFMT_16BIT                 0x8
#define TEXFMT_ARGB_8332                     TEXFMT_16BIT
#define TEXFMT_AYIQ_8422             0x9
#define TEXFMT_RGB_565               0xa
#define TEXFMT_ARGB_1555             0xb
#define TEXFMT_ARGB_4444             0xc
#define TEXFMT_ALPHA_INTENSITY_88    0xd
#define TEXFMT_AP_88                 0xe
#define TEXFMT_VQ4                 0xf

#define TEXFMT_32BIT		0x10
#define TEXFMT_ARGB_8888	TEXFMT_32BIT

#define TEXFMT_SIZE(x)  ((x < TEXFMT_16BIT) ? 1:((x < TEXFMT_32BIT) ? 2: 4))

BOOL MipReadFromMem(Mip *mip,const char *mptr,int *w, int *h);
