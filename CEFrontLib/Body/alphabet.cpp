// $Header$

// $Log$
// Revision 1.11  2000-05-06 08:15:39+01  jjs
// Changed kerning by .1 for i.
//
// Revision 1.10  2000-05-05 12:17:06+01  img
// Build 19
//
// Revision 1.9  2000-04-28 11:09:09+01  jcf
// <>
//
// Revision 1.8  2000-04-21 19:11:04+01  img
// Added ampersand
//
// Revision 1.7  2000-04-19 21:37:55+01  img
// Localisation.
//
// Revision 1.6  2000-04-08 12:37:44+01  img
// Napier-Railton
//
// Revision 1.5  2000-04-07 14:37:01+01  img
// Support for punctuation
//
// Revision 1.4  2000-03-20 18:00:39+00  img
// Beta build
//
// Revision 1.3  2000-03-02 09:49:51+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.2  2000-02-29 08:41:41+00  jjs
// <>
//

/* Alphabet file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#include "ConsoleFrontLib.h"

#include <stdio.h>

#define ADDLETTER(name,number)	sprintf(ModelName, "%s%s", AlphabetName, name); \
						Letter[(unsigned char)number] = getModelPtr( ModelName, Scale, false, NULL); \
						Kerning[(unsigned char)number] = (Letter[(unsigned char)number]->boundingBox.hi.x - Letter[(unsigned char)number]->boundingBox.lo.x);

Alphabet::Alphabet(char *Name, float Scale)
{
	arcPush(Name);
	char ModelName[38];
	char AlphabetName[50];
	char *NameStart;
	int Length;

	NameStart = Name+strlen(Name);
	while (NameStart > Name && *--NameStart != '\\');
	strncpy(AlphabetName, ++NameStart, Length = (strchr(NameStart, '.') - NameStart));
	AlphabetName[Length] = 0;

	Height = 0;

	for (int i=0 ; i<256 ; i++)
	{
		Letter[i] = NULL;
		Kerning[i] = 0.0f;
	}

	for (i='a' ; i<= 'z' ; i++)
	{
		sprintf(ModelName, "%s%c", AlphabetName, i);
		Letter[i-32] = getModelPtr( ModelName, Scale, false, NULL);
		Kerning[i-32] = (i=='i' ? 2.0f : 1) * (Letter[i-32]->boundingBox.hi.x - Letter[i-32]->boundingBox.lo.x) * 1.0f;
		// If the height of this letter is bigger than the next biggest, set that height.
		if (Height < Letter[i-32]->boundingBox.hi.y)
			Height = Letter[i-32]->boundingBox.hi.y;
	}

	for (i='0' ; i<='9' ; i++)
	{
		sprintf(ModelName, "%s%c", AlphabetName, i);
		Letter[i] = getModelPtr( ModelName, Scale, false, NULL);
		Kerning[i] = (Letter[i]->boundingBox.hi.x - Letter[i]->boundingBox.lo.x);

		if (Height < Letter[i]->boundingBox.hi.y)
			Height = Letter[i]->boundingBox.hi.y;
	}

	ADDLETTER("Ampersand",'&');
	ADDLETTER("Exclamation",'!');
	ADDLETTER("ExclamationFlipped",'¡');
	ADDLETTER("Stop",'.');
	ADDLETTER("Comma",',');
	ADDLETTER("Apostrophe",'\'');
	ADDLETTER("Question",'?');
	ADDLETTER("QuestionFlipped",'¿');
	ADDLETTER("Hyphen",'-');
	ADDLETTER("Colon",':');

	ADDLETTER("AAcute",'á');
	ADDLETTER("AGrave",'à');
	ADDLETTER("ACircumflex",'â');
	ADDLETTER("AUmlaut",'ä');

	ADDLETTER("CCedilla",'ç');

	ADDLETTER("EAcute",'é');
	ADDLETTER("EGrave",'è');
	ADDLETTER("ECircumflex",'ê');
	ADDLETTER("EUmlaut",'ë');

	ADDLETTER("IGrave",'ì');
	ADDLETTER("IAcute",'í');
	ADDLETTER("ICircumflex",'î');
	ADDLETTER("IUmlaut",'ï');

	ADDLETTER("NTilda",'ñ');

	ADDLETTER("OAcute",'ó');
	ADDLETTER("OGrave",'ò');
	ADDLETTER("OCircumflex",'ô');
	ADDLETTER("OUmlaut",'ö');

	ADDLETTER("UAcute",'ú');
	ADDLETTER("UGrave",'ù');
	ADDLETTER("UUmlaut",'ü');

	ADDLETTER("AEDipthong",'æ');
	ADDLETTER("OEDipthong",'œ');

	arcPop();
}

Alphabet::~Alphabet()
{
	for (int i=0 ; i<256 ; i++)
	{
		if (Letter[i])
			delete Letter[i];
	}
}