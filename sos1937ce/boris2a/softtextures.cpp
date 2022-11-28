//================================================================================================================================
// softTextures.cpp
// ----------------
//
//	software texture mapping.
// --------------------------
//
//  (i) change definition of vector2P so that we have for x,y,z3d:
//													     union { float x; slong xi; } etc.
//	    also change 'nonGeoData's 'u','v''l' to have similar unions.
//
//	(ii) When software rendering is used, directly before rendering the triangle lists scan the
//		 list of 2D vertices and nonGeoData list to convert each float to integer<<16.
//
//
//  THIS CODE MUST BE CONVERTED TO ASSEMBLER ONCE STABLE
//
//================================================================================================================================

#include "BS2all.h"
#include <malloc.h>

#define MAXSOFTHANDLES 4096
DWORD SOFTnextHandle;
char * SOFThandleToTexture[ MAXSOFTHANDLES ];
BorisMaterial * SOFTcurrentMaterial;

//debugger DBGR;



long zbuffer[1024 * 768];

long SOFTdeltaTable[ 12025 ];
float SOFTrecip64table[ 12025 ];

void SOFTrenderStart3D()
{
	long i;
	char ** thisSoftHandle = SOFThandleToTexture;

//	boris2->clipOff();		// Renderer deals with clipping!!!
	
	for( i = MAXSOFTHANDLES; i--; )
	{
		*thisSoftHandle++ = NULL;
	};
	SOFTnextHandle = 1;			//ignore 0 = dummy handle
	SOFTcurrentMaterial = NULL;

	SOFTdeltaTable[0] = 65535;
	SOFTrecip64table[0] = 0;
	for( i = 1; i<=12024; i++ )
	{
		SOFTdeltaTable[i] = 65536/(i);
		SOFTrecip64table[i] = 65536.0f / ((float)(i));

	};




//	DBGR.Dprintf("ELLO?\n");

};

void SOFTrenderSetCurrentMaterial(int r, int g, int b)
{
		// set the colour for a material to 'r' 'g' 'b' - to use with rendering a single triangle

//	SOFTcurrentMaterial = NULL;
};

void SOFTrenderSetCurrentMaterial(BorisMaterial *m1,BorisMaterial *m2)
{
	SOFTcurrentMaterial = m1;
};

void SOFTrenderTriangle(rTriangle* tri)// REDUNDANT FUNCTION - dont bother to body it!!
{

};		


//unsigned char * DUFFptr;
	
float deltaU, deltaV, deltaZ;
float z01, z02, z12;
float z0, z1, z2;
float zvalue;
float delta02z;
long pitch;
char * texPtr, * screenPtr;
float x02;
float u02;
float v02;
long * SOFTdeltaTableP;
long * zbufferPtr;
float delta02x;
float delta02u;
float delta02v;

long ycount;

float Px01; 
float Pu01; 
float Pv01; 
float Pz01;
float Pdelta01u, Pdelta01v,Pdelta01x, Pdelta01z;

float FirstX;


long roundedup( float f )
{
	return(long( f + 0.5f ));
};


long xcount, yy;
float xStart, xEnd, uStart, uEnd, vStart, vEnd, zStart, zEnd;
char  * sPtr;
char  * sPtrx;
long * zPtr;
long * zPtrx;
float k;


long LdeltaZ, LzStart, LdeltaU, LuStart, LdeltaV, LvStart;

