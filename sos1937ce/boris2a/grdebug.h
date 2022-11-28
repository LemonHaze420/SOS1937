/*
	$Header: $
	$Log: $
*/


/****Thu Oct 23 12:29:02 1997*************(C)*Broadsword*Interactive****
 *
 * 
 *
 ***********************************************************************/

#ifndef _BROAD_H_DEBUG_
#define _BROAD_H_DEBUG_

// graphical debugging buffer code

void grenable(bool f);

#if GRDEBUG

void grdebug(char *s,float v);
void grdebug(char *s,int v);
void grdebug(char *s,char *v);
void grdebug(char *s,bool v);

void grdebuginit(font *f);

void rendergrdebug();

#define	CarDebug	if(CarNo==ID)grdebug

#else

#define	grdebug(x,y)
#define	rendergrdebug()
#define	CarDebug(x,y)
#define	grdebuginit(x)

#endif

#endif
