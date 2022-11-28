// $Header

// $Log

/* The screen to display after a single race.

	Ian Gledhill 07/03/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <archive.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

extern void ConvertShortToLong(char *ShortName, char *LongName);

SingleRaceOverScreen::SingleRaceOverScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	// Reset the counter to zero.
	Counter = 0;
	
	RaceOver = new TextWord(TranslateLocale(77), MainAlphabet);
	RaceOver->Write(400,140,600.0f);

	for (int i=0 ; i<10 ; i++)
		CarInTable[i] = NULL;

	LoadStandingsTable();
	DrawStandings();

	ScreenHeaders[HEADER_SINGLERACE]->Draw();

	SetHelpText(TranslateLocale(78),TranslateLocale(79));

	SetStartState(true);

	// Use the default background.
	UseDefaultBackdrop = true;
}

void SingleRaceOverScreen::Update(struct Instruction *ScreenCommand)
{
	if (Counter > -1)
	{
		Counter+=1;
		RaceOver->Rotate(0,(Counter >= 180 ? Counter - 180 : Counter)-90,0);
	}

	if (Counter > 360) Counter -= 180;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int SingleRaceOverScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_B || ControlPacket & 1 << CON_C)
	{
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
	}

	if (ControlPacket & 1 << CON_START)
	{
		Destroy();

		return 1;
	}
	return 0;
}

extern void ConvertCarToDescription(char *CarName, char *DescriptionName);

void SingleRaceOverScreen::LoadStandingsTable()
{
	char IconName[50], Buffer[50], Buffer2[50];

	for (int i=0 ; i<10 ; i++)
	{
		if (MainGameInfo.CarTypes[i])
		{
			ConvertCarToDescription(CarNumbers[MainGameInfo.CarTypes[i]-1], Buffer);
			sprintf(IconName, "%sSide", Buffer);
			CarInTable[i] = new FrontEndObject(NULL, IconName);
			CarInTable[i]->Justify(JUSTIFY_LEFT);
		}
		else CarInTable[i] = NULL;
	}

	for (i=0 ; i<10 && CarInTable[i] ; i++)
	{
		sprintf(Buffer, "%d", MainGameInfo.Positions[i]+1);
		PositionWord[i] = new TextWord(Buffer, MainAlphabet);
		PositionWord[i]->Justify(JUSTIFY_RIGHT);
		
		sprintf(Buffer, "%d", MainGameInfo.CarTypes[i]);
		CarNumber[i] = new TextWord(Buffer, MainAlphabet);
		CarNumber[i]->Justify(JUSTIFY_LEFT);

		strcpy(Buffer, CarNumbers[MainGameInfo.CarTypes[i]-1]);
		ConvertShortToLong(Buffer, Buffer2);
		CarType[i] = new TextWord(Buffer2, MainAlphabet);
		CarType[i]->Justify(JUSTIFY_LEFT);

		DriverName[i] = NULL;
	}
}

void SingleRaceOverScreen::DrawStandings()
{
	int x,y;
	for (int i=0 ; i<10 ; i++)
	{
		if (CarInTable[i])
		{
			x = 210;
			y = 180+MainGameInfo.Positions[i]*20;

			PositionWord[i]->Write(x,y, 1200.0f);
			CarInTable[i]->Draw(x+15,y+3, 1000.0f);
			CarNumber[i]->Write(x+88,y, 1200.0f);
			CarType[i]->Write(x+115,y,  1200.0f);
		}
	}
}


// Not needed as we're using the default stuff.
void SingleRaceOverScreen::DrawBackground()
{
}

SingleRaceOverScreen::~SingleRaceOverScreen()
{
}

void SingleRaceOverScreen::Destroy()
{
	delete RaceOver;
	Counter = -1;
		
	for (int i=0 ; i<10 ; i++)
	{
		if (CarInTable[i])
		{
			delete CarInTable[i];
			delete PositionWord[i];
			delete CarNumber[i];
			delete CarType[i];
		}
	}

	ScreenHeaders[HEADER_SINGLERACE]->Hide();

	DeleteHelp();

	SetStartState(false);

	return;
}