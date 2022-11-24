// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:27:06+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"
#include <malloc.h>

SphereSet::SphereSet(char *filename, float scale)
{
	char *area = (char *)_alloca(32768);

	data=NULL;

	unsigned long size = archivestack.ExtractIntoArea(filename,FMT_SPHERESET,area,32768);

	if(size)
	{
		num = *((int *)area);
		area+=sizeof(int);

		data = new Sphere [num];

		for(int i=0;i<num;i++)
		{
			data[i].type = *((SphereType *)area);
			area+=sizeof(SphereType);
			data[i].v = (*((vector3 *)area)) * scale;
			area+=sizeof(vector3);
			data[i].v2 = *((vector3 *)area);
			area+=sizeof(vector3);
			data[i].rad = (*((float *)area)) * scale ;
			area+=sizeof(float);

			data[i].radsquared = data[i].rad*data[i].rad;
		}
	}
}

SphereSet::~SphereSet()
{
	if(data)
		delete [] data;
}

//================================================================================================================================
//  create 'this.transformedSpheres' from the parameters:
//									
//================================================================================================================================

void SphereSet::transformSpheres( SphereSet * ss, matrix3x4 * transf )
{
	Sphere *st,*s,*end;

	ss->num  = this->num;
	s        = this->data;
	st       = ss->data;

	end      = s + this->num;

	for(;s!=end;s++,st++)
	{
		*st = *s;

		switch(s->type)
		{
			case SPHERE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				break;
			case LINE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				transf->multiplyV3By3x3mat(&st->v2, &s->v2);
				break;
			case CIRCLE:
				transf->multiplyV3By3x4mat(&st->v, &s->v);
				transf->multiplyV3By3x3mat(&st->v2, &s->v2);

				st->offset =	-(st->v2.x * st->v.x +
								st->v2.y * st->v.y +
								st->v2.z * st->v.z);

				// here we get the rotation of the wheel so we can find where it has
				// the low friction

				break;
		}
	}
}

