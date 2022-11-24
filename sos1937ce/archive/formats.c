// $Header$

// $Log$
// Revision 1.0  1999-12-07 11:48:45+00  jjs
// Initial revision
//

#include "tarcf.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined(UNDER_CE)
#define isdigit(x) ((x)>='0' && (x)<='9')
#define isupper(x) ((x)>='A' && (x)<='Z')
#endif

static struct
{
	unsigned long key;
	char *name;
} tarc_formats[]=
{
	// texture formats
	
	{ FMT_3DF_565		,	"3df-565"},
	{ FMT_3DF_1555		,	"3df-1555"},
	{ FMT_3DF_8BIT		,	"3df-8bit"},
	{ FMT_3DF_4444		,	"3df-4444"},

	// model formats

	{ FMT_VO_OBJ		,	"vo-obj"},
	{ FMT_VO_WORLD		,	"vo-world"},

	// text file formats
	
	{ FMT_TXT_MODDESC	,	"txt-mod"},
	{ FMT_TXT_SCENERY	,	"txt-scen"},
	{ FMT_TXT_PARAM		,	"txt-param"},
	
	// bitmap formats

	{ FMT_BMP_24		,	"bmp-24"},
	{ FMT_BMP_8			,	"bmp-8"},
	{ FMT_BMP_8A		,	"bmp-8a"},
	{ FMT_BMP_16		,	"bmp-16"},
	{ FMT_SSB_24		,	"ssb-24"},
	{ FMT_SSB_8			,	"ssb-8"},
	{ FMT_SSB_16		,	"ssb-16"},

	// misc  formats

	{ FMT_RLINE		,	"rline"},
	{ FMT_SPHERESET	,	"sphereset"},
	{ FMT_WAV		,	"wav"},

	{ 0,0L }
};

char *TARC_GetFormatName(unsigned long id)
{
	int i;
	static char buf[16];
	for(i=0;;i++)
	{
		if(!tarc_formats[i].name)
		{
			sprintf(buf,"0%lx",id);
			return buf;
		}
		else if(tarc_formats[i].key == id)
			return tarc_formats[i].name;
	}
	return NULL;
}

unsigned long TARC_GetFormatByName(char *s)
{
	int i;

	if(!*s || *s=='*')return 0; // any format

	if(*s=='0')
	{
		unsigned long n=0,d=1;
		char *p=(s+strlen(s))-1;
		do
		{
			n += d*
				(isdigit(*p) ? (*p-'0') :
				(isupper(*p) ? (*p-'A') : (*p-'a')));
			d*=16;p--;
		} while(p>=s);
		return n;
	}

	for(i=0;;i++)
	{
		if(!tarc_formats[i].name)
			return FMT_UNKNOWN;	// has no format
		if(!strcmp(tarc_formats[i].name,s))
			return tarc_formats[i].key;
	}
	return FMT_UNKNOWN;
}

void listformats()
{
	int i;

	for(i=0;;i++)
	{
		if(!tarc_formats[i].name)break;

		printf("%40s     %lx\n",tarc_formats[i].name,tarc_formats[i].key);
	}
}
