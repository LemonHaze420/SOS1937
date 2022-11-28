/*
	$Header: $
	$Log: $
*/

static const char *rcs_id="$Id: $";

#include "BS2All.h"


#define	NUM_DEBUG_STRINGS	32

// graphical debugging code.

static char grbuffers[NUM_DEBUG_STRINGS][64];
static int grct=0;

bool grdebugging_on=false;

void grenable(bool f) { grdebugging_on = f; }

#if GRDEBUG

void grdebug(char *s,float v)
{
	if(grct<NUM_DEBUG_STRINGS)
		sprintf(grbuffers[grct++],"%16s : %4.2f",s,v);
}

void grdebug(char *s,int v)
{	
	if(grct<NUM_DEBUG_STRINGS)
		sprintf(grbuffers[grct++],"%16s : %7d",s,v);
}

void grdebug(char *s,char *v)
{
	if(grct<NUM_DEBUG_STRINGS)
		sprintf(grbuffers[grct++],"%16s : %7s",s,v);
}

void grdebug(char *s,bool v)
{
	if(grct<NUM_DEBUG_STRINGS)
		sprintf(grbuffers[grct++],"%16s : %7s",s,v?"true":"false");
}


static font *debugfont;

void rendergrdebug()
{
	if(grdebugging_on && debugfont)
	{
		int row=0,col=0;

		for(int i=0;i<grct;i++)
		{
			if(col==4) { col=0; row++;}
			debugfont->RenderString(160*col,150+row*(int)debugfont->lineheight,
				grbuffers[i]);
			col++;
		}
		grct=0;
	}
}

void grdebuginit(font *f)
{
	debugfont = f;
}

#endif

