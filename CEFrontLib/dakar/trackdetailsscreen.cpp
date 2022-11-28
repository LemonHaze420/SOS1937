// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

TrackDetailsScreen::TrackDetailsScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	// Reset the counter to zero.
	Counter = 0;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

TrackDetailsScreen::~TrackDetailsScreen()
{
}

void TrackDetailsScreen::Update(struct Instruction *)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int TrackDetailsScreen::ControlPressed(int ControlPacket)
{
	return 0;
}

void TrackDetailsScreen::DrawBackground()
{
}

void TrackDetailsScreen::Destroy()
{
}
