// $Header$

// $Log$
// Revision 1.1  2000-04-13 15:23:29+01  jjs
// Adds SSB support.
//
// Revision 1.0  1999-12-07 11:48:47+00  jjs
// Initial revision
//

// first we have the header

#define TARC_MAGIC	0xdeafcede

struct tarc_header
{
	long magic;		// magic number
	char vblock[64]; 	// encryption verification block
	long filect;	// file count
	long index;	// offset to index (at end of file)
	int createdate;		// creation date
};

// then we have data blocks, which are effectively raw.

// then we have the index block, consisting of these:

struct tarc_index
{
	unsigned long id;	// format identifier
	unsigned long date;	// date in internal format
	long index;	// byte offset of data from file start
	long size;	// size of data
	unsigned short flags;	// TARCF_ flags
	short version;		// version number
	char tname[128];	// texture name
	char fname[256];	// file name from which it came
};

#define	TARCF_DELETED	1
#define	TARCF_SUPERCEDED	2


// special version numbers

#define	TARCV_LATEST	-1	// the latest
#define	TARCV_OLD	-2	// all old versions
#define	TARCV_ALL	-3	// all versions
#define	TARCV_DEL	-4	// deleted (not merely old) versions

// this structure is used to hold an opened tarc's details

struct tarc
{
	HANDLE	 f;
	int modified;
	struct tarc_header hd;
	struct tarc_index *i;
	char fn[256];
	int writable;
	char *block;		// NULL if the file is not cached in memory
};

/****Fri Dec 12 10:57:39 1997*************(C)*Broadsword*Interactive****
 *
 * prototypes
 *
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BOOL TARC_SetCacheMode(BOOL);
int TARC_ReadHeaderAndIndex(struct tarc *t);
int TARC_OpenR(struct tarc *t,char *fname);
unsigned long TARC_Extract(struct tarc *t,char *name,unsigned long id,
	short version,char *buf);
char *TARC_GetFormatName(unsigned long id);
unsigned long TARC_GetFormatByName(char *s);
void TARC_CloseR(struct tarc *t);
struct tarc_index *TARC_GetIndexData(struct tarc *t,char *name,unsigned long id,
	short version);

/* internals */

// TARC_List formats 

#define	LF_NAMES	1
#define	LF_FILES	2
#define	LF_DATES	4
#define	LF_SIZES	8

void TARC_ExtractEntry(struct tarc *t,struct tarc_index *ti,void *buf);
int TARC_EntryMatches(struct tarc_index *i,unsigned long id,
	char *name,short version);
void TARC_Crypt(char *buf,long size,char *key);

extern char *KeyHeader,*KeyBody,*KeyIndex; 

#ifdef __cplusplus
}
#endif

