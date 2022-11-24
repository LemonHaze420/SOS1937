// $Header$

// $Log$
// Revision 1.3  2000-01-28 11:59:12+00  img
// <>
//
// Revision 1.2  2000-01-19 11:48:07+00  jjs
// Ians fix for deleting modelchain.
//
// Revision 1.1  2000-01-13 17:27:05+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

extern model *modelChain;

model::model()
{
	//spheres = NULL;
}

model::~model()
{
//	meshData.~mesh();
//	delete meshData;
	delete [] meshData;
	//IMG remove the model from the chain.
	model *CurrentModel = modelChain, *PreviousModel = NULL;

	while (CurrentModel)
	{
		if (CurrentModel == this)
		{
			if (PreviousModel)
				PreviousModel->next = this->next;
			else
				modelChain = NULL;
		}

		PreviousModel = CurrentModel;
		CurrentModel = CurrentModel->next;
	}

}
