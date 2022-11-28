// $Header$

// $Log$
// Revision 1.15  2000-05-05 12:17:18+01  img
// Build 19
//
// Revision 1.14  2000-05-04 17:39:32+01  img
// Build 18
//
// Revision 1.13  2000-04-22 18:26:09+01  img
// Added C button to joystick support
//
// Revision 1.12  2000-04-19 21:38:05+01  img
// Localisation.
//
// Revision 1.11  2000-04-07 14:38:13+01  img
// Help support, generic background support
//
// Revision 1.10  2000-04-03 14:53:46+01  img
// New rotation
//
// Revision 1.9  2000-03-20 18:01:00+00  img
// Beta build
//
// Revision 1.8  2000-03-09 10:32:36+00  img
// New layout
//
// Revision 1.7  2000-03-06 13:03:16+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.6  2000-02-29 11:28:26+00  jjs
// Added Header and Log lines.
//

/* The track selection screen of the game.

	Ian Gledhill 06/01/1900 :-)
	Broadsword Interactive Ltd. */


#include "ConsoleFrontLib.h"

extern ButtonBank *MainMenu;
extern int AllControllerTypes[];

SelectTrackScreen::SelectTrackScreen(Alphabet *DefaultAlphabet)
{
	// Use the default alphabet for this screen.
	MainAlphabet = DefaultAlphabet;

	// The current car and the last defined car.
	MaxTrack = 8;
	CurrentTrack = 0;

	CurrentTrack = MainGameInfo.Track;	

	LeftArrow->Draw(345,140,600.0f);
	RightArrow->Draw(480,140,600.0f);

	CarObject = GlobalCar;
	TrackObject = GlobalTrack;

	TrackObject->Draw();

	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(74),TranslateLocale(1));

	// Use the default background.
	UseDefaultBackdrop = true;
}

SelectTrackScreen::ControlPressed(int ControlPacket)
{
	QueuedCommand = 0;

	if (ControlPacket & 1 << CON_LEFT || ControlPacket & 1 << CON_X && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		if (TrackObject != GlobalTrack) DestroyModel(); else GlobalTrack->Hide();
		CurrentTrack--;
		if (CurrentTrack < 0)	CurrentTrack = MaxTrack;
		LoadModel();
	}
	if (ControlPacket & 1 << CON_RIGHT || ControlPacket & 1 << CON_Y && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		if (TrackObject != GlobalTrack) DestroyModel(); else GlobalTrack->Hide();
		CurrentTrack++;
		if (CurrentTrack > MaxTrack) CurrentTrack = 0;
		LoadModel();		
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		MainMenu->Unselect(TRACK);
		MainGameInfo.Track = CurrentTrack;
		if (GlobalTrack != TrackObject)
		{
			delete GlobalTrack;
			GlobalTrack = TrackObject;
		}
		Destroy();	

		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(TRACK);
	}
	
	if (ControlPacket & 1 << CON_B)
	{
		MainMenu->Unselect(TRACK);
		if (TrackObject != GlobalTrack) DestroyModel();
		Destroy();
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(TRACK);
	}
	return 0;
}

void SelectTrackScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	// Rotate the map.
	if (TrackObject)
		TrackObject->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,-45.0f+(float)sin(Counter*0.013f)*3.0f,0.0f);
	
	Counter++;
}

void SelectTrackScreen::DrawBackground()
{
}

void SelectTrackScreen::DestroyModel()
{
	delete TrackObject;
	TrackObject = NULL;
}

void SelectTrackScreen::LoadModel()
{
	TrackObject = new FrontEndTrack(CurrentTrack);
	TrackObject->SetTrackPosition(395,230,18.0f);
	TrackObject->SetNamePosition(415,115,900.0f);
	TrackObject->SetGraphPosition(425,365,700.0f);
	TrackObject->Draw();	
}

SelectTrackScreen::~SelectTrackScreen()
{
}

void SelectTrackScreen::Destroy()
{
	LeftArrow->Hide(); RightArrow->Hide();

	GlobalTrack->Hide();

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	DeleteHelp();

	return;
}