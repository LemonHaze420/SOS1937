// $Header$

// $Log$
// Revision 1.3  2000-05-03 09:36:16+01  jjs
// uses malloc32 and aligned reads.
//
// Revision 1.2  2000-05-01 17:13:48+01  jjs
// Uses malloc32 for loading speedup
//
// Revision 1.1  2000-03-06 12:48:58+00  jjs
// Modified for Dreamcast.
//
// Revision 1.0  1999-12-07 11:48:44+00  jjs
// Initial revision
//

#include "bs2all.h"

BroadArchiveStack archivestack;

BroadArchiveStack::BroadArchiveStack()
{
	stackct=0;
}

BroadArchiveStack::~BroadArchiveStack()
{
	;
}

unsigned long _inline readlong(char *longptr)
{
	unsigned long t;
	char *pt = (char *)&t;
	*pt++ = *longptr++;
	*pt++ = *longptr++;
	*pt++ = *longptr++;
	*pt = *longptr;

	return t;
}

char *BroadArchiveStack::MallocAndExtract(char *name, unsigned long id,unsigned long *size)
{
	int i=stackct;
	struct tarc_index *ti;
	char *data;

	do
	{
		if(ti = stack[--i].Stat(name,id))
		{
			long sz = readlong((char *)&ti->size);

			data = (char *)malloc32((sz+32) & ~31L);
			if(data)
			{
				if(size)*size = sz;
				stack[i].Extract(name,id,data);
			}
			return data;
		}
	} while(i);

	return NULL;
}



unsigned long BroadArchiveStack::ExtractIntoArea(char *name,unsigned long id,char *area,int asize)
{
	int i=stackct;
	struct tarc_index *ti;

	do
	{
		if(ti = stack[--i].Stat(name,id))
		{
			long sz = readlong((char *)&ti->size);
			if(sz > asize)return 0xffffffff;
					
			stack[i].Extract(name,id,area);
			return sz;
		}
	} while(i);
	return 0;
}

bool BroadArchiveStack::Push(char *name)
{
	if(stackct==STACKSIZE)
		return false;
	if(stack[stackct].Open(name))
	{
		stackct++;
		return true;
	}
	return false;
}

void BroadArchiveStack::Pop()
{
	if(stackct)
		stack[--stackct].Close();
}


