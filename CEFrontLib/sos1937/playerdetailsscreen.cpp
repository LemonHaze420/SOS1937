// $Header$

// $Log$
// Revision 1.10  2000-03-21 17:28:27+00  img
// Pre-loading more now...
//
// Revision 1.9  2000-03-08 10:30:47+00  img
// Use Courier font
//
// Revision 1.8  2000-03-07 16:02:24+00  img
// Removed gearbox etc, added OK.
//
// Revision 1.7  2000-03-06 13:03:15+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.6  2000-03-02 09:49:56+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.5  2000-03-01 12:36:00+00  img
// Now can choose difficulty level.
//
// Revision 1.4  2000-02-29 11:28:20+00  jjs
// Added Header and Log lines.
//

/* The Memory Card screen of the game.

	Ian Gledhill 14/02/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;

PlayerDetailsScreen::PlayerDetailsScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	PlayerNameTitle = new TextWord("ENTER NAME", MainAlphabet);
	PlayerNameTitle->Write(400,120,800.0f);

	PlayerName[0] = new TextWord(MainGameInfo.PlayerName[0], MainAlphabet);
	PlayerName[1] = new TextWord(MainGameInfo.PlayerName[1], MainAlphabet);
	PlayerNameYellow[0] = new TextWord(MainGameInfo.PlayerName[0], YellowAlphabet);
	PlayerNameYellow[1] = new TextWord(MainGameInfo.PlayerName[1], YellowAlphabet);

	PlayerName[0]->Write(400,160,1000.0f);
	PlayerName[1]->Write(400,180,1000.0f);

	OKButton = new TextWord("OK", MainAlphabet);
	OKYellowButton = new TextWord("OK", YellowAlphabet);

	OKButton->Write(400,220,800.0f);

	// Start on the difficulty level.
	CurrentSelection = PDS_PLAYERNAME1;

	HighlightSelection();

	// Reset the counter to zero.
	Counter = 0;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void PlayerDetailsScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

void PlayerDetailsScreen::LowlightSelection()
{
	switch (CurrentSelection)
	{
	case PDS_PLAYERNAME1:
		PlayerNameYellow[0]->Hide();
		PlayerName[0]->Write(400,160,1000.0f);
		break;
	case PDS_PLAYERNAME2:
		PlayerNameYellow[1]->Hide();
		PlayerName[1]->Write(400,180,1000.0f);
		break;
	default:
	case PDS_OK:
		OKYellowButton->Hide();
		OKButton->Write(400,220,800.0f);
		break;
	}
}

void PlayerDetailsScreen::HighlightSelection()
{
	switch (CurrentSelection)
	{
	case PDS_PLAYERNAME1:
		PlayerName[0]->Hide();
		PlayerNameYellow[0]->Write(400,160,1000.0f);
		break;
	case PDS_PLAYERNAME2:
		PlayerName[1]->Hide();
		PlayerNameYellow[1]->Write(400,180,1000.0f);
		break;
	default:
	case PDS_OK:
		OKButton->Hide();
		OKYellowButton->Write(400,220,800.0f);
		break;
	}

}

int PlayerDetailsScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_BRAKE)
	{
		LowlightSelection();
		if (++CurrentSelection > PDS_OK) CurrentSelection = PDS_PLAYERNAME1;
		HighlightSelection();
	}

	if (ControlPacket & 1 << CON_THROTTLE)
	{
		LowlightSelection();
		if (--CurrentSelection < PDS_PLAYERNAME1) CurrentSelection = PDS_OK;
		HighlightSelection();
	}

	if (ControlPacket & 1 << CON_GLANCE)	
	{
		switch (CurrentSelection)
		{
		case PDS_PLAYERNAME1:
			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new EnterPlayerNameScreen(MainAlphabet,1);
			break;
		case PDS_PLAYERNAME2:
			Destroy();
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new EnterPlayerNameScreen(MainAlphabet,2);
			break;
		case PDS_OK:
			MainMenu->Unselect(PLAYER);
			Destroy();
			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
			((FrontScreen *)QueuedValue)->SetHighlighted(PLAYER);
			break;		
		default:
			break;
		}
	}

	if (ControlPacket & 1 << CON_B)
	{
		MainMenu->Unselect(PLAYER);
	
		Destroy();

		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(PLAYER);
	}

	return 0;
}

// Not needed as we're using the default stuff.
void PlayerDetailsScreen::DrawBackground()
{
}

PlayerDetailsScreen::~PlayerDetailsScreen()
{
}

void PlayerDetailsScreen::Destroy()
{
	delete PlayerNameTitle;

	delete PlayerName[0];
	delete PlayerName[1];

	delete PlayerNameYellow[0];
	delete PlayerNameYellow[1];

	delete OKButton;
	delete OKYellowButton;

	return;
}