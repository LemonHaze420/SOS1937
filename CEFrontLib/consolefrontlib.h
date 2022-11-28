// $Header$

// $Log$
// Revision 1.18  2000-04-19 21:37:57+01  img
// Localisation.
//
// Revision 1.17  2000-03-21 17:28:20+00  img
// Pre-loading more now...
//
// Revision 1.16  2000-03-20 18:00:43+00  img
// Beta build
//
// Revision 1.15  2000-03-13 18:14:00+00  img
// stricmp() and strnicmp() now functional.
//
// Revision 1.14  2000-03-10 16:09:07+00  img
// Boris3 compatibility
//
// Revision 1.13  2000-03-07 13:48:27+00  img
// Fix bug in strdup()
//
// Revision 1.12  2000-03-06 13:07:21+00  img
// SO* constants included
//
// Revision 1.11  2000-03-03 15:03:50+00  jjs
// Fixed for Boris3.
//
// Revision 1.10  2000-03-02 14:26:36+00  img
// Controller screen now working
//
// Revision 1.9  2000-03-01 11:31:27+00  img
// Properly working memory card screen again.
//
// Revision 1.8  2000-02-29 14:03:02+00  jjs
// Modified to work with Boris3.
//
// Revision 1.7  2000-02-29 08:44:40+00  jjs
// Added Header and Log lines
//

/* Header file for ConsoleFrontLib 

	Broadsword Interactive Ltd. 09/11/1999
	Author:			Ian Gledhill
	Rev. Date:		09/11/1999
	Rev. Number:	0.01

*/

#ifndef __CONSOLEFRONTLIB_H__
#define __CONSOLEFRONTLIB_H__

//#define BORISEMULATIONHACK

#define gWFSHack (1.0f/21000.0f)

#include <string.h>

#include <BS2All.h>

// WHY can't they include strdup()???
#ifdef UNDER_CE
__inline char * strdup(const char *Source)
{
	return strcpy((char *)malloc(strlen(Source)+1), Source);
}

__inline int strnicmp(const char *Source1, const char *Source2, int Length)
{
	int f, l;
	
	if(Length)
	{
		do {
			if ( ((f = (unsigned char)(*(Source1++))) >= 'A') &&
				(f <= 'Z') )
				f -= 'A' - 'a';
			
			if ( ((l = (unsigned char)(*(Source2++))) >= 'A') &&
				(l <= 'Z') )
				l -= 'A' - 'a';
			
		} while ( --Length && f && (f == l) );
		return ( f - l);
	}
	else
		return 0;
}

__inline int stricmp(const char *Source1, const char *Source2)
{
	int f,l;
	
	do {
		if ( ((f = (unsigned char)(*(Source1++))) >= 'A') && (f <= 'Z') )
			f -= ('A' - 'a');
		
		if ( ((l = (unsigned char)(*(Source2++))) >= 'A') && (l <= 'Z') )
			l -= ('A' - 'a');
	} while ( f && (f == l) );
	
	return (f - l);
}

//#define strnicmp(x,y,z) strncmp(x,y,z)
//#define stricmp(x,y) strcmp(x,y)

// Urgh.
//#define free(x)
#endif

#include <errorlib.h>

#ifdef BORISEMULATIONHACK
#include "./header/boris2emulation.h"
#endif

#ifndef DISKNAME
#define DISKNAME "\\pc"
#endif

#define MAXCAR 15

//#include <reg.h>

//#include "./header/ConsoleRegistry.h"

#include "./SOS1937/GlobalStructure.h"
#include "./header/IOFunctions.h"

#include "./header/FrontEndObject.h"
#include "./header/Buttons.h"
#include "./header/Slider.h"
#include "./header/alphabet.h"
#include "./header/word.h"
#include "./header/localeword.h"
#include "./header/screen.h"

#include "./SOS1937/SOS_Index.h"
#include "./header/main.h"

#endif __CONSOLEFRONTLIB_H__
