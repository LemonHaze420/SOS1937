// $Header$

// $Log$
// Revision 1.3  2000-02-29 08:43:32+00  jjs
// Added Header & Log lines.
//


/* Emulation of Boris2 stuff. */

#include "ConsoleFrontLib.h"

#if defined(BORISEMULATIONHACK)
struct ObjectNode
{
	object *This;
	struct ObjectNode *Next;
	struct ObjectNode *Previous;
};

struct ObjectNode *ObjectList=NULL;

void EmulationRemoveAllObjects()
{
	ObjectList = NULL;
}

void EmulationInsertObject(object *NewObject)
{
	struct ObjectNode *NewNode;

	NewNode = new ObjectNode;
	NewNode->This = NewObject;

	// Insert the new node at the beginning of the list.
	
	NewNode->Next = ObjectList;
	ObjectList = NewNode;
	if (NewNode->Next)
		NewNode->Next->Previous = NewNode;
	NewNode->Previous = NULL;
}

void EmulationRemoveObject(object *OldObject)
{
	struct ObjectNode *TestedNode = ObjectList;
	bool found = false;

	while (TestedNode && !found)
	{
		if (TestedNode->This == OldObject)
			found = true;
		else
			TestedNode = TestedNode->Next;
	}

	// No such object.
	if (!found || !TestedNode)
		return;

	if (TestedNode->Previous)
		TestedNode->Previous->Next = TestedNode->Next;
	if (TestedNode->Next)
		TestedNode->Next->Previous = TestedNode->Previous;

	if (TestedNode == ObjectList)
		ObjectList = TestedNode->Next;
		
	delete TestedNode;
	
}

object::~object()
{
};

void EmulationGetWorldFromScreen(vector3 *Destination, float x, float y, float z)
{
	boris2->getWorldFromScreen(Destination, x,y,z/21000.0f);
	return;
	static float Y=950.0f;
	*Destination = vector3(0.0f,Y,300.0f);
	Y+=1.0f;
}

void EmulationRender()
{
	struct ObjectNode *TestedNode = ObjectList;
	if (ObjectList)
	{
		while (TestedNode)
		{
			boris2->processObjectUnlit(TestedNode->This);
			TestedNode = TestedNode->Next;
		}
	}
}

#endif