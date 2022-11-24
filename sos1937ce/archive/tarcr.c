// $Header$

// $Log$
// Revision 1.9  2000-05-31 17:19:23+01  jjs
// Has a horrible hack for GridModels.tc.
//
// Revision 1.8  2000-05-10 18:18:08+01  jcf
// Reset checking
//
// Revision 1.7  2000-05-04 16:25:46+01  jcf
// Archive caching
//
// Revision 1.6  2000-05-03 09:36:17+01  jjs
// uses malloc32 and aligned reads.
//
// Revision 1.5  2000-04-28 11:09:08+01  jcf
// <>
//
// Revision 1.4  2000-04-21 19:01:48+01  jcf
// <>
//
// Revision 1.3  2000-04-13 15:23:30+01  jjs
// Adds SSB support.
//
// Revision 1.2  2000-02-07 12:20:38+00  jjs
// Added WIN98/CE compatibility.
//
// Revision 1.1  2000-01-13 17:26:14+00  jjs
// Modified for Dreamcast loading.
//
// Revision 1.0  1999-12-07 11:48:49+00  jjs
// Initial revision
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "tarc.h"

#if defined(UNDER_CE)
#define islower(x) ((x)>='a' && (x)<='z')
#endif

void ReadDiskError(char *,int);

static BOOL always_cache = FALSE;

BOOL TARC_SetCacheMode(BOOL cache)
{
	BOOL q = always_cache;
	always_cache = cache;
	return q;
}

#if defined(UNDER_CE)
BOOL firmware_reset = FALSE;
void TARC_SetFirmwareReset(BOOL f)
{
	firmware_reset = f;
}

void DoFirmwareResetTest();

void FirmwareResetTest()
{
	if(firmware_reset)DoFirmwareResetTest();
}

#else

void TARC_SetFirmwareReset(BOOL f)
{
	;
}
void FirmwareResetTest()
{
	;
}

#endif




/****Thu Dec 11 16:52:02 1997*************(C)*Broadsword*Interactive****
 *
 * Read the index block of a tarc. Returns 100+ if not ok, otherwise 0
 *
 ***********************************************************************/

int TARC_ReadHeaderAndIndex(struct tarc *t)
{
	struct tarc_index *ti;
	DWORD bytesRead;
	int i;
	char temp[100];

	FirmwareResetTest();
	if(t->block)
	{
		memcpy(&(t->hd),t->block,sizeof(struct tarc_header));
	}
	else
	{
		for(i=0;i<10;i++)
		{
			if(SetFilePointer(t->f,0,NULL,FILE_BEGIN)!=0xffffffff)
				if(ReadFile(t->f,&(t->hd),sizeof(struct tarc_header),&bytesRead,NULL))
					break;

			OutputDebugString(TEXT("read error\n"));

			Sleep(100);
		}
		if(i==10)ReadDiskError(t->fn,1);
	}


	memcpy(temp,&t->hd,sizeof(struct tarc_header));
	TARC_Crypt((char *)&(t->hd),sizeof(struct tarc_header),KeyHeader);
	
	if(t->hd.magic != TARC_MAGIC)
		return 100;	// not a tarc
	
	// allocate the index, or just set up a pointer if this is a cached tarc

	if(t->block)
	{
		t->i = t->block + t->hd.index;	// DO NOT FREE!!
		TARC_Crypt((char *)t->i,sizeof(struct tarc_index)*t->hd.filect,
				KeyIndex);
	}
	else
	{
		if(t->hd.filect)
		{	
	 		ti=(struct tarc_index *)
				malloc(sizeof(struct tarc_index)*t->hd.filect);

			// and read it in

			for(i=0;i<10;i++)
			{
				if(SetFilePointer(t->f,t->hd.index,NULL,FILE_BEGIN) != 0xffffffff)
					if(ReadFile(t->f,ti,t->hd.filect*sizeof(struct tarc_index),&bytesRead,NULL))
						break;

				OutputDebugString("read error (2)\n");

			}

			if(i==10)ReadDiskError(t->fn,2);

			TARC_Crypt((char *)ti,sizeof(struct tarc_index)*t->hd.filect,
				KeyIndex);

			t->i = ti;
		}
		else t->i=NULL;
	}
	return 0;
} 
 	



