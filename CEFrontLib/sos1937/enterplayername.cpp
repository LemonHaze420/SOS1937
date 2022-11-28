// $Header$

// $Log$
// Revision 1.12  2000-05-08 19:53:16+01  img
// Final? build
//
// Revision 1.11  2000-05-06 13:48:38+01  img
// Sean build
//
// Revision 1.10  2000-05-04 17:39:27+01  img
// Build 18
//
// Revision 1.9  2000-04-22 18:26:04+01  img
// Added C button to joystick support
//
// Revision 1.8  2000-04-19 21:38:00+01  img
// Localisation.
//
// Revision 1.7  2000-04-07 14:38:06+01  img
// Help support, generic background support
//
// Revision 1.6  2000-04-03 14:53:01+01  img
// Only one player.
//
// Revision 1.5  2000-03-30 14:48:22+01  img
// More preloadings
//
// Revision 1.4  2000-03-08 10:30:13+00  img
// Uses Courier font
//
// Revision 1.3  2000-03-07 16:02:10+00  img
// Grid appears "over" the Player screen
//
// Revision 1.2  2000-03-06 13:03:13+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.1  2000-03-02 09:49:54+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.0  2000-03-01 13:09:18+00  img
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
extern int AllControllerTypes[];
extern bool UseKeys;

#define GRID_X		216
#define GRID_Y		185

EnterPlayerNameScreen::EnterPlayerNameScreen(Alphabet *DefaultAlphabet, int _PlayerNumber)
{
	MainAlphabet = DefaultAlphabet;

	UseKeys = true;

	PlayerNumber = _PlayerNumber-1;

	strcpy(OldName, MainGameInfo.PlayerName[PlayerNumber]);

	EnterName = new TextWord(TranslateLocale(35), MainAlphabet);
	EnterName->Write(400,120,800.0f);

	End = EndLetter;
	Del = DelLetter;

	char LetterName[2];
	for (int i=0 ; i<36 ; i++)
	{
		sprintf(LetterName, "%c", i+(i > 25 ? '0'-26 : 'A' ));
		Letter[i] = new TextWord(LetterName, MainAlphabet);
	}

	PlayerNameWord = NULL;
	DrawName();

	DrawGrid();

	CurrentLetter = 39;
	DrawCursor();
	
	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(36),TranslateLocale(37));

	// Reset the counter to zero.
	Counter = 0;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

void EnterPlayerNameScreen::DrawGrid()
{
	for (int i=0 ; i<36 ; i++)
	{
		Letter[i]->Write(GRID_X + (i%10)*38 , GRID_Y + ((int)((float)i/10.0f)) * 38, 600.0f);
	}
	
	i=36;
	Space->Draw(GRID_X + (i%10)*38 , GRID_Y + ((int)((float)i/10.0f)) * 38, 600.0f);
	i=38;
	Del->Draw(GRID_X + (i%10)*38 , GRID_Y + ((int)((float)i/10.0f)) * 38, 600.0f);
	i=39;
	End->Draw(GRID_X + (i%10)*38 , GRID_Y + ((int)((float)i/10.0f)) * 38, 600.0f);
}

void EnterPlayerNameScreen::DrawCursor()
{
	Cursor->Draw(GRID_X + (CurrentLetter%10)*38 , GRID_Y+6 + ((int)((float)CurrentLetter/10.0f)) * 38,600.0f);
}

void EnterPlayerNameScreen::DrawName()
{
	if (PlayerNameWord)
		delete PlayerNameWord;
	
	if (strlen(MainGameInfo.PlayerName[PlayerNumber]))
	{
		PlayerNameWord = new TextWord(MainGameInfo.PlayerName[PlayerNumber], MainAlphabet);
		PlayerNameWord->Write(400,380,700.0f);
	}
	else
		PlayerNameWord = NULL;
}

void EnterPlayerNameScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

void EnterPlayerNameScreen::AddLetter(char Letter)
{
	int Length = strlen(MainGameInfo.PlayerName[PlayerNumber]);
	if (Length < 12)
	{
		MainGameInfo.PlayerName[PlayerNumber][Length] = Letter;
		MainGameInfo.PlayerName[PlayerNumber][Length+1] = 0;
		DrawName();
	}
}

void EnterPlayerNameScreen::Delete()
{
	int Length = strlen(MainGameInfo.PlayerName[PlayerNumber]);
	if (Length)
	{
		MainGameInfo.PlayerName[PlayerNumber][Length-1] = 0;
		DrawName();
	}
}

int EnterPlayerNameScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_KEY)
	{
		char Letter;
		Letter = (char) (ControlPacket & 0xFF);
		if (Letter == 0x8) Delete();
		else AddLetter(Letter);
	}
	else
	{
		if (ControlPacket & 1 << CON_LEFT || ControlPacket & 1 << CON_BRAKE && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL)
		{
			if (--CurrentLetter < 0) CurrentLetter = 39;
			if (CurrentLetter == 37) CurrentLetter = 36;
			DrawCursor();
		}
		
		if (ControlPacket & 1 << CON_RIGHT || ControlPacket & 1 << CON_THROTTLE && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
		{
			if (++CurrentLetter > 39) CurrentLetter = 0;
			if (CurrentLetter == 37) CurrentLetter = 38;
			DrawCursor();
		}
		
		if (ControlPacket & 1 << CON_BRAKE && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  != CCT_WHEEL)
		{
			if ((CurrentLetter+=10) > 39) CurrentLetter -= 40;
			if (CurrentLetter == 37) CurrentLetter = 38;
			DrawCursor();
		}
		
		if (ControlPacket & 1 << CON_THROTTLE && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  != CCT_WHEEL)
		{
			if ((CurrentLetter-=10) < 0) CurrentLetter += 40;
			if (CurrentLetter == 37) CurrentLetter = 38;
			DrawCursor();
		}
		
		if (ControlPacket & 1 << CON_GLANCE || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
		{
			if (CurrentLetter < 26)
			{
				// We've selected a letter.
				AddLetter(CurrentLetter+'A');
			}
			else if (CurrentLetter < 36)
			{			
				// We've selected a number.
				AddLetter(CurrentLetter+'0'-26);
			}
			else 
				switch (CurrentLetter)
				{
				case 36:
				case 37:
					AddLetter(' ');
					break;
				case 38:		// Del
					Delete();
					break;
				case 39:
					// We must have a name!
					if (!*MainGameInfo.PlayerName[PlayerNumber]) break;
					Destroy();

					MainMenu->Unselect(PLAYER);

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
			strcpy(MainGameInfo.PlayerName[PlayerNumber], OldName);

			MainMenu->Unselect(PLAYER);

			Destroy();
			
			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
			((FrontScreen *)QueuedValue)->SetHighlighted(PLAYER);
		}
	}
	
	return 0;
}

// Not needed as we're using the default stuff.
void EnterPlayerNameScreen::DrawBackground()
{
}

EnterPlayerNameScreen::~EnterPlayerNameScreen()
{
}

void EnterPlayerNameScreen::Destroy()
{
	delete EnterName;

	for (int i=0 ; i<36 ; i++)
	{
		delete Letter[i];
	}

	Cursor->Hide();
	Del->Hide();
	End->Hide();
	Space->Hide();

	UseKeys = false;

	delete PlayerNameWord;

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	Counter = -1;

	DeleteHelp();

	return;
}