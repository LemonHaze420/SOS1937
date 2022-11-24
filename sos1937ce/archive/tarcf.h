// $Header$

// $Log$
// Revision 1.0  1999-12-07 11:48:48+00  jjs
// Initial revision
//

#define	MAKEFORMAT(a,b,c,d)	((a<<24) | (b<<16) | (c<<8) | d)

// master format types

// 3df images

#define	FF1_3DF		1

#define	FF2_565		1
#define	FF2_1555	2
#define	FF2_8BIT	4
#define	FF2_4444	8
#define	FF2_VQ		16

// models

#define	FF1_VO		2

#define	FF2_OBJ		1
#define	FF2_WORLD	2


// text blocks

#define	FF1_TXT		3

#define	FF2_MODDESC	1
#define	FF2_SCENERY	2
#define	FF2_PARAM	4

// bitmaps

#define	FF1_BMP		4

#define	FF2_24		1
#define	FF2_8		2
#define	FF2_16		4

#define	FF3_ALPHA	1

// assorted data

#define	FF1_MISC	5

#define	FF2_RLINE	1
#define	FF2_SPHERESET	2
#define	FF2_WAV		3

// Spirit of Speed Bitmap (i.e. front end BMP)

#define	FF1_SSB		6
// same subformats as FF1_BMP



/****Fri Dec 12 16:28:38 1997*************(C)*Broadsword*Interactive****
 *
 * Put the actual formats down here, and also in formats.c
 *
 ***********************************************************************/
 

#define	FMT_3DF_565			MAKEFORMAT(FF1_3DF,FF2_565,0,0)
#define	FMT_3DF_1555		MAKEFORMAT(FF1_3DF,FF2_1555,0,0)
#define	FMT_3DF_8BIT		MAKEFORMAT(FF1_3DF,FF2_8BIT,0,0)
#define	FMT_3DF_4444		MAKEFORMAT(FF1_3DF,FF2_4444,0,0)
#define	FMT_3DF_VQ			MAKEFORMAT(FF1_3DF,FF2_VQ,0,0)

#define	FMT_VO_OBJ			MAKEFORMAT(FF1_VO,FF2_OBJ,0,0)
#define	FMT_VO_WORLD		MAKEFORMAT(FF1_VO,FF2_WORLD,0,0)

#define	FMT_TXT_MODDESC		MAKEFORMAT(FF1_TXT,FF2_MODDESC,0,0)
#define	FMT_TXT_SCENERY		MAKEFORMAT(FF1_TXT,FF2_SCENERY,0,0)
#define	FMT_TXT_PARAM		MAKEFORMAT(FF1_TXT,FF2_PARAM,0,0)

#define	FMT_BMP_24			MAKEFORMAT(FF1_BMP,FF2_24,0,0)
#define	FMT_BMP_8			MAKEFORMAT(FF1_BMP,FF2_8,0,0)
#define	FMT_BMP_8A			MAKEFORMAT(FF1_BMP,FF2_8,FF3_ALPHA,0)
#define	FMT_BMP_16			MAKEFORMAT(FF1_BMP,FF2_16,0,0)

#define	FMT_SSB_24			MAKEFORMAT(FF1_SSB,FF2_24,0,0)
#define	FMT_SSB_8			MAKEFORMAT(FF1_SSB,FF2_8,0,0)
#define	FMT_SSB_16			MAKEFORMAT(FF1_SSB,FF2_16,0,0)

#define	FMT_RLINE		MAKEFORMAT(FF1_MISC,FF2_RLINE,0,0)
#define	FMT_SPHERESET	MAKEFORMAT(FF1_MISC,FF2_SPHERESET,0,0)
#define	FMT_WAV			MAKEFORMAT(FF1_MISC,FF2_WAV,0,0)

#define	FMT_UNKNOWN			MAKEFORMAT(0xff,0xff,0xff,0xff)