void coreLoopNonAlpha()
{

	LdeltaZ = (long)( deltaZ * 256 * 1024 );
	LzStart = (long)( zStart * 256 * 1024 );
	LdeltaU = (long)( deltaU * 65536 );	
	LuStart = (long)( uStart * 65536 );
	LdeltaV = (long)( deltaV * 65536 );
	LvStart = (long)( vStart * 65536 );
	_asm{

		mov  esi, zPtrx
		mov  edi, sPtrx
		push ecx
		push ebp
		mov  ecx, xcount
		lea  esi, [esi + ecx*4]
		lea  edi, [edi + ecx*2]
		neg  ecx
		mov  ebp, LzStart
		mov  ebx, texPtr
		jmp  N01

		N02:
			 mov  eax, LdeltaU
			 mov  edx, LdeltaV
			 add  LuStart, eax
			 add  LvStart, edx
			 add  ebp, LdeltaZ
			 inc  ecx
			 jns  N04

		N01:
			 cmp [esi + ecx*4], ebp				// zbuffer test
			 jle  N02
	
			 mov ah, byte ptr [LvStart + 2]	
			 xor edx, edx
 			 mov al, byte ptr [LuStart + 2]	 //  u is twice as big as we need it so that it is left justified in a byte
			 shr eax, 1
			 and eax, 0x03FFF				
			 mov dl,  [eax + ebx + 8 + 512]
			 mov Eax,  [edx*2 + ebx + 8]
			 mov [esi + ecx*4], ebp
			 mov word ptr [edi + ecx*2], ax

			 add  ebp, LdeltaZ
			 mov  eax, LdeltaU
			 mov  edx, LdeltaV
			 add LuStart, eax
			 add LvStart, edx
			 
			 
		inc  ecx
		js   N01
N04:
		pop  ebp
		pop  ecx
	};

};


void coreLoopAlpha()
{

	LdeltaZ = (long)( deltaZ * 256 * 1024 );
	LzStart = (long)( zStart * 256 * 1024 );
	LdeltaU = (long)( deltaU * 65536 );	
	LuStart = (long)( uStart * 65536 );
	LdeltaV = (long)( deltaV * 65536 );
	LvStart = (long)( vStart * 65536 );
	_asm{

		mov  esi, zPtrx
		mov  edi, sPtrx
		push ecx
		push ebp
		mov  ecx, xcount
		lea  esi, [esi + ecx*4]
		lea  edi, [edi + ecx*2]
		neg  ecx
		mov  ebp, LzStart
		mov  ebx, texPtr
		jmp  A01

		A02:						// end of loop when alpha pixel ... avoids another branch

			 mov  eax, LdeltaU
			 mov  edx, LdeltaV
			 add LuStart, eax
			 add LvStart, edx
			 add ebp, LdeltaZ
			 inc ecx
			 jns  A04

		A01:
			 cmp [esi + ecx*4], ebp					// zbuffer test
			 jle  A02
				
			 mov ah, byte ptr [LvStart + 2]
			 xor edx, edx
			 mov al, byte ptr [LuStart + 2]
			 shr eax, 1
			 and eax, 0x03FFF
			 or  dl,  [eax + ebx + 8 + 512]			// since edx==0 already
			 je  A02

			 mov Eax,  [edx*2 + ebx + 8]
			 mov [esi + ecx*4], ebp					// zbuffer only updated when outputing the pixel
			 mov word ptr [edi + ecx*2], ax
			
			 mov  eax, LdeltaU
			 mov  edx, LdeltaV
			 add  LuStart, eax
			 add  LvStart, edx
			 add  ebp, LdeltaZ

			 inc  ecx
			 js   A01
		A04:
			 pop  ebp
			 pop  ecx
	};


};
				
void renderTriangleNonAlphaHI( )  
{  
	sPtr = screenPtr;
	zPtr = zbufferPtr;

	if(ycount)
	for( yy = ycount ; yy--; )
	{
		//xcount = (long)( Px01 - x02);
		if ( x02 <= Px01 )
		{
			xStart = (long)x02 ;
			xEnd   = (long)Px01 ;
		//	if( ( xStart < 639 ) && (xEnd >= 0)) 
			{
				uStart = u02 * 256;
				uEnd   = Pu01 * 256;
				vStart = v02 * 128;
				vEnd   = Pv01* 128;
				zStart = z02;
				zEnd   = Pz01;
			
				xcount = 1 + xEnd - xStart;

				//if(Px01 - x02)
				// k = RECIPROCAL(Px01 - x02); //xEnd - xStart);		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);
				
				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
			
				if(xcount > 0)
					coreLoopNonAlpha();

				
			}
		} else
		{
			xEnd = (long)x02 ;
			xStart   = (long)Px01 ;
			//if( ( xStart < 639 ) && (xEnd >= 0)) 
			{
				uEnd = u02 * 256;
				uStart   = Pu01 * 256;
				vEnd = v02 * 128;
				vStart   = Pv01 * 128;
				zEnd = z02;
				zStart   = Pz01;
			
				xcount = 1 + xEnd - xStart;

				//if(x02 - Px01)
				// k = RECIPROCAL(x02 - Px01); //xEnd - xStart);		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));

				if(xcount > 0)
					coreLoopNonAlpha();

			}

		};

		x02 += delta02x;
		u02 += delta02u;
		v02 += delta02v;
		z02 += delta02z;
		Px01 += Pdelta01x;
		Pu01 += Pdelta01u;
		Pv01 += Pdelta01v;
		Pz01 += Pdelta01z;

		sPtr += pitch;
		zPtr += 1024;
		screenPtr = sPtr;
		zbufferPtr = zPtr;
	};
};




