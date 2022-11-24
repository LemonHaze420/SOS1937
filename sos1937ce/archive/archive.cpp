// $Header$

// $Log$
// Revision 1.0  1999-12-07 11:48:43+00  jjs
// Initial revision
//

#include "bs2all.h"

BroadArchive::BroadArchive()
{
	t = new tarc;
	isopen=false;
}

BroadArchive::~BroadArchive()
{
	if(isopen)Close();
	delete t;
}


void BroadArchive::Close()
{
	if(isopen)
	{
		TARC_CloseR(t);
		isopen=false;
	}
}

bool BroadArchive::Open(char *filename)
{
	if(TARC_OpenR(t,filename))
		return false;
	else
	{
		isopen=true;
		return true;
	}
}

unsigned long BroadArchive::Extract(char *name,unsigned long id,char *buffer)
{
	if(!isopen)return false;
	size=TARC_Extract(t,name,id,TARCV_LATEST,buffer);


	return size;
}

struct tarc_index *BroadArchive::Stat(char *name,unsigned long id)
{
	if(!isopen)return NULL;

	return TARC_GetIndexData(t,name,id,TARCV_LATEST);
}
