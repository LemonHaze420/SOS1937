// $Header$

// $Log$
// Revision 1.10  2000-05-04 17:39:34+01  img
// Build 18
//
// Revision 1.9  2000-04-23 20:48:43+01  img
// Bug fixes
//
// Revision 1.8  2000-04-22 18:26:11+01  img
// Added C button to joystick support
//
// Revision 1.7  2000-04-21 19:06:03+01  img
// Bug fixes
//
// Revision 1.6  2000-04-21 12:11:13+01  img
// bug fix?
//
// Revision 1.5  2000-04-19 21:38:07+01  img
// Localisation.
//
// Revision 1.4  2000-04-14 14:49:45+01  img
// New terms
//
// Revision 1.3  2000-04-07 17:32:20+01  img
// Set ReEntry
//
// Revision 1.2  2000-04-07 14:38:18+01  img
// Help support, generic background support
//
// Revision 1.1  2000-04-03 14:54:13+01  img
// Working copy.
//
// Revision 1.0  2000-04-03 13:09:38+01  img
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
#include <dinput.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

extern void ConvertShortToLong(char *ShortName, char *LongName);
extern int AllControllerTypes[4];
extern LPDIRECTINPUTDEVICE2 Controller[4];

StartQuickRaceScreen::StartQuickRaceScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	// Check if we need to recreate the buttons.
	if (!MainMenu)
	{
		MainMenu = new ButtonBank();
		
		for (int i=0 ; i<7 ; i++)
			MainMenu->Add(RaceOptionsBank[i]);
		
		MainMenu->DrawAll();
	}

	for (int i=0 ; i<CONTINUE ; i++)
	{
		TextValues[i] = NULL;
	}

	TextValues[RACE_TYPE-1] = new TextWord(TranslateLocale(41), MainAlphabet);

	char Buffer[80];
	ConvertShortToLong(CarNumbers[MainGameInfo.CarTypes[0]-1], Buffer);

	TextValues[CAR-1] = new TextWord(Buffer, MainAlphabet);
	TextValues[TRACK-1] = new TextWord(TrackNumbers[MainGameInfo.Track], MainAlphabet);

	// Use the wheel type if we have a wheel connected.
	int ControllerType = (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) ;
	if (ControllerType != CCT_WHEEL && ControllerType != CCT_STICK && ControllerType != CCT_DREAMPAD)
		ControllerType = CCT_DREAMPAD;

	for (i=0 ; i<6 ; i++)
		MainGameInfo.QualifiedPosition[i]=i;

	if (ControllerType == -1) ControllerType = CCT_DREAMPAD;

	switch (ControllerType)
	{
	case CCT_STICK:
		TextValues[CONTROL-1] = new TextWord(TranslateLocale(81), MainAlphabet);
		break;
	case CCT_WHEEL:
		TextValues[CONTROL-1] = new TextWord(TranslateLocale(82), MainAlphabet);
		break;
	default:
	case CCT_DREAMPAD:
		TextValues[CONTROL-1] = new TextWord(TranslateLocale(83), MainAlphabet);
		break;
	}

	TextValues[PLAYER-1] = new TextWord(MainGameInfo.PlayerName[0], MainAlphabet);

	for (i=0 ; i<CONTINUE ; i++)
	{
		if (TextValues[i]) TextValues[i]->Justify(JUSTIFY_LEFT);
	}

	TextValues[RACE_TYPE-1]->Write(210,141,1000.0f);
	TextValues[CAR-1]->Write(210,183,1000.0f);
	TextValues[TRACK-1]->Write(210,226,1000.0f);
	TextValues[CONTROL-1]->Write(210,312,1000.0f);
	TextValues[PLAYER-1]->Write(210,356,1000.0f);

	// Reset the counter to zero.
	Counter = 0;

	ScreenHeaders[HEADER_SINGLERACE]->Draw();

	SetStartState(true);

	SetHelpText(TranslateLocale(84), " ");

	// Use the default background.
	UseDefaultBackdrop = true;
}

void StartQuickRaceScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int StartQuickRaceScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_B)
	{
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(CAR);
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
		ReEntryPoint = REP_QUICKRACEOVER;
		Destroy();
		MainGameInfo.RaceMode = RM_RACE;	

		return 1;
	}

	return 0;
}

// Not needed as we're using the default stuff.
void StartQuickRaceScreen::DrawBackground()
{
}

StartQuickRaceScreen::~StartQuickRaceScreen()
{
}

void StartQuickRaceScreen::Destroy()
{
	for (int i=0 ; i<CONTINUE ; i++)
	{
		if (TextValues[i])
			delete TextValues[i];
	}

	ScreenHeaders[HEADER_SINGLERACE]->Hide();

	SetStartState(false);

	DeleteHelp();

	return;
}