void renderTriangleNonAlphaLO( )  
{  
	sPtr = screenPtr;
	zPtr = zbufferPtr;


	if(ycount)
	for( yy = ycount; yy--; )
	{
		
		Px01 += Pdelta01x;
		Pu01 += Pdelta01u;
		Pv01 += Pdelta01v;
		Pz01 += Pdelta01z;

		
		if ( x02 <= Px01 )
		{
			
			xStart = (long)x02 ;
			xEnd   = (long)Px01 ;
			{
				uStart = u02 * 256;
				uEnd   = Pu01 * 256;
				vStart = v02 * 128;
				vEnd   = Pv01* 128;
				zStart = z02;
				zEnd   = Pz01;
			
				xcount = 1 + xEnd - xStart;
		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));

							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
							
				if(xcount > 0)
					coreLoopNonAlpha();

			}
		} else
		{
			xEnd = (long)x02 ;
			xStart   = (long)Px01 ;
			{
				uEnd = u02 * 256;
				uStart   = Pu01 * 256;
				vEnd = v02 * 128;
				vStart   = Pv01 * 128;
				zEnd = z02;
				zStart   = Pz01;
			
				xcount =  1 + xEnd - xStart;


				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
				
					
				if(xcount > 0)
					coreLoopNonAlpha();

			}

		};

		x02 += delta02x;
		u02 += delta02u;
		v02 += delta02v;
		z02 += delta02z;
		

		sPtr += pitch;
		zPtr += 1024;
		screenPtr = sPtr;
		zbufferPtr = zPtr;
	};


};



void renderTriangleAlphaHI( )  
{  
	sPtr = screenPtr;
	zPtr = zbufferPtr;

	if(ycount)
	for( yy = ycount ; yy--; )
	{
		if ( x02 <= Px01 )
		{
			xStart = (long)x02 ;
			xEnd   = (long)Px01 ;
			{
				uStart = u02 * 256;
				uEnd   = Pu01 * 256;
				vStart = v02 * 128;
				vEnd   = Pv01* 128;
				zStart = z02;
				zEnd   = Pz01;
			
				xcount = 1 + xEnd - xStart;
	
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);
				
				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
			
				if(xcount > 0)
					coreLoopAlpha();
				
			}
		} else
		{
			xEnd = (long)x02 ;
			xStart   = (long)Px01 ;
			//if( ( xStart < 639 ) && (xEnd >= 0)) 
			{
				uEnd = u02 * 256;
				uStart   = Pu01 * 256;
				vEnd = v02 * 128;
				vStart   = Pv01 * 128;
				zEnd = z02;
				zStart   = Pz01;
			
				xcount = 1 + xEnd - xStart;

				//if(x02 - Px01)
				// k = RECIPROCAL(x02 - Px01); //xEnd - xStart);		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));

				if(xcount > 0)
					coreLoopAlpha();

			}

		};

		x02 += delta02x;
		u02 += delta02u;
		v02 += delta02v;
		z02 += delta02z;
		Px01 += Pdelta01x;
		Pu01 += Pdelta01u;
		Pv01 += Pdelta01v;
		Pz01 += Pdelta01z;

		sPtr += pitch;
		zPtr += 1024;
		screenPtr = sPtr;
		zbufferPtr = zPtr;
	};
};


