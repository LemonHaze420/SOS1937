// $Header$

// $Log$
// Revision 1.19  2000-05-08 19:53:27+01  img
// Final? build
//
// Revision 1.18  2000-05-05 21:27:32+01  img
// Moved success string down (again!)
//
// Revision 1.17  2000-05-05 12:17:17+01  img
// Build 19
//
// Revision 1.16  2000-05-04 17:39:31+01  img
// Build 18
//
// Revision 1.15  2000-04-28 11:09:13+01  jcf
// <>
//
// Revision 1.14  2000-04-23 20:48:43+01  img
// Bug fixes
//
// Revision 1.13  2000-04-22 18:26:08+01  img
// Added C button to joystick support
//
// Revision 1.12  2000-04-20 16:05:21+01  img
// Qualification works
//
// Revision 1.11  2000-04-19 21:38:04+01  img
// Localisation.
//
// Revision 1.10  2000-04-07 14:38:11+01  img
// Help support, generic background support
//
// Revision 1.9  2000-04-05 17:46:11+01  img
// Show scenario result
//
// Revision 1.8  2000-03-30 14:48:26+01  img
// More preloadings
//
// Revision 1.7  2000-03-28 17:56:42+01  img
// Fixed scenario loading
//
// Revision 1.6  2000-03-27 10:26:30+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.5  2000-03-22 09:29:08+00  img
// <>
//
// Revision 1.4  2000-03-21 17:28:28+00  img
// Pre-loading more now...
//
// Revision 1.3  2000-03-20 18:00:57+00  img
// Beta build
//
// Revision 1.2  2000-03-15 09:32:58+00  img
// Fully working with start grid.
//
// Revision 1.1  2000-03-14 14:25:53+00  img
// First working version. Still needs grid displayed.
//
// Revision 1.0  2000-03-14 09:26:47+00  img
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

ScenarioDetailsScreen::ScenarioDetailsScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;
	delete MainMenu;

	AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	dprintf("new scenario details screen");

	MainMenu = new ButtonBank();

	MainMenu->Add(ScenarioBank[0]);
	MainMenu->Add(ScenarioBank[1]);
	MainMenu->Add(ScenarioBank[2]);
	MainMenu->Add(ScenarioBank[3]);
	MainMenu->DrawAll();

	CurrentSelection = SD_OBJECTIVE;
	MainMenu->Highlight(CurrentSelection);
	LoadScenarioDetails();
	DrawObjective();

	for (int i=0 ; i<5 ; i++) ScenarioResult[i] = NULL;
	if (ReEntryPoint)
		DrawScenarioResult();
	ReEntryPoint = 0;

	CarObject = GlobalCar;

	TrackObject = GlobalTrack;
	LoadMiniCars();

	ScreenHeaders[HEADER_SCENARIO]->Draw();

	SetStartState(true);

	SetHelpText(TranslateLocale(0)," ");

	// Reset the counter to zero.
	Counter = 0;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ScenarioDetailsScreen::DrawScenarioResult()
{
	char *Pointer;
	
	// "Length" is as good a field as any.
	if (!strnicmp(ThisScenario->LengthString, "LapTime", 7))
	{
		if (MainGameInfo.LapTime[0] <= ThisScenario->SuccessValue)
			Pointer = ThisScenario->SuccessString;
		else
			Pointer = ThisScenario->FailureString;
	}
	else
	{
		if (MainGameInfo.Positions[0] <= ThisScenario->SuccessValue-1)
			Pointer = ThisScenario->SuccessString;
		else
			Pointer = ThisScenario->FailureString;
	}

	int Length;
	char Lines[5][40];

	for (int i=0 ; i<5 ; i++)
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
			ScenarioResult[i]->Write(200,304+i*24, 1200.0f);
		}
	}
}

void ScenarioDetailsScreen::HideScenarioResult()
{
	if (ScenarioResult[0])
		for (int i=0 ; i<5 ; i++)
		{
			delete ScenarioResult[i];
			ScenarioResult[i] = NULL;
		}
}

void ScenarioDetailsScreen::LoadScenarioDetails()
{
	ThisScenario = AllScenarios[MainGameInfo.Difficulty][MainGameInfo.ScenarioID];

	char *Pointer = ThisScenario->Description;
	int Length;


	for (int i=0 ; i<10 ; i++)
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
		if (*ScenarioLines[i])
		{
			ScenarioDetails[i] = new TextWord(ScenarioLines[i], MainAlphabet);
			ScenarioDetails[i]->Justify(JUSTIFY_LEFT);
			ScenarioDetails[i]->Write(200,176+i*16, 1700.0f);
			ScenarioDetails[i]->Hide();
		}
	}

	for (i=1 ; i<10 ; i++)
	{
		if ( i<ThisScenario->GridPosition )
			MainGameInfo.QualifiedPosition[i] = i-1;
		else
			MainGameInfo.QualifiedPosition[i] = i;
	}
	MainGameInfo.QualifiedPosition[0] = ThisScenario->GridPosition-1;
	MainGameInfo.Laps = ThisScenario->Laps;
}

extern void ConvertCarToDescription(char *CarName, char *DescriptionName);
extern void ConvertShortToLong(char *, char *);

