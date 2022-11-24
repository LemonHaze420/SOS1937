// $Header$

// $Log$
// Revision 1.3  2000-03-06 12:52:36+00  jjs
// Adds support for VQ textures.
//
// Revision 1.2  2000-02-07 12:20:59+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.1  2000-01-13 17:29:21+00  jjs
// First version that supports the loading of objects.
//

#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include "MipRead.h"
#include <stdio.h>

char *Format_Name[] = {
         "rgb332",                              // TEXFMT_RGB_332
         "yiq",                                 // TEXFMT_YIQ_422
         "a8",                                  // TEXFMT_ALPHA_8
         "i8",                                  // TEXFMT_INTENSITY_8
         "ai44",                                // TEXFMT_ALPHA_INTENSITY_44
         "p8",                                  // TEXFMT_P_8
         "rsvd1",                               // TEXFMT_RSVD1
         "rsvd2",                               // TEXFMT_RSVD2
         "argb8332",                            // TEXFMT_ARGB_8332
         "ayiq8422",                            // TEXFMT_AYIQ_8422
         "rgb565",                              // TEXFMT_RGB_565
         "argb1555",                            // TEXFMT_ARGB_1555
         "argb4444",                            // TEXFMT_ARGB_4444
         "ai88",                                // TEXFMT_ALPHA_INTENSITY_88
         "ap88",                                // TEXFMT_AP_88
         "rsvd4",                               // TEXFMT_RSVD4
         "argb8888",                            // TEXFMT_ARGB_8888
};


/* Read 3df header */
BOOL Read3DFHeaderMem(char ** mptr, DWORD cookie, Mip *Mip)
{
    int         i;
    char        version[6];
    char        color_format[10];
    int         lod_min, lod_max;
    int         aspect_width, aspect_height;
    int         w, h;
	char *ptr=*mptr;
	
    /* magic cookie starts with 3df v%6...., of which we've eaten 3d */
    // printf("3df file header\n");
	memcpy(version,ptr+2,3);
	while (*ptr++ != '\n');
	
    /* 
	* skip comments
	*/
    while (*ptr++ == '#') 
	{
        while (*ptr++ != '\n');
    }
	ptr-=1;

	char *NewPtr = ptr;

	/* color format, lod range, aspect ratio */
/*    if (5 != sscanf (ptr, "%s\nlod range: %i %i\naspect ratio: %i %i\n",
        color_format, &lod_min, &lod_max, &aspect_width, &aspect_height))
		return FALSE;*/
	int Length;

	strncpy(color_format, NewPtr, Length = (strchr(NewPtr, '\n') - NewPtr));
	color_format[Length] = 0;
//	sscanf(NewPtr, "%s", color_format);
	NewPtr = strchr(ptr, '\n');
	NewPtr += strlen("lod range: ")+1;
//	sscanf(NewPtr, "%i %i", &lod_min, &lod_max);
	lod_min = atoi(NewPtr);
	NewPtr = strchr(NewPtr, ' ')+1;
	lod_max = atoi(NewPtr);
	NewPtr = strchr(NewPtr, '\n');
	NewPtr += strlen("aspect ratio: ")+1;
//	sscanf(NewPtr, "%i %i", &aspect_width, &aspect_height);
	aspect_width = atoi(NewPtr);
	NewPtr = strchr(NewPtr, ' ')+1;
	aspect_height = atoi(NewPtr);

    // printf("%s %d %d (lods) %d %d (aspect)\n", color_format, lod_min, lod_max, aspect_width, aspect_height);

    /* eat final nl */
	while (*ptr++ != '\n');
	while (*ptr++ != '\n');
	while (*ptr++ != '\n');
	
    /* make sure null terminated */
    color_format[9] = 0;
	
    /* lookup name */
    for (i = 0; i <= TEXFMT_ARGB_8888; i++)
        if (strcmp (Format_Name[i], color_format) == 0) 
			break;
	if (i > TEXFMT_ARGB_8888)
		return FALSE;
	Mip->format = i;
		
	/* Validate lod */
	if ((lod_max & (lod_max-1)) || (lod_min & (lod_min-1))) 
		return FALSE;
	if ((lod_max > 256) || (lod_max < 1)) 
		return FALSE;
	if ((lod_min > 256) || (lod_min < 1)) 
		return FALSE;
	if (lod_min > lod_max) 
		return FALSE;
		
	/* validate aspect ratio */
	w = h = lod_max;
	switch ((aspect_width << 4) | (aspect_height)) 
	{
        case    0x81:
			h = h/8; 
			break;
        case    0x41:   
			h = h/4; 
			break;
        case    0x21:   
			h = h/2; 
			break;
        case    0x11:   
			h = h/1; 
			break;
        case    0x12:   
			w = w/2; 
			break;
        case    0x14:   
			w = w/4; 
			break;
        case    0x18:   
			w = w/8; 
			break;
        default:                
			return FALSE;
	}
		
	Mip->width  = w;
	Mip->height = h;
	Mip->size   = w * h;
		
	for (Mip->depth=1; lod_max > lod_min; lod_max >>= 1) 
	{
			// printf("w = %d, h = %d, lod_max = %d\n", w, h, lod_max);
		Mip->depth++;
		if (w > 1) 
			w >>= 1;
		if (h > 1) 
			h >>= 1;
		Mip->size += (w * h);
	}
	Mip->size *= TEXFMT_SIZE(Mip->format);
		
	*mptr=ptr;
	// printf("3df file: %dx%dx%d:%d\n", Mip->width, Mip->height, Mip->depth, Mip->size);
	return TRUE;
}