void renderTriangleAlphaLO( )  
{  
	sPtr = screenPtr;
	zPtr = zbufferPtr;

	if(ycount)
	for( yy = ycount; yy--; )
	{
		
		Px01 += Pdelta01x;
		Pu01 += Pdelta01u;
		Pv01 += Pdelta01v;
		Pz01 += Pdelta01z;

		
		if ( x02 <= Px01 )
		{
			
			xStart = (long)x02 ;
			xEnd   = (long)Px01 ;
			//if( ( xStart < 639 ) && (xEnd >= 0)) 
			{
				uStart = u02 * 256;
				uEnd   = Pu01 * 256;
				vStart = v02 * 128;
				vEnd   = Pv01* 128;
				zStart = z02;
				zEnd   = Pz01;
			
				xcount = 1 + xEnd - xStart;

//				if(Px01 - x02)
//				 k = RECIPROCAL(Px01 - x02); //xEnd - xStart);		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));

							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

 				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
							
				if(xcount > 0)
					coreLoopAlpha();

			}
		} else
		{
			xEnd = (long)x02 ;
			xStart   = (long)Px01 ;
			//if( ( xStart < 639 ) && (xEnd >= 0)) 
			{
				uEnd = u02 * 256;
				uStart   = Pu01 * 256;
				vEnd = v02 * 128;
				vStart   = Pv01 * 128;
				zEnd = z02;
				zStart   = Pz01;
			
				xcount =  1 + xEnd - xStart;

//				if(x02 - Px01)
//				 k = RECIPROCAL(x02 - Px01); //xEnd - xStart);		
				if( xcount )
					k = RECIPROCAL(((float)(xcount)));
							
				deltaU = k * (uEnd- uStart);
				deltaV = k * (vEnd- vStart);
				deltaZ = k * (zEnd- zStart);

				sPtrx = sPtr + ( ((long)(xStart))<<1);
				zPtrx = zPtr + ((long)(xStart));
				
					
				if(xcount > 0)
					coreLoopAlpha();

			}

		};

		x02 += delta02x;
		u02 += delta02u;
		v02 += delta02v;
		z02 += delta02z;
		

		sPtr += pitch;
		zPtr += 1024;
		screenPtr = sPtr;
		zbufferPtr = zPtr;
	};


};



void QFASTFTOL( long *l, float f )
{
	*l = f;
};

