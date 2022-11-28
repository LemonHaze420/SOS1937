// $Header$

// $Log$
// Revision 1.1  2000-02-29 08:44:40+00  jjs
// Added Header and Log lines
//

/* Alphabet file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#ifndef __ALPHABET_H__
#define __ALPHABET_H__

#include "ConsoleFrontLib.h"

class Alphabet
{
public:
	Alphabet(char *Name, float Scale);
	~Alphabet();
		
	// The letters in the alphabet
	model *Letter[256];

	// The width of the letter
	float Kerning[256];

	// The Height of the alphabet
	float Height;
private:
	// One model per letter.
	object *FirstLetter;

};

#endif __ALPHABET_H__