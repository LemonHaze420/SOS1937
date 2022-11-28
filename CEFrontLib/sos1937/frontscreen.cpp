// $Header$

// $Log$
// Revision 1.40  2000-05-06 13:48:40+01  img
// Sean build
//
// Revision 1.39  2000-05-04 17:39:28+01  img
// Build 18
//
// Revision 1.38  2000-04-28 11:09:11+01  jcf
// <>
//
// Revision 1.37  2000-04-22 18:26:05+01  img
// Added C button to joystick support
//
// Revision 1.36  2000-04-21 19:06:02+01  img
// Bug fixes
//
// Revision 1.35  2000-04-20 16:05:21+01  img
// Qualification works
//
// Revision 1.34  2000-04-19 21:38:00+01  img
// Localisation.
//
// Revision 1.33  2000-04-17 14:52:59+01  img
// Bug fixes
//
// Revision 1.32  2000-04-12 18:02:22+01  img
// re-enabled controllers
//
// Revision 1.31  2000-04-12 13:03:04+01  jcf
// <>
//
// Revision 1.30  2000-04-11 16:12:03+01  img
// New icons on the screen
//
// Revision 1.29  2000-04-07 15:58:09+01  img
// Controller loadr moved to SOSIndex.cpp
//
// Revision 1.28  2000-04-07 15:29:12+01  img
// Better management of frontscreen's memory card
//
// Revision 1.27  2000-04-07 14:38:07+01  img
// Help support, generic background support
//
// Revision 1.26  2000-04-05 17:46:03+01  img
// Forces dreampad
//
// Revision 1.25  2000-04-04 18:40:44+01  img
// Controller sanity checking code
//
// Revision 1.24  2000-04-03 14:54:24+01  img
// New rotation, no playerscreen
//
// Revision 1.23  2000-03-30 14:48:24+01  img
// More preloadings
//
// Revision 1.22  2000-03-28 17:55:41+01  img
// Removed Debug message
//
// Revision 1.21  2000-03-27 10:26:28+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.20  2000-03-22 12:47:11+00  jjs
// <>
//
// Revision 1.19  2000-03-21 18:27:35+00  img
// Car shown
//
// Revision 1.18  2000-03-21 17:56:35+00  img
// Starts on Car
//
// Revision 1.17  2000-03-20 18:00:54+00  img
// Beta build
//
// Revision 1.16  2000-03-15 09:34:44+00  img
// Moved ConvertCarToDescription() to CarDetails.cpp
//
// Revision 1.15  2000-03-14 14:26:23+00  img
// Nowopens Scenario screen
//
// Revision 1.14  2000-03-13 18:16:00+00  img
// Now uses different name for race type titles
//
// Revision 1.13  2000-03-10 16:13:54+00  jjs
// Commented out Memory screen for now.
//
// Revision 1.12  2000-03-09 14:55:11+00  img
// Continue and Memory switched.
//
// Revision 1.11  2000-03-09 10:33:26+00  img
// New car/track layout
//
// Revision 1.10  2000-03-06 13:03:13+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.9  2000-02-29 11:28:17+00  jjs
// Added Header and Log lines.
//