void SOFTrenderList(D3DTLVERTEX* list,int numVert,BOOL wireframe,BOOL clip)
{
	//
	// Every 3 vertex entries forms a triangle. If the current material is NULL then
	// render a plain coloured triangle (no texture) otherwise check the current material
	// flags. If MAT_ALPHA then render alpha ( char byte 0 = transparent ) otherwise
	// render simply (opaque).
	//

	if(SOFTcurrentMaterial->flags & MAT_SHADOW)
		return;

	D3DTLVERTEX * c0, * c1, * c2, * tempC;
	float y0, y1, y2;


	pitch = boris2->videoPitch;		// bytes per raster scan line


if(numVert)
	for( long ijk = 0; ijk < numVert; ijk+=3 )
	{
	   
		float tempL;
		float k;

		c0 = list++; c1 = list++; c2 = list++;
/*
static D3DTLVERTEX v0, v1, v2;
   // union {        D3DVALUE sx;          D3DVALUE dvSX;    };    union {
     //   D3DVALUE sy;        D3DVALUE dvSY;    };    union {        D3DVALUE sz;
     //   D3DVALUE dvSZ;    };    union {        D3DVALUE rhw; 
     //   D3DVALUE dvRHW;    };    union {        D3DCOLOR color;  
     //   D3DCOLOR dcColor;    };    union {        D3DCOLOR specular;   
     //   D3DCOLOR dcSpecular;    };    union {        D3DVALUE tu;  
     //   D3DVALUE dvTU;    };    union {        D3DVALUE tv;
     //   D3DVALUE dvTV;    };

v0.sx = 50;
v0.sy = 50;
v0.sz = 0.5f;
v0.tu = 0;
v0.tv = 0;

v1.sx = 250;
v1.sy = 200;
v1.sz = 0.5f;
v1.tu = 0.5f;
v1.tv = 0.75f;

v2.sx = 450;
v2.sy = 250;
v2.sz = 0.5f;
v2.tu = 1;
v2.tv = 1;

c0 = &v0; c1 = &v1; c2 = &v2;
*/

			// order the vertices in terms of ascending 2D y values

		 y0 =  c0->sy;
		 y1 =  c1->sy;
		 y2 =  c2->sy;


		 if( y0 > y1 )
		 { 
		   tempL = y0; y0 = y1; y1 = tempL;
		   tempC = c0; c0 = c1; c1 = tempC;	
		 };
		 if( y0 > y2 )
		 { 
		   tempL = y0; y0 = y2; y2 = tempL;
		   tempC = c0; c0 = c2; c2 = tempC;	
		 };
		 if( y1 > y2 )
		 { 
		   tempL = y1; y1 = y2; y2 = tempL;
		   tempC = c1; c1 = c2; c2 = tempC;	
		 };
//if(( y0<0 ) || (y1<0) || (y2<0)) continue;

		float c0u = c0->tu ;
		float c0v = c0->tv ;
		float c1u = c1->tu ;
		float c1v = c1->tv ;
		float c2u = c2->tu ;
		float c2v = c2->tv ;


if(SOFTcurrentMaterial->flags & MAT_TCLAMP)
{
	if( c0u < 0.0f ) c0u = 0.0f;
	else
		if( c0u > 1.0f ) c0u = 1.0f;

	if( c1u < 0.0f ) c1u = 0.0f;
	else
		if( c1u > 1.0f ) c1u = 1.0f;
		
	if( c2u < 0.0f ) c2u = 0.0f;
	else
		if( c2u > 1.0f ) c2u = 1.0f;
};

if(SOFTcurrentMaterial->flags & MAT_SCLAMP)
{
	if( c0v < 0.0f ) c0v = 0.0f;
	else
		if( c0v > 1.0f ) c0v = 1.0f;

	if( c1v < 0.0f ) c1v = 0.0f;
	else
		if( c1v > 1.0f ) c1v = 1.0f;
		
	if( c2v < 0.0f ) c2v = 0.0f;
	else
		if( c2v > 1.0f ) c2v = 1.0f;
};

		float  y2y0 = y2-y0;

		 if( y2y0 > 1.0f )		// ignore single line triangles!!
		 {
	
    		screenPtr = boris2->videoPtrActual + ((long)y0) * pitch;// starting ROW position (no x)
			zbufferPtr = &zbuffer[((long)y0) * 1024];

			long texhandle    = SOFTcurrentMaterial->textureHandle;
			texPtr = SOFThandleToTexture[ texhandle];			// if tex handle is 0 ?

		

			long ystart = (long) y0;
			long yend =   (long) y1;
			long yveryend = (long) y2;

			ycount = 1 + yend - ystart;

			k = 0;

			if( 1 + yveryend - ystart )
				k = RECIPROCAL( ((float)(1 + yveryend - ystart) ) );

			delta02x = ( c2->sx - c0->sx ) * k;
			delta02u = ( c2u - c0u ) * k;
			delta02v = ( c2v - c0v ) * k;
			delta02z = ( c2->sz - c0->sz ) * k;

			Px01 = c0->sx;
			Pu01 = c0u;
			Pv01 = c0v;
			Pz01 = c0->sz;

			x02 = Px01;
			u02 = Pu01;
			v02 = Pv01;
			z02 = Pz01;
		
			k = 0;

			if( ycount ) 
				k = RECIPROCAL( (float)ycount );


			Pdelta01x = ( c1->sx - c0->sx )* k;
			Pdelta01u = ( c1u - c0u )* k;
			Pdelta01v = ( c1v - c0v )* k;
			Pdelta01z = ( c1->sz - c0->sz )* k;
			
				
			SOFTdeltaTableP = SOFTdeltaTable;

	if( (texhandle!=0) && (texPtr !=0) )
	{
	
		if(SOFTcurrentMaterial->flags & MAT_ALPHA )
		{
			
				if(ycount)
					renderTriangleAlphaHI();

				long ystart = (long) y1;
				long yend   = (long) y2;
				ycount = 1 +yend - ystart;


				  if( ycount )
				  {
					k = RECIPROCAL( ycount );
				
					Px01 = c1->sx;
					Pu01 = c1u;
					Pv01 = c1v;
					Pz01 = c1->sz;
					Pdelta01u = ( c2u - c1u )* k;
					Pdelta01v = ( c2v - c1v )* k;
					Pdelta01x = ( c2->sx - c1->sx ) * k;
					Pdelta01z = ( c2->sz - c1->sz ) * k;

					if( ycount)
						renderTriangleAlphaLO();

				};


		} else			// NOT ALPHA
		{
				
				if(ycount)
					renderTriangleNonAlphaHI();

				long ystart = (long) y1;
				long yend   = (long) y2;
				ycount = 1 + yend - ystart;
				
				  if( ycount )
				  {
					k = RECIPROCAL( ycount );
								
					Px01 = c1->sx;
	 				Pu01 = c1u;
					Pv01 = c1v;
					Pz01 = c1->sz;
					Pdelta01u = ( c2u - c1u )* k;
					Pdelta01v = ( c2v - c1v )* k;
					Pdelta01x = ( c2->sx - c1->sx ) * k;
					Pdelta01z = ( c2->sz - c1->sz ) * k;

					if( ycount)
						renderTriangleNonAlphaLO();

				};


		}	// else...
			
	} else
	{
			// no texture - plain colour

	};



		 }

	};
};

