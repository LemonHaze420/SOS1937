// $Header$

// $Log$
// Revision 1.3  2000-02-29 08:44:43+00  jjs
// Added Header and Log lines
//

/* Model file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#ifndef __FRONTENDOBJECT_H__
#define __FRONTENDOBJECT_H__

#include "ConsoleFrontLib.h"

class FrontEndObject
{
public:
	// Use ParentObject if this is relative to another FrontEndObject.
	FrontEndObject(char *ArchiveName, char *ObjectName, float Scale=1.0f, FrontEndObject *ParentFEObject=NULL, bool UseZeroCentre=false);
	~FrontEndObject();

	// Delete the associated model.
	void DeleteModel();

	void Draw();
	void Draw(vector3 *Position);
	void Draw(int x, int y, float z=20.0f);
	void Hide();

	void MoveInModelSpace(vector3 *Position);

	void Rotate(vector3 *Target);
	void Rotate(float x=0.0f, float y=0.0f, float z=0.0f);

	void Attach(object *NewChild);

	void GetDimensions(vector3 *BottomLeft, vector3 *TopRight);

	// justify the model according to the parameter. i.e. Left means left-justified text.
	void Justify(int Edge);
private:
	// The boris2 objects.
	// The Parent Object is used to make all operators on the object central.
	object *ParentObject, *ActualObject;

	model *ActualModel;

	vector3 Offset;

	// Set to true if the model is in the model chain.
	bool InChain;
};

#endif