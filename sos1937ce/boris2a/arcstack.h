/****Thu Jan 08 11:38:20 1998*************(C)*Broadsword*Interactive****
 *
 * Archive stack. 
 *
 ***********************************************************************/


#define	STACKSIZE	8	// maximum number of stacked archives.


// Note that there is only one archivestack object, called "archivestack".
// You don't have to declare it, it's done for you.

class BroadArchiveStack
{
public:
	class BroadArchive stack[STACKSIZE];
	
	int stackct;

	BroadArchiveStack();
	~BroadArchiveStack();

	// push an archive onto the stack. Returns false if the archive was not found.
	// CHECK THIS, because trying to do an extract on a non-existent archive will
	// crash!

	BOOL Push(char *name);

	// pop the top archive from the stack.

	void Pop();

	// extract an item into an already allocated area of memory. The top archive
	// is searched first, and then the one below and so on. The size of the
	// item found is returned, or 0 for not found. 0xffffffff is returned if
	// the area is not big enough for the archive.
	
	// Example:
	//	char *area = malloc(BUFFERSIZE);
	//	arcstack.Push("world.tc");
	//	size=arcstack.ExtractIntoArea("world",FMT_VO_OBJ,area,BUFFERSIZE);
	//	arcstack.Pop();
	//	ProcessWorld(area);
	//	free(area);
	
	unsigned long ExtractIntoArea(char *name,unsigned long id,char *area,int asize);

	// As above, but internally mallocs a region of memory which you have to
	// free when you've finished with the data. Example:
	//	char *area;
	//	arcstack.Push("world.tc");
	//	area=arcstack.ExtractIntoArea("world",FMT_VO_OBJ);
	//	arcstack.Pop();
	//	ProcessWorld(area);
	//	free(area);
	
	// If you want to know the size of the allocated area, the optional third argument points
	// to an unsigned long which will be filled in with the size.

	char *BroadArchiveStack::MallocAndExtract(char *name, unsigned long id,unsigned long *size=NULL);

};

extern BroadArchiveStack archivestack;
