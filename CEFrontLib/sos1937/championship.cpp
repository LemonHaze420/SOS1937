// $Header$

// $Log$
// Revision 1.27  2000-05-12 12:12:12+01  img
// Practice/Qualify strings
//
// Revision 1.26  2000-05-12 11:02:14+01  img
// RC5?
//
// Revision 1.25  2000-05-11 17:53:36+01  jjs
// Hack to fix championship screen bug.
//
// Revision 1.24  2000-05-09 17:24:23+01  img
// RC3
//
// Revision 1.23  2000-05-09 14:07:57+01  img
// Release Candidate 2
//
// Revision 1.22  2000-05-06 13:48:36+01  img
// Sean build
//
// Revision 1.21  2000-05-05 18:12:56+01  img
// Build 19
//
// Revision 1.20  2000-05-05 12:17:09+01  img
// Build 19
//
// Revision 1.19  2000-05-04 17:39:23+01  img
// Build 18
//
// Revision 1.18  2000-05-02 17:03:37+01  img
// Build 17
//
// Revision 1.17  2000-04-23 20:48:39+01  img
// Bug fixes
//
// Revision 1.16  2000-04-22 18:26:00+01  img
// Added C button to joystick support
//
// Revision 1.15  2000-04-20 16:05:20+01  img
// Qualification works
//
// Revision 1.14  2000-04-19 21:37:58+01  img
// Localisation.
//
// Revision 1.13  2000-04-11 16:12:54+01  img
// Fixes in championship table
//
// Revision 1.12  2000-04-10 17:20:00+01  img
// Corrected bug in initial table
//
// Revision 1.11  2000-04-07 15:28:59+01  img
// Clears help line after memory.
//
// Revision 1.10  2000-04-07 14:38:02+01  img
// Help support, generic background support
//
// Revision 1.9  2000-04-05 17:45:37+01  img
// Refined start grid
//
// Revision 1.8  2000-03-30 14:47:49+01  img
// More pre-loadings.
//
// Revision 1.7  2000-03-28 17:55:08+01  img
// Bug fixes
//
// Revision 1.6  2000-03-27 10:26:27+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.5  2000-03-22 09:29:08+00  img
// <>
//
// Revision 1.4  2000-03-21 17:28:22+00  img
// Pre-loading more now...
//
// Revision 1.3  2000-03-20 18:00:47+00  img
// Beta build
//
// Revision 1.2  2000-03-15 09:35:13+00  img
// renderReset() added.
//
// Revision 1.1  2000-03-10 17:24:38+00  img
// No graphics except menu
//
// Revision 1.0  2000-03-10 13:31:41+00  img
// Initial revision
//
// Revision 1.5  2000-02-29 11:28:22+00  jjs
// Added Header and Log lines.
//

