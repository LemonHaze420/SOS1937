// $Header$

// $Log$

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>

#include "tarcf.h"
#include "archive.h"
#include "arcstack.h"
#include "param.h"

#define	MAX_PFILE_SIZE	10000

#define isalnum(x) (((x)>='0' && (x)<='9') || ((x)>='A' && (x)<='Z') || ((x)>='a' && (x)<='z'))

BroadParamFile::BroadParamFile()
{
	num=0;curoff=-1;lastlen=-1;
	data=NULL;keys=NULL;vals=NULL;offset=0;
}

BroadParamFile::~BroadParamFile()
{
	if(data)
	{
		free(data);data=NULL;
		delete [] keys;
		delete [] vals;
	}
}


bool BroadParamFile::Read(char *name)
{
	char *p,*q;
	char *curdata;
	unsigned long off=0,size,stringsize=0L,i;
	int s;
	char *area=(char *)_alloca(MAX_PFILE_SIZE)	;

	// clear out any old data
	
	if(data)
	{
		free(data);data=NULL;
		delete [] keys;
		delete [] vals;
	}
	
	num = 0;

	// attempt to read the file into archivestack data area 1
	// return false on failure

	if(!(size=archivestack.ExtractIntoArea(name,FMT_TXT_PARAM,area,MAX_PFILE_SIZE)))
		return false;
	
	// get a pointer to the data
	
	p = area;
	
	// preprocess the data by replacing crlf pairs with NULL.
	
	for(off=0;off<size;off++)if(p[off]==0x0d || p[off]==0x0a)p[off]=0;
		
		
	// now go through the data, just counting items
	// first.
	
	for(off=0;off<size;off++)
	{
		// at this point we are pointing to a line.. if it
		// starts with # it's a comment
		
		if(p[off] != '#' && p[off])
		{
			// if it doesn't have a colon, ignore it.
			
			if(q=strchr(p+off,':'))
			{
				// aha.. an item..
				num++;
				
				// work out the size of the key and value
				
				s = off;
				
				// find the key's end
				while(p[s]!=':' && p[s]!=' ' && p[s]!='\t')s++;
				stringsize+= (s-off)+1;
				
				q++;s=0;
				while(*q==' ' || *q=='\t')q++; // find val start
				if(*q=='\"') // quoted string
				{
					q++;
					while(q[s] && q[s]!='\"')s++;q[s--]=0;
				}
				else
				{
					while(q[s] && q[s]!=' ' && q[s]!='\t')s++;
				}
				q[s]=0;	// null terminate the value
				stringsize+= s+1;
			}
		}
		
		// next line, please...
		
		while(off<size && p[off])off++;	// skip till NULL or eof
		off++;	// increase the pointer past the extra NULL.
		if(off>=size)break;	// if at eof, end.
	}
	
	// OK, we now have a count of items in 'num' and the string length in 'stringsize'
	// ... create the arrays
	
	data = (char *)malloc(stringsize);
	keys = new char * [num];
	vals = new char * [num];
	
	// reparse the file, filling the data in.
	
	i=0;	// index of current item
	
	curdata=data;

	for(off=0;off<size;off++)
	{
		
		// at this point we are pointing to a line.. if it
		// starts with # it's a comment
		
		if(p[off] != '#' && p[off])
		{
			// if it doesn't have a colon, ignore it.
			
			if(q=strchr(p+off,':'))
			{
				s=off;


				while(p[s]!=':' && p[s]!=' ' && p[s]!='\t')s++; // find end of key
				p[s]=0;
				
				keys[i] = curdata;
				
				strcpy(curdata,p+off);
				curdata+=strlen(p+off);*curdata++ = 0;
				
				// find the value start
				
				while(! (isalnum(*q) || *q=='-' || *q=='.')    )q++;
				strcpy(curdata,q);
				vals[i]=curdata;
				curdata+=strlen(q);*curdata++ = 0;
				
				i++;
			}
		}
		
		// next line, please...
		
		while(off<size && p[off])off++;	// skip till NULL or eof
		off++;	// increase the pointer past the extra NULL.
		if(off>=size)break;	// if at eof, end.
	}
	return true;
}


// search the file for the given key - if NULL, look for more occurrences
// of the previous Find key. Returns the value, or NULL if the key
// is not found.

char *BroadParamFile::Find(char *key,int len)
{
	if(key)
	{
		strcpy(curkey,key);
		offset=0;	// specifying a new search
	}
	else
		if(len==-1)len=lastlen;

	lastlen=len;

	for(;offset<num;offset++)
	{

		if(!len)
		{
			curoff=offset;
			return vals[offset++];	// return all values
		}
		else if(len<0)				// normal search
		{
			if(!strcmp(curkey,keys[offset]))
			{
				curoff=offset;
				return vals[offset++];
			}
		}
		else if(!strncmp(curkey,keys[offset],len))
		{
			curoff=offset;
			return vals[offset++];
		}
			
	}

	curoff=-1;
	return NULL;
}


