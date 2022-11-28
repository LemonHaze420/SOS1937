// $Header$

// $Log$
// Revision 1.26  2000-05-06 13:48:47+01  img
// Sean build
//
// Revision 1.25  2000-05-04 17:39:31+01  img
// Build 18
//
// Revision 1.24  2000-04-26 10:08:51+01  img
// <>
//
// Revision 1.23  2000-04-25 11:50:03+01  img
// Bug fixes
//
// Revision 1.22  2000-04-22 18:26:07+01  img
// Added C button to joystick support
//
// Revision 1.21  2000-04-19 21:38:03+01  img
// Localisation.
//
// Revision 1.20  2000-04-13 12:46:26+01  img
// six cars max
//
// Revision 1.19  2000-04-13 12:46:06+01  img
// Difficulty level used
//
// Revision 1.18  2000-04-07 14:38:10+01  img
// Help support, generic background support
//
// Revision 1.17  2000-03-30 14:48:26+01  img
// More preloadings
//
// Revision 1.16  2000-03-28 17:56:34+01  img
// Championship code.
//
// Revision 1.15  2000-03-27 10:26:30+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.14  2000-03-20 18:00:56+00  img
// Beta build
//
// Revision 1.13  2000-03-13 18:28:18+00  img
// B Button works again.
//
// Revision 1.12  2000-03-13 18:16:39+00  img
// Bug fixes
//
// Revision 1.11  2000-03-09 11:40:29+00  img
// Now with arrows!
//
// Revision 1.10  2000-03-08 10:31:30+00  img
// Difficulty level on this screen now
//
// Revision 1.9  2000-03-07 14:01:51+00  img
// Proper deletion of models
//
// Revision 1.8  2000-03-06 13:03:15+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.7  2000-03-06 10:39:48+00  img
// Icons displayed properly and rotating
//
// Revision 1.6  2000-03-02 14:26:38+00  img
// Controller screen now working
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
extern int AllControllerTypes[];

RaceTypeScreen::RaceTypeScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	Difficulty[PDS_EASY] = new TextWord(TranslateLocale(61), MainAlphabet);
	Difficulty[PDS_MEDIUM] = new TextWord(TranslateLocale(62), MainAlphabet);
	Difficulty[PDS_HARD] = new TextWord(TranslateLocale(63), MainAlphabet);

	DifficultyLevel = MainGameInfo.Difficulty;
	
	ShowDifficulty();
	
	RaceTypeArray[RT_CHAMPIONSHIP] = new TextWord(TranslateLocale(40), MainAlphabet);
	RaceTypeArray[RT_QUICK] = new TextWord(TranslateLocale(41), MainAlphabet);
	RaceTypeArray[RT_SCENARIO] = new TextWord(TranslateLocale(42), MainAlphabet);
	RaceTypeArray[RT_SINGLE] = new TextWord(TranslateLocale(43), MainAlphabet);
	
	// Reset the counter to zero.
	Counter = 0;

	CurrentRaceType = MainGameInfo.RaceType;
	
	RaceTypeArray[CurrentRaceType]->Write(400,110,1200.0f);

	DrawRaceTypeIcon();

	LeftArrow->Draw(285,115,600.0f);
	RightArrow->Draw(515,115,600.0f);
	UpArrow->Draw(400,135,800.0f);
	DownArrow->Draw(400,165,800.0f);

	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(68), TranslateLocale(69));

	// Use the default background.
	UseDefaultBackdrop = true;
}

void RaceTypeScreen::HideDifficulty()
{
	Difficulty[DifficultyLevel]->Hide();
}

void RaceTypeScreen::ShowDifficulty()
{
	Difficulty[DifficultyLevel]->Write(400,145,1200.0f);
}

void RaceTypeScreen::Update(struct Instruction *ScreenCommand)
{
	if (RaceTypeIcon)
		RaceTypeIcon->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f + (float)sin(Counter*0.013f)*3.0f,0.0f);

	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

void RaceTypeScreen::DrawRaceTypeIcon()
{
	RaceTypeIcon = RaceTypeModels[CurrentRaceType];
	if (RaceTypeIcon)
	{
		RaceTypeIcon->Draw(400,300,300.0f);
		RaceTypeIcon->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f + (float)sin(Counter*0.013f)*3.0f,0.0f);
	}
}

extern int GetRandomCar(int DifficultyLevel);

int RaceTypeScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_RIGHT && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL || ControlPacket & 1 << CON_X && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		RaceTypeArray[CurrentRaceType]->Hide();
		RaceTypeIcon->Hide();
		CurrentRaceType++;
		if (CurrentRaceType > RT_SINGLE) CurrentRaceType = RT_CHAMPIONSHIP;
		if (CurrentRaceType == RT_CHAMPIONSHIP && !strnicmp(CarNumbers[MainGameInfo.CarTypes[0]-1], "Napier", 6)) CurrentRaceType++;
		RaceTypeArray[CurrentRaceType]->Write(400,110,1200.0f);

		DrawRaceTypeIcon();
	}

	if (ControlPacket & 1 << CON_LEFT && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL || ControlPacket & 1 << CON_Y && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		RaceTypeArray[CurrentRaceType]->Hide();
		RaceTypeIcon->Hide();
		CurrentRaceType--;
		if (CurrentRaceType == RT_CHAMPIONSHIP && !strnicmp(CarNumbers[MainGameInfo.CarTypes[0]-1], "Napier", 6)) CurrentRaceType--;
		if (CurrentRaceType < RT_CHAMPIONSHIP) CurrentRaceType = RT_SINGLE;
		RaceTypeArray[CurrentRaceType]->Write(400,110,1200.0f);

		DrawRaceTypeIcon();
	}

	if ((AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL && ControlPacket & 1 << CON_THROTTLE || ControlPacket & 1 << CON_DPAD_UP
		|| (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL && ControlPacket & 1 << CON_RIGHT)
	{
		HideDifficulty();
		if (++DifficultyLevel > PDS_HARD) DifficultyLevel = PDS_EASY;
		ShowDifficulty();
	}

	if ((AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL && ControlPacket & 1 << CON_BRAKE || ControlPacket & 1 << CON_DPAD_DOWN
		|| (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL && ControlPacket & 1 << CON_LEFT)
	{
		HideDifficulty();
		if (--DifficultyLevel < PDS_EASY) DifficultyLevel = PDS_HARD;
		ShowDifficulty();
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		MainGameInfo.RaceType = CurrentRaceType;
		MainGameInfo.Difficulty = DifficultyLevel;
		RaceTypeIcon = RaceTypeModels[CurrentRaceType];

		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';

		if (CurrentRaceType == RT_CHAMPIONSHIP)
		{
			if (GlobalTrack->GetNumber() != T_TRIPOLI)
			{
				delete GlobalTrack;
				GlobalTrack = NULL;
				MainGameInfo.Track = T_TRIPOLI;
			}
		}
		if (CurrentRaceType == RT_QUICK || CurrentRaceType == RT_SINGLE)
		{
			for (int i=1 ; i<10 ; i++)
				MainGameInfo.CarTypes[i] = 0;
			
			int NumberOfCars= rand()%5 + 2;
			for (i=1 ; i<NumberOfCars ; i++)
			{
				bool TryAgain = true;
				while (TryAgain)
				{
					TryAgain = false;
					MainGameInfo.CarTypes[i] = GetRandomCar(MainGameInfo.Difficulty);//rand()%99 +1;
					for ( int j=0 ; j<NumberOfCars ; j++)
					{
						if (i!=j)
						{
							if (MainGameInfo.CarTypes[i] == MainGameInfo.CarTypes[j])
								TryAgain = true;
						}
					}
					if (!*CarNumbers[MainGameInfo.CarTypes[i]-1]) TryAgain = true;
				}
			}
//			MainGameInfo.Track = rand()%9;
		}
		
		if (CurrentRaceType == RT_SCENARIO)
		{
			QueuedValue = (Screen *) new ChooseScenarioScreen(MainAlphabet);
		}
		else
		{
			MainMenu->Unselect(RACE_TYPE);
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
			((FrontScreen *)QueuedValue)->SetHighlighted(RACE_TYPE);
		}
	}

	if (ControlPacket & 1 << CON_B)
	{
		Destroy();
		QueuedCommand = 'S';
		MainMenu->Unselect(RACE_TYPE);
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
		((FrontScreen *)QueuedValue)->SetHighlighted(RACE_TYPE);
	}

	return 0;
}

// Not needed as we're using the default stuff.
void RaceTypeScreen::DrawBackground()
{
}

RaceTypeScreen::~RaceTypeScreen()
{
}

void RaceTypeScreen::Destroy()
{
	for (int i=0 ; i<=RT_SINGLE ; i++)
	{
		delete RaceTypeArray[i];
	}
		
	RaceTypeIcon->Hide();

	for (i=PDS_EASY ; i<=PDS_HARD ; i++)
	{
		delete Difficulty[i];
	}

	LeftArrow->Hide();	RightArrow->Hide();
	UpArrow->Hide();	DownArrow->Hide();

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	DeleteHelp();

	return;
}