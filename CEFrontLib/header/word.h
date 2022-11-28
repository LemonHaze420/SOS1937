// $Header$

// $Log$
// Revision 1.2  2000-03-10 16:09:32+00  img
// True Justification; Word has InChain bool.
//
// Revision 1.1  2000-02-29 08:44:47+00  jjs
// Added Header and Log lines
//

/* Word file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#ifndef __TEXTWORD_H__
#define __TEXTWORD_H__

#define ROT_XYZ 0
#define ROT_XZY 1
#define ROT_YXZ 2
#define ROT_YZX 3
#define ROT_ZYX 4
#define ROT_ZXY 5

class TextWord
{
public:
	TextWord(char *Data, Alphabet *UsedAlphabet, float MaxWidth = -1);
	~TextWord();

	// Write the word at x,y
	void Write(int x, int y, float z=20.0f);
	void Write();

	// Remove the word from the display.
	void Hide();

	// Rotate the word
	void Rotate(int x, int y, int z);

	// Rotate the component letters
	void RotateLetters(int x, int y, int z);

	// Move the word
	void Move(int x, int y, float z=20.0f);

	// Justify it to a particular edge.
	void Justify(int Edge);

	vector3 Rotation;
	vector3 BasePosition;

private:
	object *WordObject, *ParentObject;
	object *Letters[80];

	vector3 Offset[80];

	bool InChain;

	int PreviousX, PreviousY; float PreviousZ;
	int Length;

	float Width, Height, Depth;
};

#endif