//================================================================================================================================
// archiveIface.h
// --------------
//
//================================================================================================================================

#ifndef _BS_archiveIface
#define _BS_archiveIface

void  initialiseArcHandles( void );
bool  arcPush( char * archiveName );
inline void  arcPop( void )
{
	archivestack.Pop();
}

void arcPopAll();

ulong arcExtract_bmp(   char * fileName );
ulong arcExtract_3df(   char * fileName );
ulong arcExtract_mul(   char * fileName );
ulong arcExtract_txt(   char * fileName);
ulong arcExtract_rlf(   char * fileName);
ulong arcExtract_mdf(   char * fileName);
ulong arcExtract_wav(   char * fileName);
bool  arcGets( char * outStr, ulong count, ulong handle );
void  arcDispose( ulong handle );
char * arcGetData( ulong handle );
long arcGetSize( ulong handle );
void  arcRead( char * outStr, ulong count, ulong handle );
bool  arcGets_NoWhiteSpace( char * outStr, ulong count, ulong handle );


#endif	// _BS_archiveIface

//================================================================================================================================
//END OF FILE
//================================================================================================================================
