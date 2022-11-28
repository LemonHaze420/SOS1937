/* Car details procedures... */

#include "ConsoleFrontLib.h"

#include "CarDetails.h"

// Convert from a car name to a car description name.
void ConvertCarToDescription(char *CarName, char *DescriptionName)
{
	if (   !strnicmp("AlfaP3", CarName, 6)		|| !strnicmp("Alfa12C", CarName, 7) || !strnicmp("Bugatti59", CarName, 9)
		|| !strnicmp("Duesenberg", CarName, 10) || !strnicmp("Miller", CarName, 6))
	{
		if (strchr(CarName, '_'))
		{
			strncpy(DescriptionName, CarName, strchr(CarName, '_')-CarName);
			DescriptionName[strchr(CarName, '_')-CarName] = 0;
		}
		else
			strcpy(DescriptionName, CarName);
		return;
	}
	if (!strnicmp("AlfaBiMotore", CarName, 12)) { strcpy(DescriptionName, "AlfaBim"); return; }
	if (!strnicmp("AutoUnionC", CarName, 10)) { strcpy(DescriptionName, "AutoC"); return; }
	if (!strnicmp("AutoUnionD", CarName, 10)) { strcpy(DescriptionName, "AutoD"); return; }
	if (!strnicmp("BonnerSpecial", CarName, 13)) { strcpy(DescriptionName, "Bonner"); return; }
	if (!strnicmp("Bugatti35", CarName, 9)) { strcpy(DescriptionName, "Bugatti35B"); return; }
	if (!strnicmp("ERARemus", CarName, 8)) { strcpy(DescriptionName, "ERA"); return; }
	if (!strnicmp("Mercedes125", CarName, 11)) { strcpy(DescriptionName, "Merc125"); return; }
	if (!strnicmp("Mercedes154", CarName, 11)) { strcpy(DescriptionName, "Merc154"); return; }
	if (!strnicmp("NapierRailton", CarName, 13)) { strcpy(DescriptionName, "Napier"); return; }
	if (!strnicmp("WatsonFlyer", CarName, 11)) { strcpy(DescriptionName, "Watson"); return; }

	*DescriptionName = 0;
	return;
}

void ConvertShortToLong(char *ShortName, char *LongName)
{
	if (!strnicmp(ShortName, "AlfaP3",6))			strcpy(LongName, "Alfa Romeo P3");
	if (!strnicmp(ShortName, "Alfa12C",7))			strcpy(LongName, "Alfa Romeo 12C");
	if (!strnicmp(ShortName, "AlfaBiMotore",12))	strcpy(LongName, "Alfa Romeo Bimotore");
	if (!strnicmp(ShortName, "AutounionC",10))		strcpy(LongName, "Auto Union C");
	if (!strnicmp(ShortName, "AutounionD",10))		strcpy(LongName, "Auto Union D");
	if (!strnicmp(ShortName, "AutounionSL",11))		strcpy(LongName, "Auto Union Streamlined");
	if (!strnicmp(ShortName, "BonnerSpecial",13))	strcpy(LongName, "Bonner Special");
	if (!strnicmp(ShortName, "Bugatti35",9))		strcpy(LongName, "Bugatti 35B");
	if (!strnicmp(ShortName, "Bugatti59",9))		strcpy(LongName, "Bugatti 59");
	if (!strnicmp(ShortName, "Duesenberg",10))		strcpy(LongName, "Duesenberg");
	if (!strnicmp(ShortName, "ERARemus",8))			strcpy(LongName, "ERA Remus");
	if (!strnicmp(ShortName, "Mercedes125",11))		strcpy(LongName, "Mercedes-Benz 125");
	if (!strnicmp(ShortName, "Mercedes154",11))		strcpy(LongName, "Mercedes-Benz 154");
	if (!strnicmp(ShortName, "MercedesSL",10))		strcpy(LongName, "Mercedes-Benz Streamlined");
	if (!strnicmp(ShortName, "Miller",6))			strcpy(LongName, "Miller");
	if (!strnicmp(ShortName, "NapierRailton",13))	strcpy(LongName, "Napier-Railton");
	if (!strnicmp(ShortName, "WatsonFlyer",11))		strcpy(LongName, "Watson Flyer");
}

int GetRandomCar(int DifficultyLevel)
{
	int ID=rand()%23;
	switch (DifficultyLevel)
	{
	case PDS_EASY:
		return (int)(EasyCars[ID]+1);
	case PDS_MEDIUM:
		return (int)(MediumCars[ID]+1);
	case PDS_HARD:
		return (int)(HardCars[ID]+1);
	default:
		return -1;
	}
}

int GetPositionFromTime(int Track, int Time)
{
	// Return last if impossible time.
	if (Time < 100) return 5;
	
	int BaseTimes[9] =	 {15500,  5300,  8200,  3100,  20400, 11000,  6600,  9800, 19400};
	int QualifyGaps[9] = {  160,    90,   120,    80,    200,   145,   100,   120,   200};

	int Threshold=BaseTimes[Track];

	for (int Count=0 ; Count<5 ; Count++)
	{
		if (Time < Threshold) return Count;
		Threshold += QualifyGaps[Track];
	}
	return 5;
}
