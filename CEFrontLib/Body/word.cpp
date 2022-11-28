// $Header$

// $Log$
// Revision 1.11  2000-04-21 19:06:00+01  img
// Bug fixes
//
// Revision 1.10  2000-04-20 20:18:34+01  img
// bug fix
//
// Revision 1.9  2000-04-19 21:37:57+01  img
// Localisation.
//
// Revision 1.8  2000-04-12 14:00:12+01  img
// bug fixes
//
// Revision 1.7  2000-04-12 12:54:23+01  img
// Bug fix
//
// Revision 1.6  2000-03-13 18:13:30+00  img
// Converts to upper case now.
//
// Revision 1.5  2000-03-10 16:09:32+00  img
// True Justification; Word has InChain bool.
//
// Revision 1.4  2000-03-03 15:03:41+00  jjs
// Fixed for Boris3.
//
// Revision 1.3  2000-03-02 09:49:53+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.2  2000-02-29 08:43:38+00  jjs
// Added Header & Log lines.
//

/* Word file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#include "ConsoleFrontLib.h"

TextWord::TextWord(char *Data, Alphabet *UsedAlphabet, float MaxWidth)
{
	WordObject = new object(NULL, 0, false);

	float LetterPosition = 0.0f, ThisKerning;
	int LastLetter = 0;
	for (unsigned int i=0 ; i<strlen(Data) ; i++)
	{
		if ((unsigned char)*(Data+i) >=(unsigned char)'a' && (unsigned char) *(Data+i) <= (unsigned char)'z') *(Data+i) -= ((unsigned char)'a'-(unsigned char)'A'); 
		if ((unsigned char)*(Data+i) >=(unsigned char)'Á' && (unsigned char) *(Data+i) <= (unsigned char)'Ü') *(Data+i) += ((unsigned char)'á'-(unsigned char)'Á');
		if ((unsigned char)*(Data+i) == 32 || !UsedAlphabet->Letter[(unsigned char )*(Data+i)])
		{
			LetterPosition += UsedAlphabet->Kerning[(unsigned char)'T'];
			Letters[i] = NULL;
		}
		else
		{
			ThisKerning = UsedAlphabet->Kerning[(unsigned char)*(Data+i)] * (*(Data+i+1) == 'I' || *(Data+i+1) == 'i' ? 1.0f : 0.9f);
			Letters[i] = new object(UsedAlphabet->Letter[(unsigned char)*(Data+i)],0,false);
			Letters[i]->visible = false;
			Letters[i]->setAngle(&vector3(0,0,0));

			Letters[i]->transform.translation = vector3(LetterPosition + ThisKerning*0.5f,0,0);
			Letters[i]->changed = true;
			if (i && Letters[LastLetter])
				Letters[LastLetter]->insertSister(Letters[i]);
			else
				WordObject->insertChild(Letters[i]);
			LetterPosition += ThisKerning;
			LastLetter = i;
		}
	}

	Length = LastLetter;

	// Now we know the width of the word...
	Width = LetterPosition;
	Height = UsedAlphabet->Height;

	// Set the object point to the middle of the word.
	for (i=0 ; i<strlen(Data) ; i++)
	{
		if (Letters[i])
		{
			Offset[i].x = (Letters[i]->transform.translation.x -= Width*0.5f);
			Offset[i].y = (Letters[i]->transform.translation.y -= Height*0.5f);
			Offset[i].z = 0.0f;
		}
	}
	Letters[i] = NULL;

	WordObject->transform.translation.z = -100.0f;
	WordObject->changed = true;
	WordObject->inheritTransformations();

	InChain = false;
}

TextWord::~TextWord()
{
	if (InChain)
	{
	// Remove the word from the engine.
#ifdef BORISEMULATIONHACK
		EmulationRemoveObject(WordObject);
#else
		boris2->removeObject(WordObject);
#endif
	}
	if (Letters[0])
		Letters[0]->detachChild();

	// Now go through the letters, removing and deleting them.
	object *CurrentObject = Letters[0], *NextObject;
	while (CurrentObject)
	{
		NextObject = CurrentObject->sister;
		CurrentObject->detachSister();
		delete CurrentObject; 
		CurrentObject = NextObject;
	}
	delete WordObject;
	WordObject = NULL;
	for (int i=0 ; i<80 ; i++) Letters[i] = NULL;
}

void TextWord::Write()
{
	Write(PreviousX, PreviousY, PreviousZ);
}

void TextWord::Write(int x, int y, float z)
{
	PreviousX = x ; PreviousY = y ; PreviousZ = z;
	WordObject->setAngle(&vector3(0,0,0));

	vector3 WorldVector;
#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z);
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,z * gWFSHack);
#endif
	WordObject->transform.translation = WorldVector;

	
	WordObject->changed = true;
	Rotate(0,0,0);
	Rotation.x = 0.0f; Rotation.y = 0.0f; Rotation.z = 0.0f;

	WordObject->visible = true;
	for(int i=0;i<=Length;i++)if(Letters[i])Letters[i]->visible=true;	// jcf

	WordObject->inheritTransformations();

	if (!InChain)
	{
#ifdef BORISEMULATIONHACK
		EmulationInsertObject(WordObject);
#else
		boris2->insertObject(WordObject);
#endif
	}
	InChain = true;
}

void TextWord::Hide()
{
	if (InChain)
	{
#ifdef BORISEMULATIONHACK
		EmulationRemoveObject(WordObject);
#else
		boris2->removeObject(WordObject);
#endif
	}
	WordObject->visible = false;
	for(int i=0;i<=Length;i++)if(Letters[i])Letters[i]->visible=false;	// jcf
	InChain = false;
}

void TextWord::Rotate(int x, int y, int z)
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
	temp.translation = WordObject->transform.translation;
	WordObject->transform = temp;
	WordObject->changed = true;

	WordObject->inheritTransformations();
}

void TextWord::RotateLetters(int x, int y, int z)
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

	object *ThisObject;

	ThisObject = WordObject->child;
	WordObject->changed = true;

	while (ThisObject)
	{
		temp.translation = ThisObject->transform.translation;
		ThisObject->transform = temp;
		ThisObject->changed = true;
		ThisObject = ThisObject->sister;
	}

	WordObject->inheritTransformations();
}

void TextWord::Move(int x, int y, float z)
{
#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&(WordObject->transform.translation), (float)x, (float)y, z);
#else
	boris2->getWorldFromScreen(&(WordObject->transform.translation), (float)x, (float)y, z * gWFSHack);
#endif
	WordObject->changed = true;
	WordObject->inheritTransformations();
}

void TextWord::Justify(int Edge)
{
	int i=0;
	switch (Edge)
	{
	case JUSTIFY_CENTRE:
		for (i=0 ; i<=Length ; i++)
		{
			Letters[i]->transform.translation = Offset[i];
		}
		break;	
	case JUSTIFY_LEFT:
		for (i=0 ; i<=Length ; i++)
		{
			if (Letters[i])
			{
				Letters[i]->transform.translation = Offset[i];
				Letters[i]->transform.translation.x = Offset[i].x + Width*0.5f;
			}
		}
		break;
	case JUSTIFY_RIGHT:
		for (i=0 ; i<=Length ; i++)
		{
			if (Letters[i])
			{
				Letters[i]->transform.translation = Offset[i];
				Letters[i]->transform.translation.x -= Width*0.5f;
			}
		}
		break;
	case JUSTIFY_BOTTOM:
		break;
	case JUSTIFY_TOP:
		break;
	default:
		break;
	}
	Write();
}