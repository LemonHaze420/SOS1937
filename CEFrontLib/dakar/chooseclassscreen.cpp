// $Header

// $Log

// ChooseClass screen class

#include "../ConsoleFrontLib.h"

ChooseClassScreen::ChooseClassScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	// Reset the counter to zero.
	Counter = 0;
	
	// Use the ChooseClass background.
	UseChooseClassBackdrop = true;
}

ChooseClassScreen::~ChooseClassScreen()
{
}

void ChooseClassScreen::Update(struct Instruction *)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ChooseClassScreen::ControlPressed(int ControlPacket)
{
	return 0;
}

void ChooseClassScreen::DrawBackground()
{
}

void ChooseClassScreen::Destroy()
{
}
