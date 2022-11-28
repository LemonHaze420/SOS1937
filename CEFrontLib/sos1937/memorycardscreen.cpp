// $HEADER$

// $LOG$

/* The Memory Card screen of the game.

	Ian Gledhill 14/02/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <windows.h>

#include "Selecting.h"

extern class ButtonBank *MainMenu;
extern int AllControllerTypes[];

#include "SOSLogo.h"

IFlashDevice *MemoryCardSlot[8];
FrontEndObject *MemoryCardObject[8];
int LastCard=0;

BOOL CALLBACK MemoryScreen_FlashEnumerationProc( LPCMAPLEDEVICEINSTANCE lpcmapledevinstance, LPVOID lpContext )
{
	HRESULT hr;
	
	dprintf("Found memory at port %d device %d", lpcmapledevinstance->dwPort, lpcmapledevinstance->dwDevNum);
	
	int ID=lpcmapledevinstance->dwPort*2+(lpcmapledevinstance->dwDevNum - 1);
	
	dprintf("Adding memory slot %d", ID);

	hr = MapleCreateDevice( &lpcmapledevinstance->guidDevice, (struct IUnknown **) &MemoryCardSlot[ID]);
	if (hr)
	{
		MemoryCardSlot[ID] = NULL;
		return true;
	}
	hr = MemoryCardSlot[ID]->CheckFormat();
	
	if (!hr) dprintf("Card is formatted...");
	else NonFatalError("Card is not formatted!!!");
	
	FSDEVICEDESC CardDescription;
	CardDescription.dwSize = sizeof(FSDEVICEDESC);
	CardDescription.dwFlags = FSDD_TOTAL_BLOCKS | FSDD_FREE_BLOCKS | FSDD_BLOCK_SIZE | FSDD_COLOR_VMS | FSDD_VOLUME_ICON | FSDD_DATE_TIME_CREATED;
	hr= MemoryCardSlot[ID]->GetDeviceDesc( &CardDescription);
	if (hr) NonFatalError("Error reading card properties port %d device %d", lpcmapledevinstance->dwPort, lpcmapledevinstance->dwDevNum);
	
	FSDATETIME CardDate = CardDescription.fsdatetimeCreated;
	dprintf("Memory card holds %d total blocks, %d free blocks, with a block size of %d bytes. Created on %d/%d/%2d%02d at %d:%d", CardDescription.dwTotalBlocks,
		CardDescription.dwFreeBlocks, CardDescription.dwBlockSize, CardDate.bDay, CardDate.bMonth, CardDate.bYearTop, CardDate.bYearBottom, 
		CardDate.bHour, CardDate.bMinute);
	
	return true;
}

MemoryCardScreen::MemoryCardScreen(Alphabet *DefaultAlphabet)
{
	// Save the current button that's used.
	PreviousButton = 0;

	MainAlphabet = DefaultAlphabet;

	// Destroy the button bank.
	if (MainMenu) delete MainMenu;
	MainMenu = NULL;

	MCS_hNewDeviceEvent = CreateEvent(NULL, false, false, TEXT("MAPLE_NEW_DEVICE"));
	MCS_hRemovedDeviceEvent = CreateEvent(NULL, false, false, TEXT("MAPLE_DEVICE_REMOVED"));
	
	if (!MCS_hNewDeviceEvent)
		FatalError("Error creating hNewDeviceEvent");
	if (!MCS_hRemovedDeviceEvent)
		FatalError("Error creating hRemovedDeviceEvent");

	for (int i=0 ; i<8 ; i++)
	{
		MemoryCardObject[i] = NULL;
		MemoryCardSlot[i] = NULL;
		RotationOfCard[i] = 0;
	}

	// Set the first card off.
	CurrentCard = LastCard;

	RotationOfCard[CurrentCard] = 2;

	SetDefaultHelp();

	ScreenHeaders[HEADER_VMOPTIONS]->Draw();

	char Text[8];
	for (i=0 ; i<8 ; i++)
	{
		sprintf(Text, "%c%c", 'A'+ i/2, '1'+i%2);
		Labels[i] = new TextWord(Text, MainAlphabet);
	}

	AddNewCards();

	// Reset the counter to zero.
	Counter = 0;

	// Don`t use the default background.
	UseDefaultBackdrop = true;
}

void MemoryCardScreen::SetDefaultHelp()
{
	SetHelpText(TranslateLocale(56),TranslateLocale(1));
}

void MemoryCardScreen::AddNewCards()
{
	for (int i=0 ; i<8 ; i++)
	{
		if (MemoryCardSlot[i])
			MemoryCardSlot[i]->Release();
		delete MemoryCardObject[i];
		MemoryCardSlot[i] = NULL;
		MemoryCardObject[i] = NULL;
	}

	MapleEnumerateDevices(MDT_STORAGE, MemoryScreen_FlashEnumerationProc, NULL, 0); 

	for (i=0 ; i<8 ; i++)
	{
		if (MemoryCardSlot[i])
		{
//			if (i%2 == 0)
				MemoryCardObject[i]	= new FrontEndObject(NULL, "LPMemCardOne");
//			else
//				MemoryCardObject[i] = new FrontEndObject(NULL, "LPMemCardTwo");

			MemoryCardObject[i]->Draw(210 + (i%4)*80 + ((i%4) > 1 ? 100 : 0) , 200 + (i > 3 ? 120 : 0), 600.0f);
			MemoryCardObject[i]->Rotate(30.0f,0.0f,0.0f);
		}
		else
		{
			MemoryCardObject[i] = new FrontEndObject(NULL, "LPMemCardBlack");
			MemoryCardObject[i]->Draw(210 + (i%4)*80 + ((i%4) > 1 ? 100 : 0) , 200 + (i > 3 ? 120 : 0), 600.0f);
			MemoryCardObject[i]->Rotate(30.0f,0.0f,0.0f);
		}

		Labels[i]->Write(210 + (i%4)*80 + ((i%4) > 1 ? 100 : 0) , 260 + (i > 3 ? 120 : 0), 1000.0f);
	}

	if (!MemoryCardSlot[CurrentCard])
	{
		dprintf("Need new card..");
		int OldCurrentCard = CurrentCard;
		while ((++CurrentCard > 7 || !MemoryCardSlot[CurrentCard]) && CurrentCard != OldCurrentCard) if (CurrentCard > 7) CurrentCard = -1;
		
		dprintf("Current Card is %d", CurrentCard);

		RotationOfCard[OldCurrentCard] = -RotationOfCard[OldCurrentCard];
		RotationOfCard[CurrentCard] = -2;
	}

	RotateCards();

	bool NoCards = true;
	int OnlyCard = -1;

	for (i=0 ; i<8 ; i++)
	{
		if (MemoryCardSlot[i])
		{
			NoCards = false;
			if (OnlyCard == -1)
				OnlyCard = i;
			else
				OnlyCard = 99;
		}
	}

	DeleteHelp();
	if (NoCards)
	{
		SetHelpText(TranslateLocale(57),TranslateLocale(58));
#if defined(JAPAN)
		SetJapanText(3);
#endif
	}
	else
	{
		SetDefaultHelp();
		if (OnlyCard != 99)
			RotationOfCard[OnlyCard] = 2;
	}

	InitialiseMemories();
	DrawVMIcon(-1, Selecting);
}

void MemoryCardScreen::RotateCards()
{
	for (int i=0 ; i<8 ; i++)
	{
		if (RotationOfCard[i])
		{
			if (RotationOfCard[i] >= 50)		// If we've fully rotated and are staying there.
			{
				MemoryCardObject[i]->Rotate((float)(30+RotationOfCard[i]),0.0f,0.0f);
			}
			else if (RotationOfCard[i] > 0)		// If we're on our way towards rotated.
			{
				RotationOfCard[i]+=2;
				MemoryCardObject[i]->Rotate(float(30+RotationOfCard[i]),0.0f,0.0f);
			}
			else if (RotationOfCard[i] < 0)		// If we're on our way back.
			{
				RotationOfCard[i]+=2;
				MemoryCardObject[i]->Rotate(float(30-RotationOfCard[i]),0.0f,0.0f);
			}
		}
	}
}

extern bool ReEnumCards;

void MemoryCardScreen::Update(struct Instruction *ScreenCommand)
{
//	if (WaitForSingleObject(MCS_hNewDeviceEvent, 0) != WAIT_TIMEOUT || 	WaitForSingleObject(MCS_hRemovedDeviceEvent, 0) != WAIT_TIMEOUT)
	if (ReEnumCards)
	{
//		Sleep(1000);
		AddNewCards();
	}

	if (Counter >= 0)
	{
		RotateCards();
		Counter++;
	}
	
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int MemoryCardScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_LEFT || ControlPacket & 1 << CON_X && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		int OldCurrentCard = CurrentCard;
		while ((--CurrentCard < 0 || !MemoryCardSlot[CurrentCard]) && CurrentCard != OldCurrentCard) if (CurrentCard < 0) CurrentCard = 8;
		
		dprintf("Current Card is %d", CurrentCard);

		if (CurrentCard != OldCurrentCard)
		{
			RotationOfCard[OldCurrentCard] = -RotationOfCard[OldCurrentCard];
			RotationOfCard[CurrentCard] = 2;
		}
	}
	if (ControlPacket & 1 << CON_RIGHT || ControlPacket & 1 << CON_Y && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		int OldCurrentCard = CurrentCard;
		while ((++CurrentCard > 7 || !MemoryCardSlot[CurrentCard]) && CurrentCard != OldCurrentCard) if (CurrentCard > 7) CurrentCard = -1;
		
		dprintf("Current Card is %d", CurrentCard);

		if (CurrentCard != OldCurrentCard)
		{
			RotationOfCard[OldCurrentCard] = -RotationOfCard[OldCurrentCard];
			RotationOfCard[CurrentCard] = 2;
		}
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		if (MemoryCardSlot[CurrentCard])
		{
			Destroy();

			QueuedCommand = 'S';
			QueuedValue = (Screen *) new MemoryCardChosenScreen(MainAlphabet, CurrentCard);
			LastCard = CurrentCard;	
		}
	}

	if (ControlPacket & 1 << CON_B)
	{
		Destroy();
		for (int i=0 ; i<8 ; i++)
			if (MemoryCardSlot[i])
				MemoryCardSlot[i]->Release();
	
		// Change screen.
		QueuedCommand = 'S';
		if (MainGameInfo.RaceType == RT_CHAMPIONSHIP && MainGameInfo.Track != T_TRIPOLI)
			QueuedValue = (Screen *) new ChampionshipScreen(MainAlphabet);
		else
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(CONTINUE);
	}

	return 0;
}

// Not needed as we're using the default stuff.
void MemoryCardScreen::DrawBackground()
{
	DrawFullScreen2D((DWORD *)VMOptionsBackdrop, true);
}

MemoryCardScreen::~MemoryCardScreen()
{
}

void MemoryCardScreen::Destroy()
{
	for (int i=0 ; i<8 ; i++)
	{
		delete MemoryCardObject[i];
		delete Labels[i];
	}

	ScreenHeaders[HEADER_VMOPTIONS]->Hide();

	Counter = -1;

	DrawVMIcon(-1, SOSLogo);

	DeleteHelp();

	CloseHandle(MCS_hNewDeviceEvent);
	CloseHandle(MCS_hRemovedDeviceEvent);
	return;
}