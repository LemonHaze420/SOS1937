// $Header$

// $Log$
// Revision 1.22  2000-05-04 17:39:32+01  img
// Build 18
//
// Revision 1.21  2000-04-22 18:26:08+01  img
// Added C button to joystick support
//
// Revision 1.20  2000-04-19 21:38:04+01  img
// Localisation.
//
// Revision 1.19  2000-04-07 14:38:13+01  img
// Help support, generic background support
//
// Revision 1.18  2000-04-03 14:53:41+01  img
// New rotations
//
// Revision 1.17  2000-03-30 14:48:27+01  img
// More preloadings
//
// Revision 1.16  2000-03-27 10:26:31+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.15  2000-03-20 18:00:59+00  img
// Beta build
//
// Revision 1.14  2000-03-15 11:57:06+00  img
// Sets CarTypes[] field.
//
// Revision 1.13  2000-03-09 11:40:16+00  img
// Now with arrows!
//
// Revision 1.12  2000-03-08 13:31:09+00  img
// New formatting.
//
// Revision 1.11  2000-03-06 13:03:16+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.10  2000-02-29 11:28:25+00  jjs
// Added Header and Log lines.
//

/* The front screen of the game.

	Ian Gledhill 1/12/1999
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <archive.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;
extern int AllControllerTypes[];

SelectCarScreen::SelectCarScreen(Alphabet *DefaultAlphabet, int _CurrentPlayer)
{
	// Use the default alphabet for this screen.
	MainAlphabet = DefaultAlphabet;

	CurrentPlayer = _CurrentPlayer;

	MaxCar = 14;

	GearBoxWord[GEARBOX_AUTOMATIC] = new TextWord(TranslateLocale(70), MainAlphabet);
	GearBoxWord[GEARBOX_MANUAL] = new TextWord(TranslateLocale(71), MainAlphabet);
	
	int CarIDs[15];
	
	CarIDs[0] = 12;		CarIDs[1] = 66;		CarIDs[2] = 8;
	CarIDs[3] = 1;		CarIDs[4] = 9;		CarIDs[5] = 10;
	CarIDs[6] = 7;		CarIDs[7] = 28;		CarIDs[8] = 3;
	CarIDs[9] = 22;		CarIDs[10] = 16;	CarIDs[11] = 15;
	CarIDs[12] = 2;		CarIDs[13] = 19;	CarIDs[14] = 5;
	
	for (int i=0 ; i<=MaxCar && strnicmp(CarNumbers[CarIDs[i]-1], CarNumbers[MainGameInfo.CarTypes[0]-1], strchr(CarNumbers[CarIDs[i]-1], '_')-CarNumbers[CarIDs[i]-1]); i++);
	CurrentCar = i;

	// Default background.
	UseDefaultBackdrop = true;

	CarModel = GlobalCar;
	CarModel->Draw();

	LeftArrow->Draw(260,115,600.0f);
	RightArrow->Draw(589,115,600.0f);
	UpArrow->Draw(420,135,800.0f);
	DownArrow->Draw(420,165,800.0f);

	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(72),TranslateLocale(73));

	DrawGearboxMode();
}

void ReadText(char * &CurrentPointer, char *Destination)
{
	int Length = 0;
	char *NewPointer = CurrentPointer;
	while (*NewPointer != ' ' && *NewPointer != ',' && *NewPointer != '\t' && *NewPointer != 13)
	{
		Length++;
		NewPointer++;
	}
	
	char TempDestination[256];
	strncpy(TempDestination, CurrentPointer, Length);

	TempDestination[Length] = 0;

	// Use a temporary buffer, and duplicate it. This way, it uses
	// the automatic allocation of the string to the right size.
	strcpy(Destination, TempDestination);

	CurrentPointer += Length;
}

void ReadVector(char *CurrentPointer, vector3 &Destination)
{
	// Find the first figure of the vector.
	while (*CurrentPointer == ' ' || *CurrentPointer == ',' || *CurrentPointer == '\t' || *CurrentPointer == '\n') CurrentPointer++;

	Destination.x = (float)atof(CurrentPointer);
	while (*CurrentPointer != ',') CurrentPointer++;
	while (*CurrentPointer == ' ' || *CurrentPointer == ',' || *CurrentPointer == '\t' || *CurrentPointer == '\n') CurrentPointer++;
	Destination.y = (float)atof(CurrentPointer);
	while (*CurrentPointer != ',') CurrentPointer++;
	while (*CurrentPointer == ' ' || *CurrentPointer == ',' || *CurrentPointer == '\t' || *CurrentPointer == '\n') CurrentPointer++;
	Destination.z = (float)atof(CurrentPointer);

}

char *FindValue(char *Pointer, char *SearchString)
{
	char * CurrentPointer = strstr(Pointer, SearchString);
	
	// pass the spaces..
	while (*CurrentPointer != ' ' && *CurrentPointer != ':' && *CurrentPointer != '\t') CurrentPointer++;
	while (*CurrentPointer == ' ' || *CurrentPointer == ':' || *CurrentPointer == '\t') CurrentPointer++;
	
	// Now we're pointing at the value.
	return CurrentPointer;
}
	
void SelectCarScreen::DrawGearboxMode()
{
	GearBoxWord[1-MainGameInfo.GearBox[CurrentPlayer]]->Hide();
	GearBoxWord[MainGameInfo.GearBox[CurrentPlayer]]->Write(420,145,1100.0f);
}

void SelectCarScreen::DestroyModel()
{
	GlobalCar->Hide();
}

void SelectCarScreen::LoadNewModel()
{
	GlobalCar = CarModel = AllCars[CurrentCar];
	CarModel->SetCarPosition(400,230,9.0f);
	CarModel->SetNamePosition(420,117,500.0f);
	CarModel->SetGraphPosition(425,365,700.0f);
	CarModel->Draw();
}

void SelectCarScreen::Update(struct Instruction *ScreenCommand)
{
	CarModel->Rotate(20.0f,(float)Counter+90.0f,0.0f);
	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int SelectCarScreen::ControlPressed(int ControlPacket)
{
	QueuedCommand = 0;

	if (ControlPacket & 1 << CON_LEFT && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL || ControlPacket & 1 << CON_X && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		CurrentCar--;
		if (CurrentCar < 0)	CurrentCar = MaxCar;

		DestroyModel();
		LoadNewModel();
	}
	if (ControlPacket & 1 << CON_RIGHT && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL || ControlPacket & 1 << CON_Y && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		CurrentCar++;
		if (CurrentCar > MaxCar) CurrentCar = 0;

		DestroyModel();
		LoadNewModel();
	}

	if ((ControlPacket & 1 << CON_THROTTLE || ControlPacket & 1 << CON_DPAD_UP) && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL
		|| (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL && ControlPacket & 1 << CON_RIGHT)
	{
		MainGameInfo.GearBox[CurrentPlayer] = 1-MainGameInfo.GearBox[CurrentPlayer];
		DrawGearboxMode();
	}

	if ((ControlPacket & 1 << CON_BRAKE || ControlPacket & 1 << CON_DPAD_DOWN) && (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) != CCT_WHEEL
		|| (AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL && ControlPacket & 1 << CON_LEFT)
	{
		MainGameInfo.GearBox[CurrentPlayer] = 1-MainGameInfo.GearBox[CurrentPlayer];
		DrawGearboxMode();
	}
		
	if (ControlPacket & 1 << CON_B)
	{
		MainMenu->Unselect(CAR);
		DestroyModel();
		Destroy();

		GlobalCar = NULL;

		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
		((FrontScreen *)QueuedValue)->SetHighlighted(CAR);
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		int CarIDs[15];
		
		CarIDs[0] = 12;		CarIDs[1] = 66;		CarIDs[2] = 8;
		CarIDs[3] = 1;		CarIDs[4] = 9;		CarIDs[5] = 10;
		CarIDs[6] = 7;		CarIDs[7] = 28;		CarIDs[8] = 3;
		CarIDs[9] = 22;		CarIDs[10] = 16;	CarIDs[11] = 15;
		CarIDs[12] = 2;		CarIDs[13] = 19;	CarIDs[14] = 5;
		
		MainMenu->Unselect(CAR);

		DestroyModel();

		MainGameInfo.CarTypes[0] = CarIDs[CurrentCar];

		Destroy();

		GlobalCar = NULL;

		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(CAR);
	}
	return 0;
}

// Not needed as we're using the default stuff.
void SelectCarScreen::DrawBackground()
{
}

SelectCarScreen::~SelectCarScreen()
{
}

void SelectCarScreen::Destroy()
{
	LeftArrow->Hide();	RightArrow->Hide();
	UpArrow->Hide();	DownArrow->Hide();

	delete GearBoxWord[0]; delete GearBoxWord[1];

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	DeleteHelp();

	return;
}