// $Header$

// $Log$
// Revision 1.20  2000-05-09 12:10:03+01  jcf
// Set UseZeroCentre to always true
//
// Revision 1.19  2000-04-19 21:37:56+01  img
// Localisation.
//
// Revision 1.18  2000-04-17 15:53:25+01  jcf
// <>
//
// Revision 1.17  2000-04-17 14:53:32+01  img
// Bug fixes
//
// Revision 1.16  2000-04-12 13:59:45+01  img
// bug fixes
//
// Revision 1.15  2000-04-12 12:21:22+01  img
// Bug fix
//
// Revision 1.14  2000-04-10 17:20:55+01  img
// Bug fix
//
// Revision 1.13  2000-04-07 14:36:52+01  img
// No model deletion (removeModelStore() does that now)
//
// Revision 1.12  2000-03-20 18:00:40+00  img
// Beta build
//
// Revision 1.11  2000-03-10 16:10:32+00  img
// New JUSTIFY_*
//
// Revision 1.10  2000-03-06 13:03:10+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.9  2000-03-03 14:42:27+00  jjs
// Fixed for Boris3.
//
// Revision 1.8  2000-03-02 14:26:35+00  img
// Controller screen now working
//
// Revision 1.7  2000-03-02 09:49:52+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.6  2000-02-29 08:43:34+00  jjs
// Added Header & Log lines.
//

