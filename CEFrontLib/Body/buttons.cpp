// $Header$

// $Log$
// Revision 1.7  2000-04-04 18:37:59+01  img
// Cleaned up slightly
//
// Revision 1.6  2000-03-20 18:00:40+00  img
// Beta build
//
// Revision 1.5  2000-03-03 14:42:43+00  jjs
// Fixed for Boris3
//
// Revision 1.4  2000-02-29 08:43:33+00  jjs
// Added Header & Log lines.
//


/* The Buttons used in the front end

	Ian Gledhill 12/01/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

Button::Button(char *ButtonName, float Scale, int x, int y, float z)
{
	strcpy(OriginalName = (char *)malloc(strlen(ButtonName)+2), ButtonName);
//	OriginalName = strdup(ButtonName);

	StartX = x; StartY = y; StartZ = z;

	char ModifiedName[256];
	vector3 WorldVector;

	// Do the Unselected ones first.
	sprintf(ModifiedName, "U%s", ButtonName);
	UActualModel = getModelPtr(ModifiedName, Scale, false, NULL);
	UActualObject = new object(UActualModel, 0, false);

	UParentObject = new object(NULL, 0, false);
	UParentObject->insertChild(UActualObject);
	UActualObject->transform.translation = vector3(-UActualModel->boundingBox.lo.x, -UActualModel->boundingBox.hi.y, -UActualModel->boundingBox.hi.z);

	UActualObject->setAngle(&vector3(0.0f,0.0f,0.0f));
	UParentObject->setAngle(&vector3(0.0f,0.0f,0.0f));

	UParentObject->changed = true;

#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z+UActualModel->boundingBox.hi.z*-0.600f);
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,(z+UActualModel->boundingBox.hi.z*-0.600f) * gWFSHack);
#endif
	UParentObject->transform.translation = WorldVector;

	UParentObject->changed = true;

	UParentObject->visible = true;
	UActualObject->visible = true;

	UParentObject->inheritTransformations();

	*ModifiedName = 'H';
	HActualModel = getModelPtr(ModifiedName, Scale, false, NULL);
	HActualObject = new object(HActualModel, 0, false);

	HParentObject = new object(NULL, 0, false);
	HParentObject->insertChild(HActualObject);
	HActualObject->transform.translation = vector3(-HActualModel->boundingBox.lo.x, -HActualModel->boundingBox.hi.y, -HActualModel->boundingBox.hi.z);

	HActualObject->setAngle(&vector3(0.0f,0.0f,0.0f));
	HParentObject->setAngle(&vector3(0.0f,0.0f,0.0f));

	HParentObject->changed = true;

#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z+HActualModel->boundingBox.hi.z*-0.6f);
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,(z+HActualModel->boundingBox.hi.z*-0.6f) * gWFSHack);
#endif
	HParentObject->transform.translation = WorldVector;

	HParentObject->changed = true;

	HParentObject->visible = true;
	HActualObject->visible = true;

	HParentObject->inheritTransformations();

	*ModifiedName = 'S';
	SActualModel = getModelPtr(ModifiedName, Scale, false, NULL);
	SActualObject = new object(SActualModel, 0, false);

	SParentObject = new object(NULL, 0, false);
	SParentObject->insertChild(SActualObject);
	SActualObject->transform.translation = vector3(-SActualModel->boundingBox.lo.x, -SActualModel->boundingBox.hi.y, -SActualModel->boundingBox.hi.z);

	SActualObject->setAngle(&vector3(0.0f,0.0f,0.0f));
	SParentObject->setAngle(&vector3(0.0f,0.0f,0.0f));

	SParentObject->changed = true;

#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z+SActualModel->boundingBox.hi.z*-0.600f);
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,(z+SActualModel->boundingBox.hi.z*-0.6f) * gWFSHack);
#endif
	SParentObject->transform.translation = WorldVector;

	SParentObject->changed = true;

	SParentObject->visible = true;
	SActualObject->visible = true;

	SParentObject->inheritTransformations();
}

Button::~Button()
{
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(UParentObject);
#else
	boris2->removeObject(UParentObject);
#endif	
//	delete UActualModel; delete HActualModel; delete SActualModel;
	delete UActualObject;delete HActualObject;delete SActualObject;
	delete UParentObject;delete HParentObject;delete SParentObject;

	free(OriginalName);
}

void Button::Draw()
{
#ifdef BORISEMULATIONHACK
	EmulationInsertObject(UParentObject);
#else
	boris2->insertObject(UParentObject);
#endif
}

void Button::Hide()
{
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(UParentObject);
#else
	boris2->removeObject(UParentObject);
#endif	
}

void Button::Highlight()
{
//	return false;
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(UParentObject);
	EmulationInsertObject(HParentObject);
#else
	boris2->removeObject(UParentObject);
	boris2->insertObject(HParentObject);
#endif
}

void Button::Lowlight()
{
//	return false;
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(HParentObject);
	EmulationInsertObject(UParentObject);
#else
	boris2->removeObject(HParentObject);
	boris2->insertObject(UParentObject);
#endif
}

void Button::Select()
{
//	return false;
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(UParentObject);
	EmulationInsertObject(SParentObject);
#else
	boris2->removeObject(UParentObject);
	boris2->insertObject(SParentObject);
#endif
}

void Button::Unselect()
{
//	return false;
#ifdef BORISEMULATIONHACK
	EmulationRemoveObject(SParentObject);
	EmulationInsertObject(UParentObject);
#else
	boris2->removeObject(SParentObject);
	boris2->insertObject(UParentObject);
#endif
}

ButtonBank::ButtonBank()
{
	ButtonList = NULL;
}

ButtonBank::~ButtonBank()
{
	ButtonNode *ThisNode = ButtonList, *NextNode;
	while (ThisNode)
	{
		NextNode = ThisNode->NextNode;
//		delete ThisNode->Node;
		ThisNode->Node->Hide();
		delete ThisNode;
		ThisNode = NextNode;
	}
}

void ButtonBank::Add(Button *NewButton)
{
	ButtonNode *ThisNode = ButtonList;

	if (ButtonList)
	{
		while (ThisNode->NextNode)
		{
			ThisNode = ThisNode->NextNode;
		}

		ThisNode->NextNode = new ButtonNode;
		ThisNode = ThisNode->NextNode;
		ThisNode->NextNode = NULL;
		ThisNode->Node = NewButton;
	}
	else
	{
		ButtonList = new ButtonNode;
		ButtonList->NextNode = NULL;
		ButtonList->Node = NewButton;
	}
	
}

void ButtonBank::Remove(Button *OldButton)
{
}

void ButtonBank::DrawAll()
{
	ButtonNode *ThisNode = ButtonList;

	while (ThisNode)
	{
		ThisNode->Node->Draw();
		ThisNode = ThisNode->NextNode;
	}
}

void ButtonBank::Highlight(int Index)
{
	ButtonNode *ThisNode = ButtonList;
	for (int i=0 ; i<Index-1 ; i++)
	{
		ThisNode = ThisNode->NextNode;
	}

	// Now we're on the right node.
	if (ThisNode)
	{
		ThisNode->Node->Highlight();
	}
}

void ButtonBank::Lowlight(int Index)
{
	ButtonNode *ThisNode = ButtonList;
	for (int i=0 ; i<Index-1 ; i++)
	{
		ThisNode = ThisNode->NextNode;
	}

	if (ThisNode)
	{
		ThisNode->Node->Lowlight();
	}
}

void ButtonBank::Select(int Index)
{
	ButtonNode *ThisNode = ButtonList;
	for (int i=0 ; i<Index-1 ; i++)
	{
		ThisNode = ThisNode->NextNode;
	}

	// Now we're on the right node.
	if (ThisNode)
	{
		ThisNode->Node->Select();
	}
}

void ButtonBank::Unselect(int Index)
{
	ButtonNode *ThisNode = ButtonList;
	for (int i=0 ; i<Index-1 ; i++)
	{
		ThisNode = ThisNode->NextNode;
	}

	// Now we're on the right node.
	if (ThisNode)
	{
		ThisNode->Node->Unselect();
	}
}