void SOFTrenderList(D3DTLVERTEX2* list,int numVert,BOOL wireframe,BOOL clip)
{
		// Do virtually exactly the same as the D3DTLVERTEX version because we dont allow
		// environment mapping in software (yet!)

//	DBGR.Dprintf("Env. Mapped Stuff\n");

};

//debugger DBGR;

DWORD SOFTregisterTexture(unsigned char *pointerToFile,int TextureStage, int *width, int *height)
{
	unsigned char * bitmapmemory;

	//  pointerToFile points to memory image of loaded file.

	//
	//  - read off the data and allocate memory to hold the bitmap and its palette.
	//
	//  The memory will be layed out as  
	//					4 byte pixel width
	//					4 byte pixel height
	//

//	typedef struct tagBITMAPINFOHEADER{ // bmih 
//  DWORD  biSize; 
//  LONG   biWidth; 
//  LONG   biHeight; 
//   WORD   biPlanes; 
//  WORD   biBitCount 
//   DWORD  biCompression; 
//   DWORD  biSizeImage; 
//   LONG   biXPelsPerMeter; 
//   LONG   biYPelsPerMeter; 
//   DWORD  biClrUsed; 
//   DWORD  biClrImportant; 
//	} BITMAPINFOHEADER; 
 
	{
		static BITMAPFILEHEADER	bmfh;
		static BITMAPINFOHEADER	bmih;
		static LONG pal[256];
		static long pal16[256];//,*outp;
		unsigned long colour, i;
		int x,y;//,pitch;
		int myHandle=0;

//unsigned char * ppp = pointerToFile;
		memcpy(&bmfh,pointerToFile,sizeof(bmfh));
		pointerToFile += sizeof(bmfh);
		memcpy(&bmih,pointerToFile,sizeof(bmih));
		pointerToFile += sizeof(bmih);
		memcpy(pal,pointerToFile, sizeof(pal));
		pointerToFile += sizeof(pal);


//pointerToFile = ppp + bmfh.bfOffBits;

//long	pitch = bmih.biSizeImage / bmih.biHeight;

		//for(i=0;i<256;++i)
		//	pal16[i] = ((pal[i]&0xff)>>3) | ((pal[i]&0xfc00)>>5) | ((pal[i]&0xf80000)>>8);

		long hhh =  bmih.biHeight;
		if(hhh<0) hhh = -hhh;
		bitmapmemory = (unsigned char *)malloc( 4 + 4 + 512 + bmih.biWidth * hhh    +4);	// the last '4' is to allow 
																							// dword access of last word in
																							// optimised spftware renderer

//		char * kptr = bitmapmemory;
//		for( long jj = 128*128;jj--;)
//		{
//			*kptr++ = 0;
//		};

//DUFFptr = bitmapmemory;
		unsigned char * optr = bitmapmemory;
		char * palptr = (char *)&pal[0];
		long * pal16ptr = pal16;

		*((long *)optr) = bmih.biWidth;
		optr+=4;
		*((long *)optr) = hhh;
		optr+=4;

		for( i = 256; i--; )
		{
			
			unsigned char blu = *palptr++;
			unsigned char grn = *palptr++;
			unsigned char red = *palptr++;
			palptr++;

			if( Format16 == RGB_555)
			{
				*pal16ptr = ( (  ((ulong)(red >> 3)) << 10) +
							  (  ((ulong)(grn >> 3)) << 5) +
							     ((ulong)(blu >> 3))
									);
			} else
			{
				*pal16ptr = ( (((ulong)(red >> 3)) << 11) +
									(((ulong)(grn >> 2)) << 5) +
									((ulong)(blu >> 3))
									);
			};

			*((WORD *) optr) = (WORD)*pal16ptr;
			pal16ptr++;
			optr += 2;
		};
		long j;

		//memcpy(optr, pointerToFile, bmih.biWidth * hhh);

		
		unsigned char * xptr = optr + bmih.biWidth * (hhh-1);

		for( j = hhh; j--; )		// since lines are even...
		{
			unsigned char * yptr = xptr;
			for( i = bmih.biWidth; i--; )		// since lines are even...
			{
				*yptr = *pointerToFile;
				pointerToFile++;
				yptr++;
			};
			xptr -= bmih.biWidth;
		};

//	static char zzz[222];
//	sprintf(zzz,"%d %d \n",bmih.biWidth,bmih.biHeight);
//	DBGR.Dprintf(zzz);
	}



	//  find first available handle, set up its pointer to link to the bitmap

	char ** thisSoftHandle = SOFThandleToTexture;

	thisSoftHandle++;			// Always start at entry 1 (0 = dummy)
	DWORD counter = 1;
	while( *thisSoftHandle )
	{
			counter++;
			thisSoftHandle++;
	};

	*thisSoftHandle = (char *)bitmapmemory;

//	static char zzz[222];
//	sprintf(zzz,"%d\n", counter);
//	DBGR.Dprintf(zzz);

	return( counter );
};



