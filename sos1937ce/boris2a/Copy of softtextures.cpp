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

debugger DBGR;



long zbuffer[1024 * 768];

long SOFTdeltaTable[ 2025 ];
float SOFTrecip64table[ 2025 ];

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
	for( i = 1; i<=2024; i++ )
	{
		SOFTdeltaTable[i] = 65536/(i);
		SOFTrecip64table[i] = 65536.0f / i;

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
	
long deltaU, deltaV, deltaZ;
long z01, z02, z12;
long z0, z1, z2;
long zvalue;
long delta02z;
long pitch;
char * texPtr, * screenPtr;
long x02;
long u02;
long v02;
long * SOFTdeltaTableP;
long * zbufferPtr;
long delta02x;
long delta02u;
long delta02v;

long Py1y0;
long Px01; 
long Pu01; 
long Pv01; 
long Pz01;
long Pdelta01u, Pdelta01v,Pdelta01x, Pdelta01z;



void renderTriangleNonAlpha( )  
{  

				_asm{
							pushad
							mov ecx, Py1y0
							mov eax, Px01
							mov ebx, x02
							mov edi, screenPtr
							mov esi, zbufferPtr
						L00:
							push ecx
								push edi
								push esi


									mov edx, ebx	// x02
									mov ecx, eax	// x01

									cmp ecx, edx
									jl  RRR
								
									shr edx, 16
									shr ecx, 16

									lea edi, [edi + ecx*2]
									lea esi, [esi + ecx*4]
									sub ecx, edx	// x01 - x02

									pushad
										push ecx
										 mov  edx, SOFTdeltaTableP
										 mov  ebx, [edx + ecx*4 ]		
									 mov  eax, Pz01
									 mov  edx, z02
									 sub  eax, edx
							//		 sar  eax, 16
									 //imul eax, ebx
									   push edx
									   imul ebx
									   shrd eax, edx, 16
									   pop  edx

									 mov  deltaZ, eax
									 mov  zvalue, edx
			 							 mov  eax, Pu01
										 mov  edx, u02
										 sub  eax, edx				
							//			 sar  eax, 16
									 	    //imul eax, ebx			
											push edx
											imul ebx
											shrd eax, edx, 16
											pop edx
										 mov  deltaU, eax
										 mov  eax, Pv01
										 mov  ecx, v02
										 sub  eax, ecx			
							//			 sar  eax, 16
											//imul eax, ebx	
										 	push edx
											imul ebx
											shrd eax, edx, 16
											pop edx
										 mov  deltaV, eax
										pop  ebx
										neg  ebx
										push ebp
										mov  ebp, texPtr
										L01:
										  push ecx

											  push edx
											  mov edx, zvalue
											  cmp [esi + ebx*4], edx
											  ja  L03
											  pop edx
 											  jmp  L04
 											  L03:
 										      mov [esi + ebx*4], edx
											  pop edx

											  mov eax, edx
											  shr ecx, 9
											  and ecx, 3F80h
											  shr eax, 16
											  
											  and eax, 0x0000007F
											  or  cl, al
											  mov  al, [ebp + ecx + 8 + 512]
											  mov  ax, [ebp + eax*2]
//mov ax, 0x07C00
											  mov word ptr [edi + ebx*2], ax
											L04:
  
											mov eax,zvalue
											add eax, deltaZ
											mov zvalue, eax
										  pop ecx
											add edx, deltaU
											add ecx, deltaV
											inc ebx
										js L01
										jmp X01

RRR:		// RHS edge
											
//jmp tttt
									mov edx, eax	// x01
									mov ecx, ebx	// x02

									shr edx, 16
									shr ecx, 16

									lea edi, [edi + ecx*2]
									lea esi, [esi + ecx*4]
									sub ecx, edx


									pushad
										push ecx
										 mov  edx, SOFTdeltaTableP
										 mov  ebx, [edx + ecx*4 ]		
									 mov  edx, Pz01
									 mov  eax, z02
									 sub  eax, edx
									// sar  eax, 16
										//imul eax, ebx
										push edx
										imul ebx
										shrd eax, edx, 16
										pop edx
									 mov  deltaZ, eax
									 mov  zvalue, edx
										 mov  edx, Pu01
										 mov  eax, u02
										 sub  eax, edx				
									//	 sar  eax, 16
									 	 //imul eax, ebx			
										 	push edx
											imul ebx
											shrd eax, edx, 16
											pop edx
										 mov  deltaU, eax
										 mov  ecx, Pv01
										 mov  eax, v02
										 sub  eax, ecx			
									//	 sar  eax, 16
											// imul eax, ebx	
											push edx
											imul ebx
											shrd eax, edx, 16
											pop edx
										 mov  deltaV, eax
										pop  ebx
										neg  ebx
										push ebp
										mov  ebp, texPtr
										R01:
										  push ecx
											push edx
											mov edx, zvalue
											cmp [esi + ebx*4], edx
											ja  R03
											pop edx
											jmp  R04
											R03:
										    mov [esi + ebx*4], edx
											pop edx
											  mov eax, edx
  											  shr ecx, 9
											  and ecx, 3F80h
											  shr eax, 16

											  and eax, 0x0000007F
											  or  cl, al
											  mov  al, [ebp + ecx + 8 + 512]
											  mov  ax, [ebp + eax*2]
//mov ax, 0x03E0
											  mov word ptr [edi + ebx*2], ax
											R04:
											mov eax,zvalue
											add eax, deltaZ
											mov zvalue, eax
										  pop ecx
											add edx, deltaU
											add ecx, deltaV
											inc ebx
										js R01
									
X01:
										pop ebp
									popad
tttt:
//RRR:
								pop esi
								pop edi
								add eax, Pdelta01x
								add ebx, delta02x
								mov edx, Pdelta01u
								mov ecx, delta02u
								add Pu01, edx
								add u02, ecx
								mov edx, Pdelta01v
								mov ecx, delta02v
								add Pv01, edx
								add v02, ecx
								mov edx, Pdelta01z
								mov ecx, delta02z
								add Pz01, edx
								add z02, ecx
								add edi, pitch
								add esi, 1024*4
							pop ecx
							dec ecx
							jns L00
							mov screenPtr, edi
							mov zbufferPtr, esi
							mov x02, ebx
							popad
				};
};



void renderTriangleAlpha( )  
{  
//	return;
				_asm{
							pushad
							mov ecx, Py1y0
							mov eax, Px01
							mov ebx, x02
							mov edi, screenPtr
							mov esi, zbufferPtr
						AL00:
							push ecx
								push edi
								push esi
									mov edx, ebx	// x02
									mov ecx, eax	// x01
									

									cmp ecx, edx
									jl  ARRR

									shr edx, 16
									shr ecx, 16

									lea edi, [edi + ecx*2]
									lea esi, [esi + ecx*4]

									sub ecx, edx	// x01 - x02
									pushad
										push ecx
										 mov  edx, SOFTdeltaTableP
										 mov  ebx, [edx + ecx*4 ]		
									 mov  eax, Pz01
									 mov  edx, z02
									 sub  eax, edx
									 sar  eax, 16
									 imul eax, ebx
									 mov  deltaZ, eax
									 mov  zvalue, edx
										 mov  eax, Pu01
										 mov  edx, u02
										 sub  eax, edx				
										 sar  eax, 16
									 	 imul eax, ebx			
										 mov  deltaU, eax
										 mov  eax, Pv01
										 mov  ecx, v02
										 sub  eax, ecx			
										 sar  eax, 16
										 imul eax, ebx	
										 mov  deltaV, eax
										pop  ebx
										neg  ebx
										push ebp
										mov  ebp, texPtr
										AL01:
										  push ecx

											push edx
											mov edx, zvalue
											cmp [esi + ebx*4], edx
											ja  AL03
											pop edx
											jmp  AL04
											AL03:
										    mov [esi + ebx*4], edx
										    pop edx

											  mov eax, edx
											  shr ecx, 9
											  and ecx, 3F80h
											  shr eax, 16
											  
											  and eax, 0x0000007F
											  or  cl, al
											  mov  al, [ebp + ecx + 8 + 512]
									or al,al
									jz AL04
											  mov  ax, [ebp + eax*2]
											  mov word ptr [edi + ebx*2], ax
											AL04:
  
											mov eax,zvalue
											add eax, deltaZ
											mov zvalue, eax
										  pop ecx
											add edx, deltaU
											add ecx, deltaV
											inc ebx
										js AL01
										jmp AX01

ARRR:		// RHS edge
									mov edx, eax	// x01
									mov ecx, ebx	// x02

									shr edx, 16
									shr ecx, 16

									lea edi, [edi + ecx*2]
									lea esi, [esi + ecx*4]
									sub ecx, edx

									pushad
										push ecx
										 mov  edx, SOFTdeltaTableP
										 mov  ebx, [edx + ecx*4 ]		
									 mov  edx, Pz01
									 mov  eax, z02
									 sub  eax, edx
									 sar  eax, 16
									 imul eax, ebx
									 mov  deltaZ, eax
									 mov  zvalue, edx
										 mov  edx, Pu01
										 mov  eax, u02
										 sub  eax, edx				
										 sar  eax, 16
									 	 imul eax, ebx			
										 mov  deltaU, eax
										 mov  ecx, Pv01
										 mov  eax, v02
										 sub  eax, ecx			
										 sar  eax, 16
										 imul eax, ebx	
										 mov  deltaV, eax
										pop  ebx
										neg  ebx
										push ebp
										mov  ebp, texPtr
										AR01:
										  push ecx
											 push edx
											 mov edx, zvalue
											 cmp [esi + ebx*4], edx
											 ja  AR03
											 pop edx
											 jmp  AR04
											 AR03:
										     mov [esi + ebx*4], edx
											 pop edx
											  mov eax, edx
  											  shr ecx, 9
											  and ecx, 3F80h
											  shr eax, 16

											  and eax, 0x0000007F
											  or  cl, al
											  mov  al, [ebp + ecx + 8 + 512]
						or al,al
						jz AR04
											  mov  ax, [ebp + eax*2]
											  mov word ptr [edi + ebx*2], ax
											AR04:
											mov eax,zvalue
											add eax, deltaZ
											mov zvalue, eax
										  pop ecx
											add edx, deltaU
											add ecx, deltaV
											inc ebx
										js AR01
									
AX01:
										pop ebp
									popad
//RRR:
								pop esi
								pop edi
								add eax, Pdelta01x
								add ebx, delta02x
								mov edx, Pdelta01u
								mov ecx, delta02u
								add Pu01, edx
								add u02, ecx
								mov edx, Pdelta01v
								mov ecx, delta02v
								add Pv01, edx
								add v02, ecx
								mov edx, Pdelta01z
								mov ecx, delta02z
								add Pz01, edx
								add z02, ecx
								add edi, pitch
								add esi, 1024*4
							pop ecx
							dec ecx
							jns AL00
							mov screenPtr, edi
							mov zbufferPtr, esi
							mov x02, ebx
							popad
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

// boris2->displayHlong, boris2->displayWlong	


	D3DTLVERTEX * c0, * c1, * c2, * tempC;
	long y0, y1, y2;
	long x0, x1, x2;
	long u0, u1, u2;
	long v0, v1, v2;
	float c0u, c1u, c2u;
	float c0v, c1v, c2v;

	pitch = boris2->videoPitch;		// bytes per raster scan line


//DBGR.Dprintf("********************************\n");

if(numVert)
	for( long ijk = 0; ijk < numVert; ijk+=3 )
	{
	   
		long tempL;
		float k;

//		if( SOFTcurrentMaterial == NULL )
//			DBGR.Dprintf("**** EGRG!!!\n");

		c0 = list++; c1 = list++; c2 = list++;


			// order the vertices in terms of ascending 2D y values

		 y0 = (long) c0->sy;
		 y1 = (long) c1->sy;
		 y2 = (long) c2->sy;

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



		 long y2y0 = y2-y0;

		 c0u = c0->tu ;
		 c1u = c1->tu ;
		 c2u = c2->tu ;

		 c0v = c0->tv ;
		 c1v = c1->tv ;
		 c2v = c2->tv ;



		 if( y2y0 )		// ignore single line triangles!!
		 {
			 //x0 = (long) c0->sx;
			 //x1 = (long) c1->sx;
			 //x2 = (long) c2->sx;

			 //z0 = (long) (c0->sz * 16);// * c0->rhw);
			 //z1 = (long) (c1->sz * 16);// * c1->rhw);
			 //z2 = (long) (c2->sz * 16);// * c2->rhw);

			// u0 = (long) (c0->tu * 127);
			// u1 = (long) (c1->tu * 127);
			// u2 = (long) (c2->tu * 127);

			// v0 = (long) (c0->tv * 127);
			// v1 = (long) (c1->tv * 127);
			// v2 = (long) (c2->tv * 127);


    		screenPtr = boris2->videoPtrActual + y0 * pitch;// starting ROW position (no x)
			zbufferPtr = &zbuffer[y0 * 1024];

			long texhandle    = SOFTcurrentMaterial->textureHandle;

			texPtr = SOFThandleToTexture[texhandle];			// if tex handle is 0 ?

			Py1y0 = y1-y0;

			k = SOFTrecip64table[ y2y0 ];
			QFASTFTOL( &delta02x , (( c2->sx - c0->sx ))  * k );
			QFASTFTOL( &delta02u , (( c2u - c0u )*128) * k);
			QFASTFTOL( &delta02v , (( c2v - c0v )*128) * k);
			QFASTFTOL( &delta02z , (( c2->sz - c0->sz )*16) * k);

			QFASTFTOL( &Px01, c0->sx * 65536 ); 
			QFASTFTOL( &Pu01, c0u * 65536*128 ); 
			QFASTFTOL( &Pv01, c0v * 65536*128 ); 
			QFASTFTOL( &Pz01, c0->sz * 65536*16 );

			x02 = Px01;
			u02 = Pu01;
			v02 = Pv01;
			z02 = Pz01;

			if( Py1y0 )		
			{
				k = SOFTrecip64table[ Py1y0 ];
				QFASTFTOL( &Pdelta01x , (( c1->sx - c0->sx ))     * k);
				QFASTFTOL( &Pdelta01u , (( c1u - c0u )*128) * k);
				QFASTFTOL( &Pdelta01v , (( c1v - c0v )*128) * k);
				QFASTFTOL( &Pdelta01z , (( c1->sz - c0->sz )*16)  * k);
			};
			

			SOFTdeltaTableP = SOFTdeltaTable;

	if( (texhandle!=0) && (texPtr !=0) )
	{
		
			//
			// A Texture exists.... texPtr +8       points at its palette
			//	texPtr +8+512   points at its data

		if(SOFTcurrentMaterial->flags & MAT_ALPHA )
		{
			
				renderTriangleAlpha();
				
				Py1y0 = y2-y1;

				if(Py1y0)
				{
					k = SOFTrecip64table[ Py1y0 ];
					
					QFASTFTOL( &Px01, c1->sx * 65536 );
					QFASTFTOL( &Pu01, c1u * 65536*128 );
					QFASTFTOL( &Pv01, c1v * 65536*128 );
					QFASTFTOL( &Pz01, c1->sz * 65536*16 );
					QFASTFTOL( &Pdelta01u, (( c2u - c1u )*128)  * k );
					QFASTFTOL( &Pdelta01v, (( c2v - c1v )*128)  * k );
					QFASTFTOL( &Pdelta01x, (( c2->sx - c1->sx ))  * k );
					QFASTFTOL( &Pdelta01z, (( c2->sz - c1->sz )*16) * k );
					renderTriangleAlpha();

				};


		} else			// NOT ALPHA
		{
				renderTriangleNonAlpha();
				
				Py1y0 = y2-y1;

				if(Py1y0)
				{

					k = SOFTrecip64table[ Py1y0 ];
					
					QFASTFTOL( &Px01, c1->sx * 65536 );
					QFASTFTOL( &Pu01, c1u * 65536*128);
					QFASTFTOL( &Pv01, c1v * 65536*128);
					QFASTFTOL( &Pz01, c1->sz * 65536*16);
					QFASTFTOL( &Pdelta01u, ( c2u - c1u )*128 * k);
					QFASTFTOL( &Pdelta01v, ( c2v - c1v )*128 * k);
					QFASTFTOL( &Pdelta01x, ( c2->sx - c1->sx )* k);
					QFASTFTOL( &Pdelta01z, ( c2->sz - c1->sz )*16 * k);
					renderTriangleNonAlpha();

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
		bitmapmemory = (unsigned char *)malloc( 4 + 4 + 512 + bmih.biWidth * hhh);

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
	for( long i = 1024*768;i--;)
		*zptr++=0x07FFFFFFF;

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
