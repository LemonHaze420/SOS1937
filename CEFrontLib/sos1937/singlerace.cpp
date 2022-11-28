// $Header$

// $Log$
// Revision 1.18  2000-05-10 14:33:03+01  img
// RC4
//
// Revision 1.17  2000-05-05 12:17:19+01  img
// Build 19
//
// Revision 1.16  2000-05-04 17:39:33+01  img
// Build 18
//
// Revision 1.15  2000-04-24 20:27:35+01  img
// Bug fixes
//
// Revision 1.14  2000-04-22 18:26:09+01  img
// Added C button to joystick support
//
// Revision 1.13  2000-04-20 16:05:22+01  img
// Qualification works
//
// Revision 1.12  2000-04-19 21:38:05+01  img
// Localisation.
//
// Revision 1.11  2000-04-08 12:51:00+01  img
// Bug fix in menu
//
// Revision 1.10  2000-04-07 14:38:15+01  img
// Help support, generic background support
//
// Revision 1.9  2000-04-05 17:46:32+01  img
// Start grid
//
// Revision 1.8  2000-04-04 18:40:29+01  img
// Better handling of ReEntryPoint
//
// Revision 1.7  2000-03-30 14:48:27+01  img
// More preloadings
//
// Revision 1.6  2000-03-21 17:28:29+00  img
// Pre-loading more now...
//
// Revision 1.5  2000-03-07 15:16:34+00  img
// Deletes menu in Destroy()
//
// Revision 1.4  2000-03-07 14:02:17+00  img
// Cancel button added
//
// Revision 1.3  2000-03-06 13:03:17+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.2  2000-02-29 11:28:28+00  jjs
// Added Header and Log lines.
//

