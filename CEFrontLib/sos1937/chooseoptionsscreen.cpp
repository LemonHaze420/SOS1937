// $HEADER$

// $LOG$

/* The race type screen of the game.

	Ian Gledhill 26/01/2000 :-)
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

extern class ButtonBank *MainMenu;

ChooseOptionsScreen::ChooseOptionsScreen(Alphabet *DefaultAlphabet)
{
	// Reset the counter to zero.
	Counter = 0;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ChooseOptionsScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ChooseOptionsScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_GLANCE)
	{
		MainMenu->Unselect(SETTINGS);
		
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(NULL);

		((FrontScreen *)QueuedValue)->SetHighlighted(SETTINGS);
	}

	return 0;
}

// Not needed as we're using the default stuff.
void ChooseOptionsScreen::DrawBackground()
{
}

ChooseOptionsScreen::~ChooseOptionsScreen()
{
}

void ChooseOptionsScreen::Destroy()
{
	return;
}