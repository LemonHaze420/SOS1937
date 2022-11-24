// $Header$

// $Log$
// Revision 1.3  2000-01-28 11:59:14+00  img
// <>
//
// Revision 1.2  2000-01-27 16:39:11+00  jjs
// Allow textures to be delted on mesh deletion.
//
// Revision 1.1  2000-01-13 17:27:07+00  jjs
// First version that supports the loading of objects.
//

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

textureData * getTexturePtr( char * filename, bool environmentMap, bool uniqueTextures, bool Bump  )
{
	ulong texhandle;
	bool matched = false;
	textureData * myTexture = textureChain;
	int offset=0;

	if(uniqueTextures)
	{
		sprintf(outfilename,"%d%s",boris2->uniqueTid++,filename);	
		offset = strlen(outfilename) - strlen(filename);
	}
	else
	{
		strcpy(outfilename, filename );	
		offset=0;
	}

	while( myTexture != 0)
	{
		if( matched = ( strcmp(myTexture->filename, outfilename) == 0) )		// matched
		   break;
		myTexture = myTexture->next;
	}

	if (matched)
	{
		myTexture->usageCount++;
		return( myTexture );
	}
	else
	{
		
		textureData * oldtc = textureChain;

		textureChain = new textureData;

		textureChain->next = oldtc;
		textureChain->tindex = lasttindex;
		
		textureChain->filename = new char[ strlen(outfilename) + 1];

		strcpy( textureChain->filename, outfilename );
		textureChain->origfilename = textureChain->filename+offset;


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
			{
//				if(Bump)
//					textureChain->textureHandle = registerBump( arcGetData( texhandle ));
//				else
				textureChain->textureHandle = registerTexture( arcGetData( texhandle ), 1 );
			}
			else
				textureChain->textureHandle = registerTexture( arcGetData( texhandle )  );
		}
		else
			textureChain->textureHandle = 0;

		lasttindex++;

		arcDispose( texhandle );	

		textureChain->usageCount = 1;

		return( textureChain );
	}
}

void removeTexture(textureData* tex)
{
	textureData * myTextureNew=NULL;

	if(--tex->usageCount)
		return;				// Still in use by someone

	textureData * myTexture = textureChain;
	textureData * unlink = NULL;

	while( myTexture != 0 )
	{
		if(myTexture == tex)
		{
			unregisterTexture( myTexture->textureHandle );
			delete myTexture->filename;
			myTextureNew = myTexture->next;
			delete myTexture;
			if(unlink)
				unlink->next = myTextureNew;
			else
				textureChain = myTextureNew;
			break;
		}
		unlink = myTexture;
		myTexture = myTexture->next;
	}
}

void removeTextureStore()
{
	textureData * myTextureNew;
	textureData * myTexture = textureChain;
	while( myTexture != 0 )
	{
		if(myTexture->textureHandle)
			unregisterTexture( myTexture->textureHandle );
		delete myTexture->filename;
		myTextureNew = myTexture->next;
		delete myTexture;
		myTexture = myTextureNew;
	}
	textureChain = NULL;
}