void SOFTunregisterTexture(DWORD handle)
{
	char * memPtr = SOFThandleToTexture[handle];
	if( memPtr )
	{
		free( memPtr );
	};
	SOFThandleToTexture[handle] = 0;
};



DWORD SOFTisTextureAlpha(DWORD handle)
{
	

	char * memPtr = (char *)SOFThandleToTexture[ handle ];	

	long width = *(long *)memPtr;    memPtr+=4;
	long height = *(long *)memPtr;    memPtr+=4;
	char * imageData = memPtr + 512;

	for( long i = width * height; i--; )
	{
		if (*imageData == 0) 
			return(1);
	
		imageData++;
	};

	return(0);
};



void SOFTSetFogColor(DWORD color)
{};

void SOFTrenderFog(BOOL fog)
{};

//extern material dummyMaterial;

void SOFTrenderBeginScene( bool clearscreen )
{

	// purge the zbuffer....

	long * zptr = zbuffer;
			//for( long i = 1024*768;i--;)
			//	*zptr++ = 0x07FFFFFFF;
	_asm{

		mov ebx, zptr
		mov edx, 1024 * 768
		mov eax, 0x07FFFFFFF

		lea ebx, [ebx + edx * 4]
		neg edx

		Z00:
			mov dword ptr [ebx + edx * 4], eax
			mov dword ptr [ebx + edx * 4 + 4], eax

		add edx, 2
		js  Z00

	};

//	SOFTcurrentMaterial = &dummyMaterial;

};



//void SOFTrenderFog(bool fogonoroff )
//{
//	//SOFTfog = fogonoroff;
//};

//void SOFTSetFogColor( ulong fogColour )
//{
//	//SOFTfogcolor = fogColour;				// need to translate into  16-bit form from argb form.
//};


//================================================================================================================================
//END OF FILE
//================================================================================================================================