/* The front screen of the game.

	Ian Gledhill 1/12/1999
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

extern void PlayFrontEndSound(int n);


FrontScreen::FrontScreen(Alphabet *DefaultAlphabet)
{
	ScreenName = FRONTSCREEN;
	ReEntryPoint = 0;

	// Use the default alphabet for this screen.
	MainAlphabet = DefaultAlphabet;

	// Reset the counter to zero.
	PreviousCounter = -1;

	// Check if we need to recreate the buttons.
	if (!MainMenu)
	{
		MainMenu = new ButtonBank();
		
		for (int i=0 ; i<7 ; i++)
			MainMenu->Add(RaceOptionsBank[i]);
		
		MainMenu->DrawAll();
	}
	
	// The currently selected button.
	Highlighted = CONTINUE;

	dprintf("new front screen");

	if (!GlobalCar)
		LoadCar();
	else
		CarObject = GlobalCar;
	RotateCar();

	if (!GlobalTrack || GlobalTrack->GetNumber() != MainGameInfo.Track)
		LoadTrack();
	else
		TrackObject = GlobalTrack;
	RotateTrack();

	LoadRaceTypeText();

	RaceTypeIcon = RaceTypeModels[MainGameInfo.RaceType];

	RotateTrack();

	GlobalMemoryCard->Draw(400,260,300.0f);
	GlobalMemoryCard->Hide();

	SoundObjects[1][SO_EFFECTS]->Draw(400,260,200.0f);
	SoundObjects[1][SO_EFFECTS]->Hide();

	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	EnterNameText = new TextWord(TranslateLocale(38), MainAlphabet);

	// Use the default background.
	UseDefaultBackdrop = true;

	SetStartState(true);
	MainGameInfo.RaceMode = 0;

	SetDefaultHelp();

	MainMenu->Highlight(Highlighted);
	SetHighlighted(RACE_TYPE);
//	DrawCar();
//	startMusic();
}

FrontScreen::~FrontScreen()
{
}

void FrontScreen::SetDefaultHelp()
{
	SetHelpText(TranslateLocale(39), TranslateLocale(1));
}

void FrontScreen::SetUnavailableHelp()
{
	DeleteHelp();
	switch (MainGameInfo.RaceType)
	{
	default:
	case RT_CHAMPIONSHIP:
		SetHelpText(TranslateLocale(39), TranslateLocale(89));
		break;
	case RT_SCENARIO:
		SetHelpText(TranslateLocale(39), TranslateLocale(88));
		break;
	}
}

void FrontScreen::LoadRaceTypeText()
{
	switch (MainGameInfo.RaceType)
	{
	case RT_CHAMPIONSHIP:	RaceTypeText = new TextWord(TranslateLocale(40), MainAlphabet);		break;
	case RT_QUICK:			RaceTypeText = new TextWord(TranslateLocale(41), MainAlphabet);	break;
	case RT_SCENARIO:		RaceTypeText = new TextWord(TranslateLocale(42), MainAlphabet);	break;
	case RT_SINGLE:			RaceTypeText = new TextWord(TranslateLocale(43), MainAlphabet); break;
	}
}

void FrontScreen::SetHighlighted(int _Highlighted)
{
	if (_Highlighted == Highlighted)
		return;

	PlayFrontEndSound(0);
	int OldHighlighted = Highlighted;

	MainMenu->Lowlight(Highlighted);
	Highlighted = _Highlighted;
	MainMenu->Highlight(Highlighted);
	
	switch (OldHighlighted)
	{
	case TRACK:
		HideTrack();
		if (MainGameInfo.RaceType == RT_CHAMPIONSHIP || MainGameInfo.RaceType == RT_SCENARIO)
		{	DeleteHelp();
			SetDefaultHelp();
		}
		break;
	case CAR:
		HideCar();
		if (MainGameInfo.RaceType == RT_CHAMPIONSHIP || MainGameInfo.RaceType == RT_SCENARIO)
		{
			DeleteHelp();
			SetDefaultHelp();
		}
		break;
	case RACE_TYPE:
		RaceTypeText->Hide();
		RaceTypeIcon->Hide();
		break;
	case SOUND:
		SoundObjects[1][SO_EFFECTS]->Hide();
		break;
	case CONTROL:
		GlobalController->Hide();
		break;
	case PLAYER:
		EnterNameText->Hide();
		CarObject->Hide();
		break;
	case CONTINUE:
		GlobalMemoryCard->Hide();
		break;
	default:
		break;
	}
	
	switch (Highlighted)
	{
	case TRACK:
		DrawTrack();
		if (MainGameInfo.RaceType == RT_CHAMPIONSHIP || MainGameInfo.RaceType == RT_SCENARIO)
			SetUnavailableHelp();
		break;
	case CAR:
		DrawCar();
		if (MainGameInfo.RaceType == RT_CHAMPIONSHIP || MainGameInfo.RaceType == RT_SCENARIO)
			SetUnavailableHelp();
		break;
	case RACE_TYPE:
		RaceTypeText->Write(400,110,1200.0f);
		RaceTypeIcon->Draw(400,300,300.0f);
		break;
	case SOUND:
		SoundObjects[1][SO_EFFECTS]->Draw();
		break;
	case CONTROL:
		GlobalController->Draw(400,240,600.0f);
		break;
	case PLAYER:
		EnterNameText->Write(400,110,1200.0f);
		CarObject->Draw(true);
		break;
	case CONTINUE:
		GlobalMemoryCard->Draw();
		break;
	default:
		break;
	}

	return;
}

void FrontScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = (void *)QueuedValue;
	
	switch (Highlighted)
	{
	case TRACK:
		RotateTrack();
		break;
	case CAR:
		RotateCar();
		break;
	case RACE_TYPE:
		RaceTypeIcon->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f + (float)sin(Counter*0.013f)*3.0f,0.0f);
		break;
	case SOUND:
		SoundObjects[1][SO_EFFECTS]->Rotate((float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
		break;
	case CONTROL:
		GlobalController->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
		break;
	case PLAYER:
		RotateCar();
		break;
	case CONTINUE:
		GlobalMemoryCard->Rotate(80.0f+(float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
		break;
	default:
		break;
	}

	Counter++;

	return;
}

int FrontScreen::ControlPressed(int ControlPacket)
{
	QueuedCommand = 0;
	int NewHighlighted = Highlighted;

	if (ControlPacket & 1 << CON_THROTTLE)
	{
		NewHighlighted = Highlighted-1;
		if (!NewHighlighted) NewHighlighted = CONTINUE;
	}
	
	if (ControlPacket & 1 << CON_BRAKE)
	{
		NewHighlighted = Highlighted+1;
		if (NewHighlighted > CONTINUE) NewHighlighted = RACE_TYPE;
	}
	
	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C)
	{
		MainMenu->Lowlight(Highlighted);
		switch (Highlighted)
		{
		case TRACK:
			if (MainGameInfo.RaceType != RT_QUICK && MainGameInfo.RaceType != RT_SINGLE)
			{
				MainMenu->Highlight(Highlighted);
				break;
			}
			MainMenu->Select(TRACK);
			
			Destroy();

			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new SelectTrackScreen(MainAlphabet);
			
			break;
		case CAR:
			if (MainGameInfo.RaceType != RT_QUICK && MainGameInfo.RaceType != RT_SINGLE)
			{
				MainMenu->Highlight(Highlighted);
				break;
			}
			MainMenu->Select(CAR);

			Destroy();

			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new SelectCarScreen(MainAlphabet);
			
			break;
		case RACE_TYPE:
			MainMenu->Select(RACE_TYPE);

			Destroy();

			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new RaceTypeScreen(MainAlphabet);
			
			break;
		case SOUND:
			MainMenu->Select(SOUND);

			Destroy();

			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new OptionsScreen(MainAlphabet);
			
			break;
		case CONTROL:
			MainMenu->Select(CONTROL);

			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new ControllerOptionsScreen(MainAlphabet);

			break;

		case PLAYER:
			MainMenu->Select(PLAYER);

			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new EnterPlayerNameScreen(MainAlphabet,1);
//			QueuedValue = (Screen *) new PlayerDetailsScreen(MainAlphabet);
			
			break;
			
		case CONTINUE:
			
			Destroy();
			
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new MemoryCardScreen(MainAlphabet);
			break;
		default:
			break;
		}
	}
	
	// Start button goes to the memory screen.
	if (ControlPacket & 1 << CON_START)
	{
		MainMenu->Lowlight(Highlighted);
		switch (MainGameInfo.RaceType)
		{
		case RT_CHAMPIONSHIP:
			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new ChampionshipScreen(MainAlphabet);
			break;
//		case RT_HEADTOHEAD:
//			Destroy();
//			return 1;
//			break;
		case RT_NETWORK:
			Destroy();
			return 1;
			break;
		case RT_QUICK:
			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new StartQuickRaceScreen(MainAlphabet);
			break;
		case RT_SCENARIO:
			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new ScenarioDetailsScreen(MainAlphabet);
			break;
		case RT_SINGLE:
			Destroy();
			
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new SingleRaceScreen(MainAlphabet);
			
			break;
		default:
			break;
		}
	}
	
	if (NewHighlighted != Highlighted)
	{
		SetHighlighted(NewHighlighted);
	}
	return 0;
}

void FrontScreen::Destroy(void)
{
	delete RaceTypeText;

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();
	
	GlobalController->Hide();
	TrackObject->Hide();
	CarObject->Hide();
	RaceTypeIcon->Hide();
	GlobalMemoryCard->Hide();
	SoundObjects[1][SO_EFFECTS]->Hide();
	delete EnterNameText;

	DeleteHelp();

	SetStartState(false);

	return;
}

// Not needed as we're using the default stuff.
void FrontScreen::DrawBackground()
{
}

extern char *FindValue(char *, char *);
extern void ReadVector(char *, vector3 &);
extern void ReadText(char * &, char *);
extern void ConvertCarToDescription(char *CarName, char *DescriptionName);

void FrontScreen::LoadCar()
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

void FrontScreen::DrawCar()
{
	CarObject->Draw();
}

void FrontScreen::HideCar()
{
	CarObject->Hide();
}

void FrontScreen::RotateCar()
{
	if (CarObject)
		GlobalCar->Rotate(20.0f,(float)Counter+90.0f,0.0f);
}

void FrontScreen::LoadTrack()
{
	GlobalTrack = TrackObject = new FrontEndTrack(MainGameInfo.Track);
	TrackObject->SetTrackPosition(395,230,18.0f);
	TrackObject->SetNamePosition(415,115,900.0f);
	TrackObject->SetGraphPosition(425,365,700.0f);
}

void FrontScreen::DrawTrack()
{
	TrackObject->Draw();
}

void FrontScreen::HideTrack()
{
	TrackObject->Hide();
}

void FrontScreen::RotateTrack()
{
	if (TrackObject)
		TrackObject->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f+(float)sin(Counter*0.013f)*3.0f,0.0f);
		
}