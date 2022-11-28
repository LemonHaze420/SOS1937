// $Header

// $Log

/* Locale word stuff.
	Pass in the index and get it translated.

	Ian Gledhill for Broadsword Interactive Ltd.
	19/04/2000 */

#include "../SOS1937/GlobalStructure.h"

#include "../SOS1937/LocaleTexts.h"

extern int reallanguage;

char *TranslateLocale(int Index)
{
	switch (reallanguage)
	{
	case LG_ENGLISH:
		return LocaleTexts0[Index];
		break;
	case LG_AMERICAN:
		return LocaleTexts1[Index];
		break;
	case LG_GERMAN:
		return LocaleTexts2[Index];
		break;
	case LG_FRENCH:
		return LocaleTexts3[Index];
		break;
	case LG_SPANISH:
		return LocaleTexts4[Index];
		break;
	default:
		return "";
	}
}