/****Thu Dec 11 16:49:58 1997*************(C)*Broadsword*Interactive****
 *
 * Open a tarc for reading and read the index block.
 *
 ***********************************************************************/

int TARC_OpenR(struct tarc *t,char *fname)
{
	HANDLE f;
	int rv,i;
#if defined(UNDER_CE)
	short buf[100];
#else
	char buf[256];
#endif
	BOOL cache;	// do we cache or not?

	if(*fname == '*')
	{
		fname++;
		cache = TRUE;
	}
	else cache = FALSE;

	for(i=0;i<strlen(fname)+1;++i)
		buf[i]=fname[i];

 	strcpy(t->fn,fname);

	for(i=0;i<10;i++)
	{
		f=CreateFile(buf,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if(f != INVALID_HANDLE_VALUE)break;

		OutputDebugString("open error\n");

		Sleep(100);
	}
	if(i==10)ReadDiskError(t->fn,3);


try_again:;
	t->block = NULL;

	if(cache || always_cache)
	{
		DWORD size,bytesread;
		size=GetFileSize(f,NULL);
	
		t->f = NULL;
		if(!(t->block = (char *)malloc(size)))
			OutputDebugString(TEXT("cannot allocate memory for file image\n"));
		else
			ReadFile(f,t->block,size,&bytesread,NULL);
	}

	if(!t->block)
	{
		t->f = f;
	}

	if(rv= TARC_ReadHeaderAndIndex(t))
	{
		if(rv == 100)
		{
			TARC_CloseR(t);
			f=CreateFile(TEXT("\\cd-rom\\sos1937FrontEnd\\gridmodels2.tc"),GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			goto try_again;
		}
		return rv;
	}
	t->writable=0;
	return 0;
}

/****Fri Dec 12 10:41:43 1997*************(C)*Broadsword*Interactive****
 *
 * Used to determine whether an index entry matches a format/name pair.
 * Returns nz if it does.
 *
 ***********************************************************************/

int TARC_EntryMatches(struct tarc_index *i,unsigned long id,char *name,short version)
{
	unsigned char flags,tversion,tid;

	if(name)
	{
		int t1,t2,j;
		char c1,c2;

		t1 = strlen(name);
		t2 = strlen(i->tname);
		if(t1 == t2)
		{
			for(j = 0 ; j < t1 ; ++j)
			{
				c1=name[j];
				c2=i->tname[j];
				if(islower(c1))
					c1 -= ('a' - 'A');
				if(islower(c2))
					c2 -= ('a' - 'A');
				if(c1 != c2)
					return 0;
			}
		}
		else
			return 0;
	}

	flags = *(unsigned char *)&(i->flags);
	tversion = *(unsigned char *)&(i->version);

	if((version!= TARCV_DEL) && (flags & TARCF_DELETED))return 0;

	if((version == TARCV_DEL) && !(flags & TARCF_DELETED))
		return 0;
	else if((version == TARCV_LATEST) && (flags & TARCF_SUPERCEDED))
		return 0;
	else if((version == TARCV_OLD) && !(flags & TARCF_SUPERCEDED))
		return 0;
	else if((version >= 0) && (tversion!=version))
		return 0;


	{
		unsigned char *b = (unsigned char *)&(i->id);

		if(	(b[0] != (id&0xff)) || 
			(b[1] != ((id>>8) & 0xff)) || 
			(b[2] != ((id>>16) & 0xff)) || 
			(b[3] != ((id>>24) & 0xff)))
		{
			return 0;
		}
	}

/*
#define CHECKBITS(bits)	( (id & (bits)) && ((id & (bits)) != (i->id & (bits))))

	if(CHECKBITS(0xff))return 0;
	if(CHECKBITS(0xff00))return 0;
	if(CHECKBITS(0xff0000))return 0;
	if(CHECKBITS(0xff000000))return 0;
*/
	return 1;
}

/****Mon Dec 15 14:35:20 1997*************(C)*Broadsword*Interactive****
 *
 * Close a tarc
 *
 ***********************************************************************/

void TARC_CloseR(struct tarc *t)
{
	if(t->block)
	{
		free(t->block);
		t->i = NULL;
		t->block = NULL;
	}
	else
	{
		CloseHandle(t->f);
		if(t->i)
		{
			free(t->i);t->i = NULL;
		}
	}
}

struct tarc_index *TARC_GetIndexData(struct tarc *t,char *name,unsigned long id,
	short version)
{
	int i;
	struct tarc_index *ti;
	
	ti = t->i;
	
	for(i=0;i<t->hd.filect;i++)
	{
		if(i==0x56)
		{
			int r;
			r=4;
		}

		if(TARC_EntryMatches(ti,id,name,version))
		{
			return ti;
		}
		ti++;
	}
	return NULL;
}


 
/****Thu Dec 11 16:50:33 1997*************(C)*Broadsword*Interactive****
 *
 * Extract a file from a tarc into a given memory block. This extracts
 * the first block with the same tname which matches the format.
 * It returns the size of the block, or 0 if nothing was found. If the
 * buffer pointer is NULL, it does no reading but still returns the size.
 *
 ***********************************************************************/
 
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

unsigned long TARC_Extract(struct tarc *t,char *name,unsigned long id,
	short version,char *buf)
{
	struct tarc_index *ti;
	
	ti = TARC_GetIndexData(t,name,id,version);
	
	if(ti && buf)
	{
		TARC_ExtractEntry(t,ti,buf);
		return readlong((char *)&ti->size);
	}
	else return 0;
}




/****Fri Dec 12 14:09:19 1997*************(C)*Broadsword*Interactive****
 *
 * This does the donkey work for the extraction.
 *
 ***********************************************************************/

void TARC_ExtractEntry(struct tarc *t,struct tarc_index *ti,void *buf)
{
	DWORD bytesRead;
	int i;
	int realsize = readlong((char *)&ti->size);

	int size = (realsize + 32) & ~31L;

	FirmwareResetTest();
	if(t->block)
	{
		unsigned long idx = readlong((char *)&ti->index);
		memcpy(buf,t->block+idx,size);
	}
	else
	{
		for(i=0;i<10;i++)
		{
			if(SetFilePointer(t->f,ti->index,NULL,FILE_BEGIN) != 0xffffffff)
				if(ReadFile(t->f,buf,size,&bytesRead,NULL))break;

			OutputDebugString("read error (Extract)\n");
		}
		if(i==10)ReadDiskError(t->fn,4);
	}

	TARC_Crypt(buf,realsize,KeyBody);
}

 

/****Mon Dec 15 12:39:26 1997*************(C)*Broadsword*Interactive****
 *
 * Really really really dumb encryption
 *
 ***********************************************************************/

char *KeyHeader="H12f4J4o^5$Ep8&a^7kl%5I4$n$3œnIS3e[2]Z1A;b'e[7]t'7;h8";

char *KeyBody=
"sjdklfjskldjfweortu2304048n02384v90285pnerugeiyn237 4b97897(&(&œ(*$&("
"23905b02n840yyyyyyyyy2034v20b8!9089034b8230480IOUIOSUDhqjw4289742byAsidyaify"
"09e8rb02802n3v5902808)(*s90e8naviosasfuwniorub034b082904ap][]35[24[5]2452-40"
"sledgehammersledgehammeraardvarksledgehammerfish2304820384v20389090)(*)*()*3"
"loosefishbadeggsadlumpscapegraceskldfjlksdjfw2*(&(*&W(*&(œ$&(œ*$&(*&!(((!&&e"
"23io58b29050238nv28903800*n(Nd)*)(w*v02380428304280368349068b306830498304n68";

char *KeyIndex=
"lqoaks'))!(****$90128390jdhfksksjdhfJH&%*$(%$&*%*%*$&%("
"klsd89090sd8reckonanyonewillguesskasjd3-230-3239490-490";

void TARC_Crypt(char *buf,long size,char *key)
{
	int i,q;

	q=strlen(key);
	for(i=0;i<size;i++)
	{
		// I really really really really hate to do this.
		// I am *quite* aware that this is the world's worst
		// encryption algorithm, thank you very much. Since
		// we're only trying to dissuade casual browsers,
		// I'm not too bothered.
		
		buf[i] ^= key[i%q];
	}
}

