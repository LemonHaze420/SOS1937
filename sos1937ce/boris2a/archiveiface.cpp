//================================================================================================================================
// archive.cpp
// -----------
//
//================================================================================================================================

#include "BS2all.h"

#define MAXHANDLES 128

class handledata 
{
public:

	char * area;
	ulong  size;
	ulong  nextByte;
	bool used;
};

handledata handles[ MAXHANDLES ];


//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------


void initialiseArcHandles( void )
{
	handledata * hptr = handles;
	long i;
	for( i = MAXHANDLES; i--; )
		(hptr++)->used = false;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong findFreeHandle( char * area, ulong size)
{

	handledata * hptr = &handles[1];
	long i;

	for( i = 1; i < MAXHANDLES; i++ )
	{
		if( hptr->used == false)
		{
			hptr->used = true;
			hptr->area = area;
			hptr->size = size;
			hptr->nextByte = 0;
			return( i );
		} 
		hptr++;
	}
	fatalError("No free archive handles");
	return(0);
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

bool arcPush( char * archiveName )
{
	if(!archivestack.Push(archiveName))
		fatalError("cannot push archive %s",archiveName);
	return true;
}


void arcPopAll()
{
	while(archivestack.stackct)archivestack.Pop();
}


//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong  arcExtract_3df(   char * fileName )
{
	
	if(!fileName)
		fatalError("attempted to load null file");
	//
	// We shall attempt to extract the appropriate archive texture in 
	// the most appropriate format.
	//
	
	ulong areasize;
	char *area;
	
	if( (area = archivestack.MallocAndExtract( fileName, FMT_3DF_565, &areasize	)) == NULL)
		if( (area = archivestack.MallocAndExtract( fileName, FMT_3DF_1555, &areasize )) == NULL)
			if( (area = archivestack.MallocAndExtract( fileName, FMT_3DF_4444, &areasize )) == NULL)
				if( (area = archivestack.MallocAndExtract( fileName, FMT_3DF_8BIT, &areasize )) == NULL)
					return(0); //fatalError("Failed to load 3df file from archive");
				
	return( findFreeHandle( area, areasize ) );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong  arcExtract_txt(   char * fileName)
{
	ulong areasize;
	char *area = archivestack.MallocAndExtract( fileName, FMT_TXT_PARAM, &areasize);

	if(area == NULL)
		return(0);

	return( findFreeHandle( area, areasize  ) );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong  arcExtract_mdf(   char * fileName)
{
	ulong areasize;
	char *area = archivestack.MallocAndExtract( fileName, FMT_TXT_MODDESC, &areasize);

	if(area == NULL)
		return(0);

	return( findFreeHandle( area, areasize  ) );
}
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong  arcExtract_rlf(   char * fileName)
{
	ulong areasize;
	char *area = archivestack.MallocAndExtract( fileName, FMT_RLINE, &areasize);

	if(area == NULL)
		return(0);

	return( findFreeHandle( area, areasize  ) );
}
//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

ulong  arcExtract_mul(   char * fileName)
{
	ulong areasize;
	char *area = archivestack.MallocAndExtract( fileName, FMT_VO_OBJ, &areasize);

	if(area == NULL)
		return(0);

	return( findFreeHandle( area, areasize  ) );
}

ulong  arcExtract_wav(   char * fileName)
{
	ulong areasize;
	char *area = archivestack.MallocAndExtract( fileName, FMT_WAV, &areasize);

	if(area == NULL)
		return(0);

	return( findFreeHandle( area, areasize  ) );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

char * arcGetData( ulong handle )
{
	return ( handles[handle].area );
}

long arcGetSize( ulong handle )
{
	return ( handles[handle].size );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void  arcRead( char * outStr, ulong count, ulong handle )
{
	handledata * hptr = &handles[ handle ];

	ulong i;
	char * optr = outStr;
	char * iptr = hptr->area + hptr->nextByte;
	for(i = count; i--; )
		*optr++ = *iptr++;

	hptr->nextByte += count;
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

bool  arcGets( char * outStr, ulong count, ulong handle )
{
	handledata * hptr = &handles[ handle ];
	long bytesLeft = hptr->size - hptr->nextByte;
   
	if( bytesLeft <= 0 )
		return false;				// eof reached!!

	char * iPtr = hptr->area + hptr->nextByte;
	char c = 0;
	char * oPtr = outStr;
	long oldbytesLeft = bytesLeft;

	while ( ( bytesLeft > 0 ) && ( c!=13) )
	{
		c = *iPtr++;
		bytesLeft--;
		switch( c )
		{
			case 10:
			case 13:
				break;
			default:
				*oPtr++ = c;
		}
	}

	*oPtr = 0;
	hptr->nextByte += oldbytesLeft - bytesLeft;

	return( true );
}

bool  arcGets_NoWhiteSpace( char * outStr, ulong count, ulong handle )
{
	handledata * hptr = &handles[ handle ];
	long bytesLeft = hptr->size - hptr->nextByte;

	if( bytesLeft <= 0 )
		return false;				// eof reached!!

	char * iPtr = hptr->area + hptr->nextByte;
	char c = 0;
	char * oPtr = outStr;
	long oldbytesLeft = bytesLeft;

	bool somethingHere = false;
	while ( ( bytesLeft > 0 ) && ( c!=13) )
	{
		c = *iPtr++;
		bytesLeft--;
		switch( c )
		{
			case 10:
			case 13:
			case 32:
			case 9:
				break;

			default:
				*oPtr++ = c;
				somethingHere = true;
				break;
		}
	}

	*oPtr = 0;
	hptr->nextByte += oldbytesLeft - bytesLeft;

	return( somethingHere );
}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------

void  arcDispose( ulong handle )
{
	handledata * hptr = &handles[ handle ];

	hptr->used = false;
	free( hptr->area );
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
