//================================================================================================================================
// font.cpp
// ---------
//
//================================================================================================================================


#include <windows.h> 
#include <stdio.h>
#include <stdarg.h>

#include "BS2all.h"

extern char tempBuffer[];
extern char tempBuffer1[];

font::font( char * filename )		// filename = name of parameter file
{
	fontchar * fcptr;
	slong i;

		// clear down the defined fields

	fcptr = character;
	for(i = 128; i--; )
		(fcptr++)->defined = false;


	// - load font parameter file

	ulong  fonthandle = arcExtract_txt( filename );

	while( arcGets( tempBuffer1, 128, fonthandle ) ) 
	{
		char * strPtr ;
		char * outPtr ;
		char * colonPtr ;

		char k;

		strPtr = tempBuffer1;
		outPtr = tempBuffer;
		colonPtr = NULL;

		// Remove spaces, comments,  etc.


		k = *strPtr++;
		while( (k != 0) && ( k != '#' ) )
		{
			if(k == ':')
				colonPtr = outPtr;

			if((k != 32) && (k != 9)  && (k != 10) && (k != 13))
				*outPtr++ = k;

			k = *strPtr++;
		}
		*outPtr = 0;

		if( outPtr != tempBuffer )
		{
			//	spaces and comments removed

			if(colonPtr != NULL)
			{
				*colonPtr = 0;		// terminate prematurely for comparison

				if (strcmp(tempBuffer, "kerning") == 0)
				{
					kern = (float)atoi( colonPtr+1 );
				}
				else
				if (strcmp(tempBuffer, "lineheight") == 0)
				{
					lineheight = (float)atoi( colonPtr+1 );
				}
				else
				if (strcmp(tempBuffer, "space") == 0)
				{
					space = (float)atoi( colonPtr+1 );
				}
				else
				if (strcmp(tempBuffer, "width") == 0)
				{
					oneoverwidth = RECIPROCAL(atoi( colonPtr+1 ));
				}
				else
				if (strcmp(tempBuffer, "height") == 0)
				{
					oneoverheight = RECIPROCAL(atoi( colonPtr+1 ));
				}
				else
				if (strcmp(tempBuffer, "bitmap") == 0)
				{
					handle = (getTexturePtr( colonPtr+1, false ))->textureHandle;
				}
				else
				{
					char kk = *(tempBuffer + 5);
					*(tempBuffer + 5) = 0;
					if( strcmp(tempBuffer, "char-") == 0)
					{
						char whatchar;

						*(tempBuffer + 5) = kk;
						char * parameter = tempBuffer+5;
						if (strcmp(parameter, "colon") == 0)
						{
							whatchar = ':';
						}
						else
						if (strcmp(parameter, "space") == 0)
						{
							whatchar = 32;
						} 
						else
						{
							whatchar = *parameter;
						}

						// parse parameter list from 'colonPtr+1 ....

						if(whatchar < 128)
						{
							fcptr = &character[whatchar];
							fcptr->defined = true;
							parameter = colonPtr+1;

							long kcount;
							float amount;

							kcount = 0;
							amount = 0.0f;
									
							fcptr->x = fcptr->y = fcptr->w = fcptr->h = 0;
							while(1)
							{
								k = *parameter++;
								if( (k == ',') || (k == 0))
								{
									switch(kcount)
									{
										case 0:
											fcptr->x = amount;
											break;
										case 1:
											fcptr->y = amount;
											break;
										case 2:
											fcptr->w = amount;
											break;
										case 3:
											fcptr->h = amount;
											break;
										default:
											break;
									}
									kcount++;
									amount = 0;
								} 
								else
									amount = amount * 10.0f + (float)((k - '0'));

								if( k==0) 
									break;


							}
						}
					}
				}
			}		// if no colon then skip the line - we dont know what it means
		}
	}
	arcDispose( fonthandle );		// get rid of archive font file - data now loaded in.
}

int font::getSize(char *s)
{
	float xposn;
	xposn = 0;
	long tx;

	while(*s)
	{
		fontchar * fcptr = &character[*s];

		if(fcptr->defined)
		{
			xposn += kern + fcptr->w;
		} 
		else
		{
			if( *s == 32 )
				xposn += space;
		}
		s++;
	}
	FASTFTOL(&tx,xposn - 0.5f);
	return (int)tx;
}


void font::RenderString(int xd,int yd,char *s,justType justification)
{
	float xposn;

	if(justification != JUST_LEFT)
	{
		int wid = getSize(s);

		if(justification == JUST_RIGHT)
			xd -= wid;
		else
			xd -= wid/2;
	}

	xposn = (float)xd;

	while(*s)
	{
	
		fontchar * fcptr = &character[*s];

		if(fcptr->defined)
		{
			boris2->addTo2Dlist( xposn, (float)yd, fcptr->w, fcptr->h,
					 fcptr->x * oneoverwidth,  fcptr->y * oneoverheight,   //0.0f, 0.0f, 1.0f, 1.0f,
					 fcptr->w * oneoverwidth,  fcptr->h * oneoverheight,   //fcptr->h,
					 handle);
			xposn += kern + fcptr->w;
		} 
		else
		{
			if( *s == 32 )
				xposn += space;
		}
		s++;
	}
}

void font::printf(int xd,int yd,char *s,...)
{
	char buf[256],*q;
	va_list argptr;
	justType just = JUST_LEFT;

	va_start(argptr,s);
	vsprintf(buf,s,argptr);

	q=buf;

	if(buf[0]=='%')
	{
		if(buf[1]=='c')just=JUST_CENTRE;
		else if(buf[1]=='r')just=JUST_RIGHT;
		q+=2;
	}

	RenderString(xd,yd,q,just);
	va_end(argptr);
}


font::~font()
{
	unregisterTexture(handle);
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