/* Model file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#include "ConsoleFrontLib.h"

FrontEndObject::FrontEndObject(char *ArchiveName, char *ObjectName, float Scale, FrontEndObject *ParentFEObject, bool UseZeroCentre)
{
	if (ArchiveName)
	{

		// Push the archives in the archives directory.
		char ActualArchiveName[256];
#if defined(UNDER_CE)
		sprintf(ActualArchiveName, DISKNAME"\\SOS1937FrontEnd\\%s", ArchiveName);
#else
		sprintf(ActualArchiveName, ".\\Archives\\%s", ArchiveName);
#endif
		arcPush(ActualArchiveName);
	}
	// Load in the object from the archive.
	ActualModel = getModelPtr( ObjectName, Scale, false, NULL);
	if(!ActualModel)dprintf("load failed : %s",ObjectName);

	// Create the object
	ActualObject = new object(ActualModel, 0, false);

	if (ArchiveName)
	{
		// .. and pop the archive afterwards.
		arcPop();
	}

	if (ParentFEObject)
	{
		ParentFEObject->Attach(ActualObject);
		ActualObject->setAngle(&vector3(0,0,0));
		ParentObject = NULL;
		ActualObject->visible = true;
		ActualObject->changed = true;
		if (UseZeroCentre)
		{
			ActualObject->transform.translation = vector3(0.0f,0.0f,0.0f);
			Offset = vector3(0.0f,0.0f,0.0f);
		}
		else
		{
			Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
				     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
			ActualObject->transform.translation = Offset;
		}
	}
	else
	{
		// Make the invisible parent object.

		ParentObject = new object(NULL, 0, false);
		ParentObject->insertChild(ActualObject);
		if (UseZeroCentre)
			ActualObject->transform.translation = vector3(0.0f,0.0f,0.0f);
		else
			ActualObject->transform.translation = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
														  (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
														  (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
	}
	InChain = false;
}

FrontEndObject::~FrontEndObject()
{

	if (ActualObject)
	{
		//	ActualObject->detachChild();
		ActualObject->detachSister();
		ActualObject->detachChild();
//	if (ActualObject->prevsister) ActualObject->prevsister->detachSister();
		delete ActualObject;
	}
	else
	{
		dprintf("Urgh! model chain corrupted!!!!");
	}
	if (ParentObject)
	{
//		ParentObject->detachChild();
		if (InChain)
		{
#ifdef BORISEMULATIONHACK
			EmulationRemoveObject(ParentObject);
#else
			boris2->removeObject(ParentObject);
#endif
		}
		delete ParentObject;
	}
}

void FrontEndObject::DeleteModel()
{
//	delete ActualModel;
}

void FrontEndObject::Attach(object *NewChild)
{
	if (ActualObject->child)
		ActualObject->child->insertSister(NewChild);
	else
		ActualObject->insertChild(NewChild);
}

void FrontEndObject::Draw()
{
	if (ParentObject)
	{
		ParentObject->visible = true;
	}
	ActualObject->changed = true;
	ActualObject->visible = true;

	if (!InChain && ParentObject)
#ifdef BORISEMULATIONHACK
		EmulationInsertObject(ParentObject);
#else
		boris2->insertObject(ParentObject);
#endif
	
	InChain = true;
}

void FrontEndObject::Draw(vector3 *Position)
{
	Draw((int)Position->x, (int)Position->y, (float)Position->z);
}

void FrontEndObject::Draw(int x, int y, float z)
{
	ActualObject->setAngle(&vector3(0,0,0));

	static vector3 WorldVector;
#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z);
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,z*gWFSHack);
#endif
	if (ParentObject)
	{
		ParentObject->setAngle(&vector3(0,0,0));
		ParentObject->transform.translation = WorldVector;
	
		ParentObject->changed = true;
		ParentObject->visible = true;
		ParentObject->inheritTransformations();
		if (!InChain)
		{
#ifdef BORISEMULATIONHACK
			EmulationInsertObject(ParentObject);
#else
			boris2->insertObject(ParentObject);
#endif
		}
		InChain = true;
	}
	ActualObject->changed = true;
	ActualObject->visible = true;
}

void FrontEndObject::Hide()
{
	if (ParentObject)
	{	
		ParentObject->visible = false;
		if (InChain) 
#ifdef BORISEMULATIONHACK
			EmulationRemoveObject(ParentObject);
#else
			boris2->removeObject(ParentObject);
#endif
	}
	ActualObject->visible = false;
	InChain = false;
}

void FrontEndObject::Rotate(vector3 *Target)
{
	Rotate(Target->x, Target->y, Target->z);
}

void FrontEndObject::Rotate(float x, float y, float z)
{
	matrix3x4 RotationMatrixX, RotationMatrixY, RotationMatrixZ, TempMatrix;
	
	float rad_x = (x*PI*2.0f)/360.0f, rad_y = (y*PI*2.0f)/360.0f, rad_z = (z*PI*2.0f)/360.0f;

	float cosx = (float)cos(rad_x), cosy = (float)cos(rad_y), cosz = (float)cos(rad_z);
	float sinx = (float)sin(rad_x), siny = (float)sin(rad_y), sinz = (float)sin(rad_z);

	RotationMatrixX.set3x3submat(1.0f,0.0f,0.0f,
								0.0f,cosx,-sinx,
								0.0f,sinx,cosx);
	RotationMatrixY.set3x3submat(cosy,0.0f,-siny,
								0.0f,1.0f,0.0f,
								siny,0.0f,cosy);
	RotationMatrixZ.set3x3submat(cosz,-sinz,0.0f,
								sinz,cosz,0.0f,
								0.0f,0.0f,1.0f);
	matrix3x4 temp;

	temp.multiply3x3submats(&RotationMatrixY,&RotationMatrixZ);
	TempMatrix = temp;
	temp.multiply3x3submats(&RotationMatrixX,&TempMatrix);
	temp.translation = ActualObject->transform.translation;
	ActualObject->transform = temp;

	if (ParentObject)
	{
		ParentObject->changed = true;
		ParentObject->inheritTransformations();
	}
	else if (ActualObject->parent)
	{
		ActualObject->parent->changed = true;
		ActualObject->parent->inheritTransformations();
	}
}

void FrontEndObject::MoveInModelSpace(vector3 *Position)
{
	ActualObject->transform.translation = *Position + Offset;
	ActualObject->changed = true;
	if (ParentObject)
	{
		ParentObject->changed = true;
		ParentObject->inheritTransformations();
	}
	else if (ActualObject->parent)
	{
		ActualObject->parent->changed = true;
		if (ActualObject->parent->parent)
		{
			ActualObject->parent->parent->changed = true;
			ActualObject->parent->parent->inheritTransformations();
		}
		else	
			ActualObject->parent->inheritTransformations();
	}
}

void FrontEndObject::GetDimensions(vector3 *BottomLeft, vector3 *TopRight)
{
	BottomLeft->x = ActualModel->boundingBox.lo.x;
	BottomLeft->y = ActualModel->boundingBox.lo.y;
	BottomLeft->z = ActualModel->boundingBox.lo.z;

	TopRight->x = ActualModel->boundingBox.hi.x;
	TopRight->y = ActualModel->boundingBox.hi.y;
	TopRight->z = ActualModel->boundingBox.hi.z;
}

void FrontEndObject::Justify(int Edge)
{
	switch (Edge)
	{
	case JUSTIFY_CENTRE:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
	case JUSTIFY_LEFT:
		Offset = vector3(ActualModel->boundingBox.hi.x,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				 (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_RIGHT:
		Offset = vector3(ActualModel->boundingBox.lo.x,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_BOTTOM:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     ActualModel->boundingBox.hi.y,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_TOP:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     ActualModel->boundingBox.lo.y,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	default:
		break;
	}
}