// $Header$

// $Log$

#ifndef _BROAD_ARCHIVE_H_
#define	_BROAD_ARCHIVE_H_


class BroadArchive
{
private:
	struct tarc *t;
	bool isopen;
	unsigned long size;
public:
	BroadArchive();
	~BroadArchive();

	// open an archive

	bool Open(char *filename);

	// close an archive

	void Close();

	// extract an item from the archive into a memory area. ID is actually
	// the format of the item. The number of bytes extracted is returned.
	// Please ensure the buffer is big enough!
	// example: size=archive->Extract("world",FMT_VO_OBJ,buf);
	
	unsigned long Extract(char *name,unsigned long id,char *buffer);

	// return the size of the last item extracted. Not terribly useful!
	
	unsigned long Size() { return size; }

	// return some details about an item in the archive. See tarc.h for
	// details of the tarc_index structure, but important members are:
	// size: size of item in bytes

	struct tarc_index *Stat(char *name,unsigned long id);
};

#endif