void ScenarioDetailsScreen::LoadMiniCars()
{
	char ShortName[25];
	
//	arcPush(DISKNAME "\\SOS1937FrontEnd\\GridModels.tc");
	
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

void ScenarioDetailsScreen::DrawObjective()
{
	for (int i=0 ; i<10 ; i++)
	{
		if (*ScenarioLines[i])
			ScenarioDetails[i]->Write();
	}
}

void ScenarioDetailsScreen::HideObjective()
{
	for (int i=0 ; i<10 ; i++)
	{
		if (*ScenarioLines[i])
			ScenarioDetails[i]->Hide();
	}

	HideScenarioResult();
}

void ScenarioDetailsScreen::DrawStartGrid()
{
	int NumberOfCars=0;
	for (int i=0 ; i<10 && MiniCar[i] ; i++);
	NumberOfCars=i;
	int Position;
	int x,y;

	for (i=0 ; i<NumberOfCars ; i++)
	{
		Position = MainGameInfo.QualifiedPosition[i];
		x = 240+ (int)(Position/2)*153 + (Position%2 ? 35 : 0);
		y = 200+ (Position%2)*100;
		MiniCar[i]->Draw(x, y, 800.0f);
		CarName[i]->Write( x,y+23,1800.0f );
	}
}

void ScenarioDetailsScreen::HideStartGrid()
{
	for (int i=0 ; i<10 && MiniCar[i] ; i++)
	{
		MiniCar[i]->Hide();
		CarName[i]->Hide();
	}
}

void ScenarioDetailsScreen::DrawTrackInfo()
{
	TrackObject->Draw();
}

void ScenarioDetailsScreen::HideTrackInfo()
{
	TrackObject->Hide();
}

void ScenarioDetailsScreen::DrawCarInfo()
{
	CarObject->Draw();
}

void ScenarioDetailsScreen::HideCarInfo()
{
	CarObject->Hide();
}

void ScenarioDetailsScreen::Update(struct Instruction *ScreenCommand)
{
	if (Counter > -1)
	{
		switch (CurrentSelection)
		{
		default:
		case SD_OBJECTIVE:
			break;
		case SD_STARTGRID:
			break;
		case SD_TRACKINFO:
			if (TrackObject)
				TrackObject->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f+(float)sin(Counter*0.013f)*3.0f,0.0f);
			break;
		case SD_CARINFO:
			if (CarObject)
				CarObject->Rotate(30.0f,90.0f+Counter,0.0f);
			break;
		}
		
		Counter++;
	}

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ScenarioDetailsScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_THROTTLE || ControlPacket & 1 << CON_DPAD_UP)
	{
		switch (CurrentSelection)
		{
		default:
		case SD_OBJECTIVE:		HideObjective();	break;
		case SD_STARTGRID:		HideStartGrid();	break;
		case SD_TRACKINFO:		HideTrackInfo();	break;
		case SD_CARINFO:		HideCarInfo();		break;
		}
		MainMenu->Lowlight(CurrentSelection);
		if (--CurrentSelection < SD_OBJECTIVE) CurrentSelection = SD_CARINFO;
		MainMenu->Highlight(CurrentSelection);
		switch (CurrentSelection)
		{
		default:
		case SD_OBJECTIVE:		DrawObjective();	break;
		case SD_STARTGRID:		DrawStartGrid();	break;
		case SD_TRACKINFO:		DrawTrackInfo();	break;
		case SD_CARINFO:		DrawCarInfo();		break;
		}
	}
	if (ControlPacket & 1 << CON_BRAKE || ControlPacket & 1 << CON_DPAD_DOWN)
	{
		switch (CurrentSelection)
		{
		default:
		case SD_OBJECTIVE:		HideObjective();	break;
		case SD_STARTGRID:		HideStartGrid();	break;
		case SD_TRACKINFO:		HideTrackInfo();	break;
		case SD_CARINFO:		HideCarInfo();		break;
		}
		MainMenu->Lowlight(CurrentSelection);
		if (++CurrentSelection > SD_CARINFO) CurrentSelection = SD_OBJECTIVE;
		MainMenu->Highlight(CurrentSelection);
		switch (CurrentSelection)
		{
		default:
		case SD_OBJECTIVE:		DrawObjective();	break;
		case SD_STARTGRID:		DrawStartGrid();	break;
		case SD_TRACKINFO:		DrawTrackInfo();	break;
		case SD_CARINFO:		DrawCarInfo();		break;
		}
	}
		
	if (ControlPacket & 1 << CON_B)
	{
		MainMenu->Lowlight(CurrentSelection);
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
		AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	}

	if (ControlPacket & 1 << CON_START)
	{
		MainGameInfo.RaceMode = RM_RACE;
		ReEntryPoint = REP_SCENARIOOVER;
		MainMenu->Lowlight(CurrentSelection);
		Destroy();
		return 1;
	}

	return 0;
}

// Not needed as we're using the default stuff.
void ScenarioDetailsScreen::DrawBackground()
{
	DrawFullScreen2D((DWORD *)ScenarioBackdrop, true);
}

ScenarioDetailsScreen::~ScenarioDetailsScreen()
{
}

void ScenarioDetailsScreen::Destroy()
{
	delete MainMenu;
	MainMenu = NULL;

	CarObject->Hide();
	TrackObject->Hide();

	HideScenarioResult();

	HideStartGrid();

	ScreenHeaders[HEADER_SCENARIO]->Hide();

	SetStartState(false);

	for (int i=0 ; i<10 ; i++)
	{
		if (ScenarioDetails[i])
		{
			delete ScenarioDetails[i];
			ScenarioDetails[i] = NULL;
		}

		if (MiniCar[i])
		{
			delete MiniCar[i];
			delete CarName[i];
		}
	}
	
	DeleteHelp();

	Counter = -1;
	
	return;
}