/* The Single Race screen of the game.

	Ian Gledhill 16/02/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

extern class ButtonBank *MainMenu;

extern int GetPositionFromTime(int Track, int Time);

SingleRaceScreen::SingleRaceScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	delete MainMenu;

	// Change the button set.
	MainMenu = new ButtonBank();
	
	MainMenu->Add(SingleRaceABank[0]);
	MainMenu->Add(SingleRaceABank[1]);

	MainMenu->DrawAll();

	Highlighted = SR_PRACTICE;
	MainMenu->Highlight(Highlighted);

	LoadMiniCars();

	if (ReEntryPoint == REP_SINGLERACEOVER)
	{
		if (MainGameInfo.RaceMode == RM_QUALIFY)
		{
			MainGameInfo.QualifiedPosition[0] = GetPositionFromTime(MainGameInfo.Track, MainGameInfo.LapTime[0]);
			for (int i=0 ; MainGameInfo.CarTypes[i] ; i++);
			if (MainGameInfo.QualifiedPosition[0] > i-1) MainGameInfo.QualifiedPosition[0] = i-1;
			
			for (i=1 ; i<10 ; i++)
			{
				if ( i<=MainGameInfo.QualifiedPosition[0] )
					MainGameInfo.QualifiedPosition[i] = i-1;
				else
					MainGameInfo.QualifiedPosition[i] = i;
			}
		}
	}
	else
	{
		for (int i=1 ; i<10 ; i++)
			MainGameInfo.QualifiedPosition[i] = i-1;
		for (i=0 ; MainGameInfo.CarTypes[i] ; i++);
		MainGameInfo.QualifiedPosition[0] = i-1;
	}
	
	
	DrawStartGrid();

	// Reset the counter to zero.
	Counter = 0;

	ScreenHeaders[HEADER_SINGLERACE]->Draw();

	SetStartState(true);

	SetHelpText(TranslateLocale(75),TranslateLocale(76));

	// Use the default background.
	UseDefaultBackdrop = true;
}

void SingleRaceScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int SingleRaceScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_THROTTLE)
	{
		MainMenu->Lowlight(Highlighted);
		Highlighted--;
		if (Highlighted < SR_PRACTICE) Highlighted = SR_QUALIFY;
		MainMenu->Highlight(Highlighted);
	}

	if (ControlPacket & 1 << CON_BRAKE)
	{
		MainMenu->Lowlight(Highlighted);
		Highlighted++;
		if (Highlighted > SR_QUALIFY) Highlighted = SR_PRACTICE;
		MainMenu->Highlight(Highlighted);
	}

	if (ControlPacket & 1 << CON_GLANCE || ControlPacket & 1 << CON_C)
	{
		MainMenu->Lowlight(Highlighted);
		Destroy();
		switch (Highlighted)
		{
		case SR_PRACTICE:
			MainGameInfo.RaceMode = RM_PRACTICE;
			break;
		case SR_QUALIFY:
			MainGameInfo.RaceMode = RM_QUALIFY;
			break;
		}
		ReEntryPoint = REP_SINGLERACEOVER;
		
		return 1;
	}

	if (ControlPacket & 1 << CON_START)
	{
		if (MainGameInfo.Track == T_TRIPOLI) MainGameInfo.Laps = 3;
		if (MainGameInfo.Track == T_AVUS) MainGameInfo.Laps = 4;
		if (MainGameInfo.Track == T_MONTANA) MainGameInfo.Laps = 10;
		if (MainGameInfo.Track == T_ROOSEVELT) MainGameInfo.Laps = 6;
		if (MainGameInfo.Track == T_MONTLHERY) MainGameInfo.Laps = 3;
		if (MainGameInfo.Track == T_PAU) MainGameInfo.Laps = 8;
		if (MainGameInfo.Track == T_DONINGTON) MainGameInfo.Laps = 6;
		if (MainGameInfo.Track == T_BROOKLANDS) MainGameInfo.Laps = 8;
		if (MainGameInfo.Track == T_MONZA) MainGameInfo.Laps = 3;
		MainMenu->Lowlight(Highlighted);
		Destroy();
		MainGameInfo.RaceMode = RM_RACE;
		ReEntryPoint = REP_SINGLERACEOVER;
		return 1;
	}

	if (ControlPacket & 1 << CON_B)
	{
		MainMenu->Lowlight(Highlighted);
		Destroy();

		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
	}

	return 0;
}

extern void ConvertCarToDescription(char *CarName, char *DescriptionName);
extern void ConvertShortToLong(char *, char *);

void SingleRaceScreen::LoadMiniCars()
{
//	arcPush(DISKNAME "\\SOS1937FrontEnd\\GridModels.tc");
	char ShortName[25];

	for (int i=0 ; i<10 ; i++)
	{
		if (MainGameInfo.CarTypes[i])
		{
			ConvertCarToDescription(CarNumbers[MainGameInfo.CarTypes[i]-1], ShortName);		
			strcat(ShortName, "Grid");
			MiniCar[i] = new FrontEndObject(NULL, ShortName);
		}
		else
			MiniCar[i] = NULL;
	}

	char LongName[40];

	for (i=0 ; i<10 && MiniCar[i] ; i++)
	{
		ConvertShortToLong(CarNumbers[MainGameInfo.CarTypes[i]-1], LongName);		
		if (i)
			CarName[i] = new TextWord(LongName, MainAlphabet);
		else
			CarName[i] = new TextWord(LongName, YellowAlphabet);
	}
//	arcPop();
}

void SingleRaceScreen::DrawStartGrid()
{
	int NumberOfCars=0;
	for (int i=0 ; i<10 && MiniCar[i] ; i++);
	NumberOfCars=i;
	int x,y;
	int Position;

	for (i=0 ; i<NumberOfCars ; i++)
	{
		Position = MainGameInfo.QualifiedPosition[i];
		x = 210+ (int)(Position/2)*153 + (Position%2 ? 35 : 0);
		y = 200+ (Position%2)*100;
		MiniCar[i]->Draw( x,y, 800.0f );
		CarName[i]->Write( x,y+23,1800.0f );
	}
}

void SingleRaceScreen::HideStartGrid()
{
	for (int i=0 ; i<10 && MiniCar[i] ; i++)
	{
		MiniCar[i]->Hide();
		CarName[i]->Hide();
	}
}

// Not needed as we're using the default stuff.
void SingleRaceScreen::DrawBackground()
{
	DrawFullScreen2D((DWORD *)SingleRaceBackdrop, true);
}

SingleRaceScreen::~SingleRaceScreen()
{
}

void SingleRaceScreen::Destroy()
{
	for (int i=0 ; MainGameInfo.CarTypes[i] ; i++)
	{
		delete MiniCar[i];
		delete CarName[i];
	}
	
	delete MainMenu;
	MainMenu = NULL;

	ScreenHeaders[HEADER_SINGLERACE]->Hide();

	DeleteHelp();

	SetStartState(false);

	return;
}