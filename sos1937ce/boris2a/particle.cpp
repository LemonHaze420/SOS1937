#include "BS2all.h"
#include "pool.h"
#include "list.h"
#include "particle.h"

#define	MAXPARTICLES	4096

class Particle
{
public:
	vector3 position;
	vector3 velocity;
	vector3 acceleration;
	unsigned long ticksleft;
	DWORD col;
	float size;
	material m;		// handy storage
	int slot;
};


Pool<Particle> *partpool=NULL;
List<Particle> *partlist=NULL;

void initialiseParticles()
{
	partpool = new Pool<Particle> (MAXPARTICLES);
	partlist = new List<Particle> (MAXPARTICLES);
}

void shutdownParticles()
{
	delete partpool;
	delete partlist;
}

float __inline frand()		//return rand from nearly -1 to nearly 1
{
	float q = (float)((rand()%32700) - 16384);
	q/=16384.0;

	return q;
}



static vector3 *randvec(vector3 *v,vector3 *ss)
{
	static vector3 tv;

	tv.x = frand() * v->x + ss->x;
	tv.y = frand() * v->y + ss->y;
	tv.z = frand() * v->z + ss->z;

	return &tv;
}

void makeParticle(vector3 *position,vector3 *vel,vector3 *acc,
				  vector3 *posrand,vector3 *velrand,
				  unsigned long tickstolive,float size,DWORD col)
{
	// attempt to allocate a particle from the particle bank

	Particle *p = partpool->Allocate();

	if(p)
	{
		p->position = *randvec(posrand,position);
		p->velocity = *randvec(velrand,vel);
		p->velocity *= 1.0/60.0;
		p->acceleration = *acc;
		p->acceleration *= 1.0/60.0;
		p->size = size;
		p->col = col;
		p->ticksleft = tickstolive;

		p->slot = partlist->Add(p);
	}
}

void runParticles()
{
	Particle *remlist[64];
	int remlistct=0;

	Particle *p;

	for(p=partlist->First();p;p=partlist->Next())
	{
		if(--(p->ticksleft))
		{
			p->position += p->velocity;
			p->velocity += p->acceleration;
		}
		else
		{
			if(remlistct<64)remlist[remlistct++]=p;
		}
	}

	for(int i=0;i<remlistct;i++)
	{
		partlist->Remove(remlist[i]->slot);
		partpool->Free(remlist[i]	);
	}
}

void drawParticles()
{
	Particle *p;
	for(p=partlist->First();p;p=partlist->Next())
	{
		boris2->DrawParticle(&(p->position),p->col,p->size,&(p->m));
	}
}