BOOL ReadHeaderMem( char **mptr, Mip *info )
{
	DWORD		cookie;
	int 		c;
	char *ptr=*mptr;
	BOOL 			fformat=FALSE;				// format of the image file.
	BOOL 			status=FALSE;

	c=*ptr++;
	cookie = (c << 8);
	c=*ptr++;
	cookie |= c;

	switch( cookie )
	{
		case ('3' << 8) | 'd':
			fformat = TRUE; 
			break;
		case ('3' << 8) | 'D':	
			fformat = TRUE; 
			break;
	}

	if(fformat) 
			status = Read3DFHeaderMem( &ptr, cookie, info ); 

	*mptr=ptr;
	return status;
}

/*************************************** 3df files ****************************/
/* Read word, msb first */
static BOOL Read16Mem (char **mptr, WORD* data)
{
    BYTE byte[2];
	char *ptr=*mptr;
    memcpy(byte,ptr,2);
    ptr+=2;
    *data = (((WORD) byte[0]) << 8) | ((WORD) byte[1]);
	*mptr=ptr;
    return TRUE;
}

/* Read long word, msb first */
static BOOL Read32Mem (char **mptr, DWORD* data)
{
    BYTE byte[4];

    char *ptr=*mptr;
    memcpy(byte,ptr,4);
    ptr+=4;

    *data = (((DWORD) byte[0]) << 24) |
            (((DWORD) byte[1]) << 16) |
            (((DWORD) byte[2]) <<  8) |
             ((DWORD) byte[3]);

    *mptr=ptr;
    return TRUE;
}

BOOL ReadDataMem( char **mptr, Mip *mip)
{
	int i, npixels;
	char *ptr=*mptr;
	
	/* read mipmap image data */
	if (mip->format < TEXFMT_16BIT) 
	{
		npixels = mip->size;
		
		memcpy (mip->data[0], ptr, npixels);
		ptr += npixels;
	}
	else 
	if (mip->format < TEXFMT_32BIT) 
	{
		WORD* data = (WORD *) mip->data[0];
		npixels = mip->size >> 1;
			
		for (i = 0; i < npixels; i++, data++)
			if (FALSE == Read16Mem (&ptr, data)) 
				return FALSE;
	}
	else 		
	{
		DWORD* data = (DWORD*) mip->data[0];
		npixels = mip->size >> 2;
			
		for (i = 0; i < npixels; i ++, data++)
			if (FALSE == Read32Mem (&ptr, data)) 
				return FALSE;
	}
	*mptr=ptr;
	return TRUE;
}

BOOL MipReadFromMem(Mip *mip,const char *mptr,int *ww, int *hh)
{
	int	i;
	int w, h;
	char *ptr=(char *)mptr;
	
	if(!ReadHeaderMem( &ptr, mip ))
	{
		fprintf(stderr,"Error: reading info \n");
		exit(2);
	}
	
	/*
	* Allocate memory requested in data[0]; 
	*/
	
	w = mip->width;
	h = mip->height;
	if(ww)
		*ww = w;
	if(hh)
		*hh = h;
	mip->data[0] = malloc(mip->size);
	for (i=1; i< MAX_LEVEL; i++) 
	{
		if (i >= mip->depth) 
		{
			mip->data[i] = NULL;
			continue;
		}
		mip->data[i] = (BYTE *)mip->data[i-1] +	w * h * TEXFMT_SIZE(mip->format);
		if (w > 1) 
			w >>= 1;
		if (h > 1) 
			h >>= 1;
	}
	
	if ( ReadDataMem( &ptr, mip ) == FALSE ) 
	{
		fprintf(stderr, "\nError: reading data for Mmeory\n");
		exit(4);
	}
	
	return TRUE;
}

