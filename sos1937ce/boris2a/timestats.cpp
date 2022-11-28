//================================================================================================================================
// timeStats.cpp
// -------------
//
//================================================================================================================================

#include "BS2all.h"


timeStatsElement stats[32];
ulong entries = 0;

extern __int64 timing_frequency;
extern D3DTLVERTEX  vertexListD3D[];
extern engine * boris2;

extern font * testFont;		

void endTimingStats( void )
{
	ulong i;
	float ypos, xpos;
	ulong proportion;
	timeStatsElement * tsePtr;
	BorisMaterial bmTest;

	__int64 sum ;

	entries++;				

	tsePtr = &stats[0];
	sum = 0;
	for(i = entries; i--; )
	{
		sum += tsePtr->totalTicks;
		tsePtr++;
	};
	if(sum)
	{
		sum = (1 << 16) / sum;			//  n/sum  =  ( n * ( 65536/sum) ) >> 16	 (16 bit acccy)

		xpos = 0;
		ypos = 20;
		tsePtr = &stats[0];

		ulong total = 0;
		D3DTLVERTEX * testVertex = vertexListD3D;
		long thiscolor;

		for(i = entries; i--; )
		{

			proportion = (ulong)(( tsePtr->totalTicks * sum ) >> 8);	// >> 8 means the numerator is in range 0..255
																		// >> 16 would give '0' every time sine it is the actual proportion

			thiscolor = ((tsePtr->r ) << 16 ) + ((tsePtr->g)<<8) + tsePtr->b;

			testVertex->sx = xpos; testVertex->sy = ypos; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;
			testVertex->sx = xpos + (float)proportion; testVertex->sy = ypos; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;
			testVertex->sx = xpos; testVertex->sy = ypos + 12; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;

			testVertex->sx = xpos + (float)proportion; testVertex->sy = ypos; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;
			testVertex->sx = xpos + (float)proportion; testVertex->sy = ypos + 12; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;
			testVertex->sx = xpos; testVertex->sy = ypos + 12; testVertex->sz = 0.0f;
			(testVertex++)->color = thiscolor;

			ypos += 16;
			tsePtr++;
			total += 6;
		}

		bmTest.textureHandle = 0;

		renderSetCurrentMaterial( &bmTest );
		renderList( &vertexListD3D[0], total);
	}
	entries = 0;


}

DWORD fonthandle;

void startStat( ulong idx, ulong r, ulong g, ulong b ) 
{	
	__int64 t;
    QueryPerformanceCounter((LARGE_INTEGER *)&t);

	stats[idx].startTime = t;
	stats[idx].r = r;
	stats[idx].g = g;
	stats[idx].b = b;
	if (idx > entries) entries = idx;		// set for max. entry
};

void endStat( ulong idx )
{
	__int64 t;
    QueryPerformanceCounter((LARGE_INTEGER *)&t);

	stats[idx].totalTicks = t - stats[idx].startTime;
};


__int64 framesToSampleTimesFreq;
__int64 frameTickCount[256];
__int64 * previousFrame, * currentFrame, * oneBeyondEnd;

void initialiseFrameRateCounter( ulong sampleframes )
{
	__int64 pcount;
	__int64 * fptr;

	framesToSampleTimesFreq = sampleframes *  timing_frequency;

	previousFrame  = frameTickCount;
	currentFrame   = frameTickCount + (sampleframes-1);
	oneBeyondEnd   = frameTickCount + 256;

	fptr = previousFrame;
    QueryPerformanceCounter((LARGE_INTEGER *)&pcount);
	do {
		*(fptr++) = pcount;
	} while(fptr != currentFrame);

	fonthandle = (getTexturePtr( "smallfont.3df", false ))->textureHandle;

}

void acknowledgeFrame( void )
{
	currentFrame++;
	if(currentFrame == oneBeyondEnd)
	{
		currentFrame = frameTickCount;
	};
	previousFrame++;
	if(previousFrame == oneBeyondEnd)
	{
		previousFrame = frameTickCount;
	};
    QueryPerformanceCounter((LARGE_INTEGER *)currentFrame);

}


extern char tempBuffer[];
ulong polygoncounter;


extern bool collisionflag;

void displayFrameRate( ulong gNum, ulong rNum, float vNum )
{
	static rTriangle rTri;

	slong framesPerSec = (ulong)(framesToSampleTimesFreq / ((*currentFrame) - (*previousFrame)));

	sprintf(tempBuffer,"frames: %d",framesPerSec);

	testFont->RenderString(8, 5, tempBuffer);

	if(1)
	{
		// Display the gear of the test car

		sprintf(tempBuffer,"gear: %d", gNum);
		testFont->RenderString(8, 30, tempBuffer);


		// Display the revs of the test car

		sprintf(tempBuffer,"revs: %d", rNum);
		testFont->RenderString(8, 60, tempBuffer);

			// Display the velocity of the test car


		vNum *= 90 * 2.237f;
		if ((fabs(vNum)) < 0.1f) vNum = 0.0f;

		sprintf(tempBuffer,"speed [mph] : %.1f", vNum);
		testFont->RenderString(8, 90, tempBuffer);
	}
}




//================================================================================================================================
// END OF FILE
//================================================================================================================================
