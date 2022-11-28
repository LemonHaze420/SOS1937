//================================================================================================================================
// textureStore.cpp
// ----------------
//
//================================================================================================================================


#include "BS2all.h"

#include <stdio.h>




textureData * textureChain;
ulong lasttindex;

void startTextureStore()
{
	textureChain = NULL;
	lasttindex = 0;
}

//
// if texture is loaded (search for filename) then just return the index
// otherwise load the texture and then return the new index.
//


extern char tempBuffer[];


char outfilename[256];
char newfilename[256];

textureData * getTexturePtr( char * filename, bool environmentMap, bool uniqueTextures  )
{
	ulong texhandle;
	bool matched = false;
	textureData * myTexture = textureChain;

	if(uniqueTextures)
	{
		sprintf(outfilename,"%d%s",boris2->uniqueTid++,filename);	
	}
	else
		strcpy(outfilename, filename );	

	while( myTexture != 0)
	{
		if( matched = ( strcmp(myTexture->filename, outfilename) == 0) )		// matched
		   break;
		myTexture = myTexture->next;
	}

	if (matched)
		return( myTexture );
	else
	{
		
		textureData * oldtc = textureChain;

		textureChain = new textureData;

		textureChain->next = oldtc;
		textureChain->tindex = lasttindex;
		
		textureChain->filename = new char[ strlen(outfilename) + 1];

		strcpy( textureChain->filename, outfilename );


		// Remove '.3df'  ('.bmp' if software texture mapping in use...)
		strcpy(newfilename, filename );	
		stripfilesuffix( newfilename );


		texhandle = arcExtract_3df( newfilename );


		//
		// set any flags from the texture filename....
		//

		char * fchar = newfilename;
		char c;

		textureChain->flags = 0;
		while( (*fchar != 0) )
		{
			if( (*fchar) == '_' )
			{
				fchar++;
				c = *fchar;
				if( c != 0 )
					if( c != 'a' )
							textureChain->flags |= MAT_ALPHA;
				break;
			}
			fchar++;
		}

		if( texhandle )
		{
			if( environmentMap )
				textureChain->textureHandle = registerTexture( arcGetData( texhandle ), 1 );
			else
				textureChain->textureHandle = registerTexture( arcGetData( texhandle )  );
		}
		else
			textureChain->textureHandle = 0;

		lasttindex++;

		arcDispose( texhandle );	

		return( textureChain );
	};

}


void removeTextureStore()
{
	textureData * myTextureNew;
	textureData * myTexture = textureChain;
	while( myTexture != 0 )
	{
		if(myTexture->textureHandle)
		{
			unregisterTexture( myTexture->textureHandle );
		}
		delete myTexture->filename;
		myTextureNew = myTexture->next;
		delete myTexture;
		myTexture = myTextureNew;
	};
	textureChain = NULL;
}


//================================================================================================================================
//END OF FILE
//================================================================================================================================
