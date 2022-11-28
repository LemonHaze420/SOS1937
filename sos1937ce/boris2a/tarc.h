/****Thu Dec 11 15:50:21 1997*************(C)*Broadsword*Interactive****
 *
 * This file defines the TARC texture archive file format.
 *
 ***********************************************************************/



// first we have the header

#define TARC_MAGIC	0xdeafcede

struct tarc_header
{
	long magic;		// magic number
	char vblock[64]; 	// encryption verification block
	unsigned long filect;	// file count
	unsigned long index;	// offset to index (at end of file)
	int createdate;		// creation date
};

// then we have data blocks, which are effectively raw.

// then we have the index block, consisting of these:

struct tarc_index
{
	unsigned long id;	// format identifier
	unsigned long date;	// date in internal format
	unsigned long index;	// byte offset of data from file start
	unsigned long size;	// size of data
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
	HFILE h;
	int modified;
	struct tarc_header hd;
	struct tarc_index *i;
	char fn[256];
	int writable;
};

/****Fri Dec 12 10:57:39 1997*************(C)*Broadsword*Interactive****
 *
 * prototypes
 *
 ***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int TARC_ReadHeaderAndIndex(struct tarc *t);
int TARC_OpenR(struct tarc *t,char *fname);
unsigned long TARC_Extract(struct tarc *t,char *name,unsigned long id,
	short version,char *buf);
char *TARC_GetFormatName(unsigned long id);
unsigned long TARC_GetFormatByName(char *s);
void TARC_CloseR(struct tarc *t);
struct tarc_index *TARC_GetIndexData(struct tarc *t,char *name,unsigned long id,
	short version);


/* these are only required by the archive manager */

#ifdef TARCMAN
int TARC_OpenW(struct tarc *t,char *fname);
void TARC_AddData(struct tarc *t,void *data, unsigned long size, unsigned long id,
char *tname,char *fname,unsigned long edate);
void TARC_CloseW(struct tarc *t);
void TARC_MarkAsDeleted(struct tarc *t,char *name,unsigned long id,short version);
void TARC_Revert(struct tarc *t,char *name,unsigned long id,short version);
void TARC_ExtractFile(struct tarc *t,char *name,unsigned long id,short version,char *dest);
void TARC_ExtractTextToStdout(struct tarc *t,char *name,unsigned long id,short version);
void TARC_Clean(struct tarc *t);
void TARC_List(struct tarc *t, char *name, unsigned long id,short version,short flags);


#endif

/* internals */

// TARC_List formats 

#define	LF_NAMES	1
#define	LF_FILES	2
#define	LF_DATES	4
#define	LF_SIZES	8

void TARC_ExtractEntry(struct tarc *t,struct tarc_index *ti,void *buf);
int TARC_EntryMatches(struct tarc_index *i,unsigned long id,
	char *name,short version);
void TARC_Crypt(char *buf,unsigned long size,char *key);

extern char *KeyHeader,*KeyBody,*KeyIndex; 

#ifdef __cplusplus
}
#endif

