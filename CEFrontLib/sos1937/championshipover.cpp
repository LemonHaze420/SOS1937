// $Header$

// $Log$
// Revision 1.4  2000-05-04 17:39:25+01  img
// Build 18
//
// Revision 1.3  2000-04-22 18:26:02+01  img
// Added C button to joystick support
//
// Revision 1.2  2000-04-19 21:37:58+01  img
// Localisation.
//
// Revision 1.1  2000-04-07 14:38:03+01  img
// Help support, generic background support
//
// Revision 1.0  2000-04-06 14:48:25+01  img
// Initial revision
//
// Revision 1.5  2000-02-29 11:28:22+00  jjs
// Added Header and Log lines.
//

/* The race type screen of the game.

	Ian Gledhill 26/01/2000 :-)
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <archive.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

ChampionshipOverScreen::ChampionshipOverScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	UpdateTable();
	DrawResult();

	ScreenHeaders[HEADER_CHAMPIONSHIP]->Draw();

	// Reset the counter to zero.
	Counter = 0;

//	SetHelpText("PRESS 'A' OR 'B' TO RETURN TO MAIN MENU"," ");
	SetHelpText(TranslateLocale(2)," ");

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ChampionshipOverScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ChampionshipOverScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_B || ControlPacket & 1 << CON_C)
	{
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(RACE_TYPE);
	}

	return 0;
}

void ChampionshipOverScreen::UpdateTable()
{
	int NumberOfCars = 0;

	for (int i=0 ; MainGameInfo.CarTypes[i] ; i++)
	{
		switch (MainGameInfo.Positions[i])
		{
		case 0:
			MainGameInfo.ChampionshipPoints[i]+=10;
			break;
		case 1:
			MainGameInfo.ChampionshipPoints[i]+=6;
			break;
		case 2:
			MainGameInfo.ChampionshipPoints[i]+=3;
			break;
		case 3:
			MainGameInfo.ChampionshipPoints[i]+=1;
			break;
		case 4:
			MainGameInfo.ChampionshipPoints[i]+=0;
			break;
		case 5:
			MainGameInfo.ChampionshipPoints[i]+=0;
			break;
		}
		NumberOfCars++;
	}

	for (i = 0 ; i<NumberOfCars ; i++)
	{
		MainGameInfo.ChampionshipPosition[i] = NumberOfCars-1;

		for (int j=0 ; j<NumberOfCars ; j++)
		{
			if (i != j && MainGameInfo.ChampionshipPoints[i] > MainGameInfo.ChampionshipPoints[j])
				MainGameInfo.ChampionshipPosition[i]--;
		}
	}

	for (i=0 ; i<NumberOfCars ; i++)
	{
		for (int j=0 ; j<NumberOfCars ; j++)
		{
			if (i > j && MainGameInfo.ChampionshipPosition[i] == MainGameInfo.ChampionshipPosition[j])
			{
				MainGameInfo.ChampionshipPosition[i]--;
				j = -1;
			}
		}
	}
}

void ChampionshipOverScreen::DrawResult()
{
	CurrentScenario=NULL;
	for (int i=0 ; i<15 && !CurrentScenario && ChampionshipScenarios[MainGameInfo.Difficulty][i] ; i++)
	{
		// If the scenario's car type is equal to the player's chosen car.
		if (!stricmp(CarNumbers[ChampionshipScenarios[MainGameInfo.Difficulty][i]->Car[0]-1], CarNumbers[MainGameInfo.CarTypes[0]-1]))
		{
			// This is our scenario!
			CurrentScenario = ChampionshipScenarios[MainGameInfo.Difficulty][i];
		}
	}
	
	char *Pointer;
	
	if (MainGameInfo.ChampionshipPosition[0] <= CurrentScenario->SuccessValue-1)
		Pointer = CurrentScenario->SuccessString;
	else
		Pointer = CurrentScenario->FailureString;

	int Length;
	char Lines[5][40];

	for (i=0 ; i<5 ; i++)
	{
		*Lines[i] = 0;
		if (*Pointer && !i || *(Pointer-1) && i)
		{
			Length = 0;
			for (int j=0 ; j<36 ; j++)
			{
				if (*(Pointer+j) == ' ')
					Length = j;
				if (*(Pointer+j) == 0)
				{
					Length = j;
					j = 60;
				}
			}
			// Length now holds the length up to the last space before the 23rd character.
			strncpy(Lines[i], Pointer, Length);
			Lines[i][Length] = 0;
			Pointer += Length+1;
		}
	}
	for (i=0 ; i<5 ; i++)
	{
		ScenarioResult[i] = NULL;
		if (*Lines[i])
		{
			ScenarioResult[i] = new TextWord(Lines[i], MainAlphabet);
			ScenarioResult[i]->Justify(JUSTIFY_LEFT);
			ScenarioResult[i]->Write(200,180+i*24, 1200.0f);
		}
	}
}

// Not needed as we're using the default stuff.
void ChampionshipOverScreen::DrawBackground()
{
}

ChampionshipOverScreen::~ChampionshipOverScreen()
{
}

void ChampionshipOverScreen::Destroy()
{
	ScreenHeaders[HEADER_CHAMPIONSHIP]->Hide();

	DeleteHelp();

	for (int i=0 ; i<5 ; i++)
		delete ScenarioResult[i];
	return;
}
