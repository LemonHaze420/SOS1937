/*
	$Id: pool.h,v 1.1 1998/12/02 11:38:19 jcf Exp jcf $
	$Log: pool.h,v $
	Revision 1.1  1998/12/02 11:38:19  jcf
	Initial revision

*/

// Pool.h: interface for the Pool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_)
#define AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class voidPool
{
protected:
	int itemsize;	// size of each item
	int itemct;		// size of pool
	char *pool;		// pointer to actual data
	int	*next;		// points to next item in freelist
	int freelisthead; 

public:
	voidPool(int sz,int count)
	{
		itemsize=0;
		itemct=0;

		// set up freelist

		next = (int *)malloc(sizeof(int)*count);

		freelisthead=0;
		for(int i=0;i<count;i++)
			next[i]=i+1;
		next[count-1]=-1;

		itemsize = sz;
		itemct = count;
	}

	~voidPool()
	{
		free(pool);
		free(next);
	}

	void *Allocate()
	{
		int q = freelisthead;
		if(q<0)return NULL;

		freelisthead = next[q];
		return (void *)(pool+itemsize*q);
	}

	void Free(void *item)
	{
		int n = (((char *)item)-pool)/itemsize;

		next[n] = freelisthead;
		freelisthead = n;
	}

	void Empty()
	{
		freelisthead=0;
		for(int i=0;i<itemct;i++)
			next[i]=i+1;
		next[itemct-1]=-1;
	}

};
	



template<class T> class Pool : public voidPool
{
public:
	Pool(int count) : voidPool(sizeof(T),count)
	{
		pool = (char *)malloc(sizeof(T)*count);
	}

	T *Allocate() {
		T *o = (T *)voidPool::Allocate();

		return o;
	}
	void Free(T *item) { voidPool::Free((void *)item); }
	void Empty() { voidPool::Empty(); }

};





#endif // !defined(AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_)
