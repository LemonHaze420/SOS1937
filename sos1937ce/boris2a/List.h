/*
	$Id: List.h,v 1.2 1998/12/02 11:36:01 jcf Exp jcf $
	$Log: List.h,v $
	Revision 1.2  1998/12/02 11:36:01  jcf
	Added id and log entries

*/

 /****Tue Oct 21 14:45:53 1997*************(C)*Broadsword*Interactive****
 *
 * Lists. BroadList is the basic list object, which can list anything.
 *
 ***********************************************************************/

#ifndef _BROAD_LIST_H_
#define	_BROAD_LIST_H_


class voidList	// lists pointers to void.
{

/****Tue Oct 21 14:46:29 1997*************(C)*Broadsword*Interactive****
 *
 * Privates
 *
 ***********************************************************************/

private:

	long *nextlist,*prevlist;
	long head,freehead;
	void **objlist;
	int size;

	int current;

	void AddToFreeList(int slot);

/****Tue Oct 21 14:48:57 1997*************(C)*Broadsword*Interactive****
 *
 * Publics
 *
 ***********************************************************************/
 
public:

	voidList(int sz);
	~voidList();

	int Add(void *obj);


	BOOL Remove(void *obj);
	BOOL Remove(int slot);

	void *First();
	void *Next();

	void *Get(int slot) { return objlist[slot]; }

	void Empty();
};

/****Wed Oct 22 13:42:55 1997*************(C)*Broadsword*Interactive****
 *
 * Now we declare our template class. We use this by declaring
 * BroadList<..sometype..>, usually BroadList<BroadObject>
 *
 ***********************************************************************/
 
template<class T> class List : public voidList
{
public:

	List(int siz) : voidList(siz)
	{
		;
	}
	
	int Add(T *obj){return voidList::Add((void *)obj);}

	BOOL Remove(void *obj){return voidList::Remove((void *)obj);}
	BOOL Remove(int slot ){return voidList::Remove(slot);}

	T *First(){return (T *)voidList::First();}
	T *Next(){return (T *)voidList::Next();}

	T *Get(int slot) { return (T *)voidList::Get(slot); }

	void Empty() { voidList::Empty(); }
};

#endif