/* The race type screen of the game.

	Ian Gledhill 26/01/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <archive.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

extern void ConvertCarToDescription(char *CarName, char *DescriptionName);
extern void ConvertShortToLong(char *, char *);
extern int GetPositionFromTime(int Track, int Time);

ChampionshipScreen::ChampionshipScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	ScreenName = CHAMPIONSHIP;

	delete MainMenu;

//	AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	MainMenu = new ButtonBank();

	for (int i=0 ; i<7 ; i++)
		MainMenu->Add(ChampionshipBank[i]);
	MainMenu->DrawAll();

	// Reset the counter to zero.
	Counter = 0;

	CurrentSelection = CS_STARTGRID;
	MainMenu->Highlight(CurrentSelection);

	if (MainGameInfo.RaceMode == RM_QUALIFY)
	{
		for (int i=0 ; MainGameInfo.CarTypes[i] ; i++);

		MainGameInfo.QualifiedPosition[0] = GetPositionFromTime(MainGameInfo.Track, MainGameInfo.LapTime[0]);
		if (MainGameInfo.QualifiedPosition[0] > i-1) MainGameInfo.QualifiedPosition[0] = i-1;

		for (i=1 ; i<10 ; i++)
		{
			if ( i<=MainGameInfo.QualifiedPosition[0] )
				MainGameInfo.QualifiedPosition[i] = i-1;
			else
				MainGameInfo.QualifiedPosition[i] = i;
		}
	}
	
	if (ReEntryPoint || MainGameInfo.RaceType == 0xFF) 
	{
		if (MainGameInfo.RaceMode != RM_QUALIFY && MainGameInfo.RaceMode != RM_PRACTICE)
		{
			UpdateTable();
			MainGameInfo.RaceType = RT_CHAMPIONSHIP;
		}
	}
	else
	{
		MainGameInfo.Track = T_TRIPOLI;
		for (i=1 ; i<10 ; i++)
		{
			MainGameInfo.ChampionshipPoints[i] = 0;
			MainGameInfo.ChampionshipPosition[i] = i-1;
			MainGameInfo.QualifiedPosition[i] = i-1;
		}
		MainGameInfo.ChampionshipPosition[0] = -1;
		MainGameInfo.QualifiedPosition[0] = -1;
		MainGameInfo.ChampionshipPoints[0] = 0;
	}

	LoadScenario();
	LoadMiniCars();
	LoadStandingsTable();
	LoadCar();
	LoadTrack();

	DrawStandings();
	HideStandings();

	ScreenHeaders[HEADER_CHAMPIONSHIP]->Draw();

	DrawCurrentScreen();

	SetStartState(true);
//	SetHelpText("USE DIRECTION PAD TO CHANGE INFORMATION","PRESS 'A' TO SELECT");
	dprintf("Help text: %s", TranslateLocale(0));
	dprintf("Help text: %s", TranslateLocale(1));
	SetHelpText(TranslateLocale(0), TranslateLocale(1));
	ChangeHelpLine(1,false);

	Confirming = false;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ChampionshipScreen::LoadCar()
{
	int CarIDs[15];
	
	CarIDs[0] = 12;		CarIDs[1] = 66;		CarIDs[2] = 8;
	CarIDs[3] = 1;		CarIDs[4] = 9;		CarIDs[5] = 10;
	CarIDs[6] = 7;		CarIDs[7] = 28;		CarIDs[8] = 3;
	CarIDs[9] = 22;		CarIDs[10] = 16;	CarIDs[11] = 15;
	CarIDs[12] = 2;		CarIDs[13] = 19;	CarIDs[14] = 5;
	
	for (int i=0 ; i<=15 && strnicmp(CarNumbers[CarIDs[i]-1], CarNumbers[MainGameInfo.CarTypes[0]-1], strchr(CarNumbers[CarIDs[i]-1], '_')-CarNumbers[CarIDs[i]-1]); i++);
	GlobalCar = CarObject = AllCars[i];
	CarObject->SetCarPosition(400,230,9.0f);
	CarObject->SetNamePosition(420,117,500.0f);
	CarObject->SetGraphPosition(425,365,700.0f);
}

void ChampionshipScreen::LoadTrack()
{
	delete TrackObject;
	TrackObject = new FrontEndTrack(MainGameInfo.Track);
	TrackObject->SetTrackPosition(395,230,18.0f);
	TrackObject->SetNamePosition(415,115,900.0f);
	TrackObject->SetGraphPosition(425,365,700.0f);
}

void ChampionshipScreen::LoadScenario()
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
	
	for (i=0 ; i<10 ; i++)
	{
		MainGameInfo.CarTypes[i] = CurrentScenario->Car[i];
	}

	char *Pointer = CurrentScenario->Description;
	int Length;
	
	for (i=0 ; i<10 ; i++)
	{
		*ScenarioLines[i] = 0;
		if (*Pointer && !i || *(Pointer-1) && i)
		{
			Length = 0;
			for (int j=0 ; j<48 ; j++)
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
			strncpy(ScenarioLines[i], Pointer, Length);
			ScenarioLines[i][Length] = 0;
			Pointer += Length+1;
		}
	}

	for (i=0 ; i<10 ; i++)
	{
		ScenarioDetails[i] = NULL;
		if (*ScenarioLines[i] || i >6)
		{
			if (i<7)
				ScenarioDetails[i] = new TextWord(ScenarioLines[i], MainAlphabet);
			else
			{
				ScenarioDetails[i] = new TextWord(TranslateLocale(90+i-7), MainAlphabet);
			}
			ScenarioDetails[i]->Justify(JUSTIFY_LEFT);
			ScenarioDetails[i]->Write(200,200+(i>7 ? i-1 : i)*16, 1700.0f);
			ScenarioDetails[i]->Hide();
		}
	}

	if (MainGameInfo.ChampionshipPosition[0] < 0)
	{
		MainGameInfo.ChampionshipPosition[0] = CurrentScenario->Cars-1;
		MainGameInfo.QualifiedPosition[0] = CurrentScenario->Cars-1;
	}

	switch (CurrentScenario->Length)
	{
	case 0:
		if (MainGameInfo.Track == T_TRIPOLI) MainGameInfo.Laps = 3;
		if (MainGameInfo.Track == T_AVUS) MainGameInfo.Laps = 4;
		if (MainGameInfo.Track == T_MONTANA) MainGameInfo.Laps = 10;
		if (MainGameInfo.Track == T_ROOSEVELT) MainGameInfo.Laps = 6;
		if (MainGameInfo.Track == T_MONTLHERY) MainGameInfo.Laps = 3;
		if (MainGameInfo.Track == T_PAU) MainGameInfo.Laps = 8;
		if (MainGameInfo.Track == T_DONINGTON) MainGameInfo.Laps = 6;
		if (MainGameInfo.Track == T_BROOKLANDS) MainGameInfo.Laps = 8;
		if (MainGameInfo.Track == T_MONZA) MainGameInfo.Laps = 3;
		break;
	case 1:
		if (MainGameInfo.Track == T_TRIPOLI) MainGameInfo.Laps = 5;
		if (MainGameInfo.Track == T_AVUS) MainGameInfo.Laps = 8;
		if (MainGameInfo.Track == T_MONTANA) MainGameInfo.Laps = 22;
		if (MainGameInfo.Track == T_ROOSEVELT) MainGameInfo.Laps = 12;
		if (MainGameInfo.Track == T_MONTLHERY) MainGameInfo.Laps = 6;
		if (MainGameInfo.Track == T_PAU) MainGameInfo.Laps = 14;
		if (MainGameInfo.Track == T_DONINGTON) MainGameInfo.Laps = 12;
		if (MainGameInfo.Track == T_BROOKLANDS) MainGameInfo.Laps = 14;
		if (MainGameInfo.Track == T_MONZA) MainGameInfo.Laps = 6;
		break;
	case 2:
		if (MainGameInfo.Track == T_TRIPOLI) MainGameInfo.Laps = 10;
		if (MainGameInfo.Track == T_AVUS) MainGameInfo.Laps = 12;
		if (MainGameInfo.Track == T_MONTANA) MainGameInfo.Laps = 30;
		if (MainGameInfo.Track == T_ROOSEVELT) MainGameInfo.Laps = 20;
		if (MainGameInfo.Track == T_MONTLHERY) MainGameInfo.Laps = 10;
		if (MainGameInfo.Track == T_PAU) MainGameInfo.Laps = 25;
		if (MainGameInfo.Track == T_DONINGTON) MainGameInfo.Laps = 20;
		if (MainGameInfo.Track == T_BROOKLANDS) MainGameInfo.Laps = 25;
		if (MainGameInfo.Track == T_MONZA) MainGameInfo.Laps = 10;
		break;
	}
}

void ChampionshipScreen::UpdateTable()
{
	if (MainGameInfo.RaceType == 0xFF)
		return;

	for (int i=0 ; MainGameInfo.CarTypes[i] ; i++)
		MainGameInfo.QualifiedPosition[i] = i-1;
	MainGameInfo.QualifiedPosition[0] = i-1;

	switch (MainGameInfo.Track)
	{
	case T_TRIPOLI: MainGameInfo.Track = T_AVUS; break;
	case T_AVUS: MainGameInfo.Track = T_MONTANA; break;
	case T_MONTANA: MainGameInfo.Track = T_ROOSEVELT; break;
	case T_ROOSEVELT: MainGameInfo.Track = T_MONTLHERY; break;
	case T_MONTLHERY: MainGameInfo.Track = T_PAU; break;
	case T_PAU: MainGameInfo.Track = T_DONINGTON; break;
	case T_DONINGTON: MainGameInfo.Track = T_BROOKLANDS; break;
	case T_BROOKLANDS: MainGameInfo.Track = T_MONZA; break;
	case T_MONZA: MainGameInfo.Track = T_TRIPOLI; break;
	default: break;
	}

	LoadTrack();
	LoadCar();
/*	
	GlobalTrack = TrackObject = new FrontEndTrack(MainGameInfo.Track);
	GlobalTrack->SetTrackPosition(395,230,18.0f);
	GlobalTrack->SetNamePosition(415,115,900.0f);
	GlobalTrack->SetGraphPosition(425,365,700.0f);

	GlobalCar = CarObject = new FrontEndCar(MainGameInfo.CarTypes[0]);
	GlobalCar->SetCarPosition(400,230,9.0f);
	GlobalCar->SetNamePosition(420,117,500.0f);
	GlobalCar->SetGraphPosition(425,365,700.0f);
*/
	int NumberOfCars = 0;

	for (i=0 ; MainGameInfo.CarTypes[i] ; i++)
	{
		switch (MainGameInfo.Positions[i])
		{
		case 0:
			dprintf("Adding %d points to car %d, number %d", 10, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=10;
			break;
		case 1:
			dprintf("Adding %d points to car %d, number %d", 6, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=6;
			break;
		case 2:
			dprintf("Adding %d points to car %d, number %d", 3, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=3;
			break;
		case 3:
			dprintf("Adding %d points to car %d, number %d", 1, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=1;
			break;
		case 4:
			dprintf("Adding %d points to car %d, number %d", 0, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=0;
			break;
		case 5:
			dprintf("Adding %d points to car %d, number %d", 0, i, MainGameInfo.CarTypes[i]);
			MainGameInfo.ChampionshipPoints[i]+=0;
			break;
		}
		NumberOfCars++;
	}

	//NumberOfCars--;

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
#if defined(JAPAN)
extern int JapanHelp;
#endif
void ChampionshipScreen::DrawScenarioDetails()
{
	for (int i=0 ; i<10 ; i++)
	{
		if (*ScenarioLines[i])
			ScenarioDetails[i]->Write();
	}

	if (CurrentSelection == CS_QUALIFY) 
	{
		ScenarioDetails[8]->Write();
		ScenarioDetails[9]->Write();
#if defined(JAPAN)
		SetJapanText(6);
#endif
	}
	if (CurrentSelection == CS_PRACTICE)
	{
		ScenarioDetails[7]->Write();
#if defined(JAPAN)
		SetJapanText(5);
#endif
	}
}

void ChampionshipScreen::HideScenarioDetails()
{
#if defined(JAPAN)
	SetJapanText(-1);
#endif
	for (int i=0 ; i<10 ; i++)
	{
		if (*ScenarioLines[i])
			ScenarioDetails[i]->Hide();
	}

	if (CurrentSelection == CS_QUALIFY) 
	{
		ScenarioDetails[8]->Hide();
		ScenarioDetails[9]->Hide();
	}
	if (CurrentSelection == CS_PRACTICE)
		ScenarioDetails[7]->Hide();
}

void ChampionshipScreen::LoadMiniCars()
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

void ChampionshipScreen::DrawStartGrid()
{
	int NumberOfCars=0;
	for (int i=0 ; i<10 && MiniCar[i] ; i++);
	NumberOfCars=i;
	int x,y;
	int Position;

	for (i=0 ; i<NumberOfCars ; i++)
	{
		Position = MainGameInfo.QualifiedPosition[i];
		x = 240+ (int)(Position/2)*153+ (Position%2 ? 35 : 0);
		y = 200+ (Position%2)*100;
		MiniCar[i]->Draw( x,y, 800.0f );
		CarName[i]->Write( x,y+23,1800.0f );
	}
}

void ChampionshipScreen::HideStartGrid()
{
	for (int i=0 ; i<10 && MiniCar[i] ; i++)
	{
		MiniCar[i]->Hide();
		CarName[i]->Hide();
	}
}

void ChampionshipScreen::LoadStandingsTable()
{
	char IconName[50], Buffer[50], Buffer2[50];

//	arcPush(DISKNAME "\\SOS1937FrontEnd\\ReducedCars.tc");
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
//	arcPop();

	for (i=0 ; i<10 && CarInTable[i] ; i++)
	{
		sprintf(Buffer, "%d", MainGameInfo.ChampionshipPosition[i]+1);
		PositionWord[i] = new TextWord(Buffer, MainAlphabet);
		PositionWord[i]->Justify(JUSTIFY_RIGHT);
		
		sprintf(Buffer, "%d", MainGameInfo.CarTypes[i]);
		CarNumber[i] = new TextWord(Buffer, MainAlphabet);
		CarNumber[i]->Justify(JUSTIFY_LEFT);
		
		sprintf(Buffer, "%d", MainGameInfo.ChampionshipPoints[i]);
		CarPoints[i] = new TextWord(Buffer, MainAlphabet);
		CarPoints[i]->Justify(JUSTIFY_LEFT);

		strcpy(Buffer, CarNumbers[MainGameInfo.CarTypes[i]-1]);
		ConvertShortToLong(Buffer, Buffer2);
		CarType[i] = new TextWord(Buffer2, MainAlphabet);
		CarType[i]->Justify(JUSTIFY_LEFT);

		DriverName[i] = NULL;
	}
}

void ChampionshipScreen::DrawStandings()
{
	int x,y;
	for (int i=0 ; i<10 ; i++)
	{
		if (CarInTable[i])
		{
			x = 210;
			y = 150+MainGameInfo.ChampionshipPosition[i]*20;

			PositionWord[i]->Write(x,y, 1200.0f);
			CarInTable[i]->Draw(x+10,y+3, 1000.0f);
			CarPoints[i]->Write(x+75,y,1200.0f);
			CarNumber[i]->Write(x+110,y, 1200.0f);
			CarType[i]->Write(x+140,y,  1200.0f);
		}
	}
}

void ChampionshipScreen::HideStandings()
{
	for (int i=0 ; i<10 ; i++)
	{
		if (CarInTable[i])
		{
			CarInTable[i]->Hide();
			PositionWord[i]->Hide();
			CarPoints[i]->Hide();
			CarNumber[i]->Hide();
			CarType[i]->Hide();
		}
	}
}


void ChampionshipScreen::Update(struct Instruction *ScreenCommand)
{
	if (Counter > -1)
	{
		switch (CurrentSelection)
		{
		case CS_STARTGRID:
			break;
		case CS_TRACKINFO:
			TrackObject->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f+(float)sin(Counter*0.013f)*3.0f,0.0f);
			break;
		case CS_CARINFO:
			CarObject->Rotate(30.0f,90.0f+Counter,0.0f);
			break;
		case CS_STANDINGS:
			break;
		case CS_PRACTICE:
			break;
		case CS_QUALIFY:
			break;
		case CS_MEMORY:
			GlobalMemoryCard->Rotate(80.0f+(float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
			break;
		}
		
		Counter++;
	}
	
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

void ChampionshipScreen::DrawCurrentScreen()
{
	switch (CurrentSelection)
	{
	case CS_STARTGRID:
		DrawStartGrid();
		break;
	case CS_TRACKINFO:
		TrackObject->Draw();
		break;
	case CS_CARINFO:
		CarObject->Draw();
		break;
	case CS_STANDINGS:
		DrawStandings();
		break;
	case CS_PRACTICE:
		DrawScenarioDetails();
		ChangeHelpLine(1,true);
		break;
	case CS_QUALIFY:
		DrawScenarioDetails();
		ChangeHelpLine(1,true);
		break;
	case CS_MEMORY:
		GlobalMemoryCard->Draw(400,260,300.0f);
		break;
	}
}

void ChampionshipScreen::HideCurrentScreen()
{
	switch (CurrentSelection)
	{
	case CS_STARTGRID:
		HideStartGrid();
		break;
	case CS_TRACKINFO:
		TrackObject->Hide();
		break;
	case CS_CARINFO:
		CarObject->Hide();
		break;
	case CS_STANDINGS:
		HideStandings();
		break;
	case CS_PRACTICE:
		HideScenarioDetails();
		ChangeHelpLine(1,false);
		break;
	case CS_QUALIFY:
		HideScenarioDetails();
		ChangeHelpLine(1,false);
		break;
	case CS_MEMORY:
		GlobalMemoryCard->Hide();
		break;
	}
}

int ChampionshipScreen::ControlPressed(int ControlPacket)
{
	if (!Confirming)
	{
		if (ControlPacket & 1 << CON_THROTTLE || ControlPacket & 1 << CON_DPAD_UP)
		{
			HideCurrentScreen();
			MainMenu->Lowlight(CurrentSelection);
			if (--CurrentSelection < CS_STARTGRID) CurrentSelection = CS_MEMORY;
			MainMenu->Highlight(CurrentSelection);
			DrawCurrentScreen();
		}
		
		if (ControlPacket & 1 << CON_BRAKE || ControlPacket & 1 << CON_DPAD_DOWN)
		{
			HideCurrentScreen();
			MainMenu->Lowlight(CurrentSelection);
			if (++CurrentSelection > CS_MEMORY) CurrentSelection = CS_STARTGRID;
			MainMenu->Highlight(CurrentSelection);
			DrawCurrentScreen();
		}
		
		if (ControlPacket & 1 << CON_START)
		{
			MainMenu->Lowlight(CurrentSelection);
			Destroy();
			MainGameInfo.RaceMode = RM_RACE;
			ReEntryPoint = REP_CHAMPIONSHIPRACEOVER;
			return 1;
		}
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C)
	{
		if (Confirming)
		{
			MainMenu->Lowlight(CurrentSelection);
			Destroy();
			
			MainGameInfo.Track = T_TRIPOLI;
			MainGameInfo.RaceMode = 0;

			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
			
			AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			if (CurrentSelection == CS_PRACTICE)
			{
				MainMenu->Lowlight(CurrentSelection);
				Destroy();
				MainGameInfo.RaceMode = RM_PRACTICE;
				ReEntryPoint = REP_CHAMPIONSHIPRACEOVER;
				return 1;
			}
			if (CurrentSelection == CS_QUALIFY)
			{
				MainMenu->Lowlight(CurrentSelection);
				Destroy();
				MainGameInfo.RaceMode = RM_QUALIFY;
				ReEntryPoint = REP_CHAMPIONSHIPRACEOVER;
				return 1;
			}
			if (CurrentSelection == CS_MEMORY)
			{
				MainMenu->Lowlight(CurrentSelection);
				Destroy();
				
				// Change screen.
				QueuedCommand = 'S';
				QueuedValue = (Screen *) new MemoryCardScreen(MainAlphabet);
			}
		}
	}


	if (ControlPacket & 1 << CON_B)
	{
		if (Confirming)
		{
			SetStartState(true);
			Confirming = false;
			DeleteHelp();
			SetHelpText(TranslateLocale(0), TranslateLocale(1));;
		}
		else
		{
			SetStartState(false);
			DeleteHelp();
			SetHelpText(TranslateLocale(87), TranslateLocale(54));
			Confirming = true;
		}
	}
	return 0;
}

// Not needed as we're using the default stuff.
void ChampionshipScreen::DrawBackground()
{
	DrawFullScreen2D((DWORD *)ChampionshipBackdrop, true);
}

ChampionshipScreen::~ChampionshipScreen()
{
}

void ChampionshipScreen::Destroy()
{
	for (int i=0; i<10 && MiniCar[i]; i++)
	{
		delete MiniCar[i];
		delete CarName[i];

		delete CarInTable[i];
		delete PositionWord[i];
		delete CarPoints[i];
		delete CarNumber[i];
		delete CarType[i];
	}

	for (i=0 ; i<10 ; i++)
	{
		if (*ScenarioLines[i] || i > 6)
			delete ScenarioDetails[i];
	}
	delete MainMenu;
	MainMenu = NULL;

	GlobalCar->Hide();
	delete TrackObject;
	GlobalTrack = NULL;
	GlobalMemoryCard->Hide();

	ScreenHeaders[HEADER_CHAMPIONSHIP]->Hide();

	SetStartState(false);

	DeleteHelp();

	Counter = -1;
	return;
}