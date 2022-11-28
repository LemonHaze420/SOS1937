//================================================================================================================================
// spriteFX.cpp
// ------------
//
//================================================================================================================================

#include "BS2all.h"


extern ulong GBLmaterialIndex;



spriteFXframe::spriteFXframe( vector3 * pos,
						float xsize, float ysize, 
						float ucoord, float vcoord,
						float usize, float vsize)
{
	xlength = xsize;
	ylength = ysize;
	nv.u = ucoord;
	nv.v = vcoord;
	ulength = usize;
	vlength = vsize;
	offset = * pos;
}

spriteFXframe::~spriteFXframe()
{
}


void spriteFX::appendFrame( float pixelsX, float pixelsY, vector3 * pos,
						float xsize, float ysize, 
						float ucoord, float vcoord,
						float usize, float vsize,
						float xvar, float yvar, float wvar, float hvar, textureData * tdptr)
{
	
	float oneoverpixelsx = RECIPROCAL( pixelsX );
	float oneoverpixelsy = RECIPROCAL( pixelsY );
	spriteFXframe * freshFrame = new spriteFXframe( pos, xsize , ysize , 
		ucoord * oneoverpixelsx, vcoord * oneoverpixelsy, usize* oneoverpixelsx, vsize* oneoverpixelsy);
	

	if( firstFrame == NULL )
	{
		firstFrame = lastFrame = freshFrame;
		freshFrame->next = NULL;
		freshFrame->prev = NULL;
	} 
	else
	{
		lastFrame->next = freshFrame;
		freshFrame->prev = lastFrame;
		freshFrame->next = NULL;
		lastFrame = freshFrame;
	}

	freshFrame->xvariation = xvar;
	freshFrame->yvariation = yvar;
	freshFrame->wvariation = wvar;
	freshFrame->hvariation = hvar;

	freshFrame->mat.textureDataPtr= tdptr;

	freshFrame->mat.bm.textureHandle = tdptr->textureHandle;				 //registerTexture( (char * )dataPtr );
	freshFrame->mat.bm.flags = MAT_TCLAMP | MAT_SCLAMP | MAT_DOUBLESIDED | MAT_ALPHA;	  

	freshFrame->mat.diffuse = 0x00c0c0c0;
	freshFrame->mat.sortingKey   = ((tdptr->tindex) << (9 +16) ) + 
								( GBLmaterialIndex << 9);

	GBLmaterialIndex++;											 // each spriteFX material is different.
}


extern char tempBuffer[];

#define MYfread( a, b, c, d ) \
			if(fread( a, b, c, d ) != 1) \
				fatalError("Error reading World file")


//debugger DBGR;

float readFieldF( char ** cptr )
{
	char * kptr = * cptr;
	char k;
	float result;
	
	do
	{
		k = *kptr++;
	}
	while( ( k != 0 ) && ( k != ',' ) );

	if( k == ',' )
	{
		*(kptr-1) = 0;
	}
	result = (float)atof( * cptr );
	* cptr = kptr;

	return(result);
}

spriteFX::spriteFX(  char * filename )//, float zcastback )
{
	textureData * tdptr;

	firstFrame = NULL;
	lastFrame  = NULL;

	ydrift=false;
	engydrift=false;

	//
	//--  load in the description of the effect 
	//
	ulong  spriteDesc = arcExtract_txt( filename );


	ulong linecount = 0;
	while( arcGets_NoWhiteSpace( tempBuffer, 200, spriteDesc ) )
	{
		if( *tempBuffer != '#' )
		{
			switch(linecount++)
			{
			
				case 0:	framesPerSecond = (float)atof(tempBuffer);
						
						//if( framesPerSecond == 0 ) framesPerSecond = 0.0001f;
						break;
				case 1: repeatCount = atoi(tempBuffer);
						if (repeatCount == 0) repeatCount = 999999;
						break;
				case 2:	oscillate = ( (*tempBuffer) == 'T' );
						ydrift = ( tempBuffer[1] == 'T' );		
						break;
				default:
						// Each line:
						// pX,pY,pZ, W,H,  u,v,uW,vW,  dx,dy,dw,dh


						char * cptr = tempBuffer;
						float pX,pY,pZ, W, H, u,v,uW,vW, dx, dy, dw, dh, pixelsX, pixelsY;


						while( *cptr != ',' )
							cptr++;
						*cptr++ = 0;

						tdptr = getTexturePtr( tempBuffer, false );

						pixelsX = readFieldF( &cptr );
						pixelsY = readFieldF( &cptr );

						pX = readFieldF( &cptr );
						pY = readFieldF( &cptr );
						pZ = readFieldF( &cptr );

						W = readFieldF( &cptr );
						H = readFieldF( &cptr );
						u = readFieldF( &cptr );
						v = readFieldF( &cptr );
						uW = readFieldF( &cptr );
						vW = readFieldF( &cptr );
						dx = readFieldF( &cptr );
						dy = readFieldF( &cptr );
						dw = readFieldF( &cptr );
						dh = readFieldF( &cptr );
						appendFrame( pixelsX, pixelsY, &vector3(pX,pY,pZ),   W,H,   u,v,uW,vW,  dx,dy,dw,dh, tdptr );
						break;
			}
		}
	}
	arcDispose(spriteDesc);
}

spriteFX::~spriteFX()
{
	// needs to trace through and delete all frames

	spriteFXframe * nextFrame;

	while( firstFrame )
	{
		nextFrame = firstFrame->next;
		delete firstFrame;
		firstFrame = nextFrame;
	}
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
