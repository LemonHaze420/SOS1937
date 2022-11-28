// $Header

// $Log

/* The race type screen of the game.

	Ian Gledhill 06/02/2000
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"
#include "SOSLogo.h"

#include <archive.h>

extern class ButtonBank *MainMenu;
extern struct GlobalStructure MainGameInfo;
extern IFlashDevice *MemoryCardSlot[8];
int LastSlot=0;
extern bool ReEnumCards;

extern char Selecting[];

MemoryCardChosenScreen::MemoryCardChosenScreen(Alphabet *DefaultAlphabet, int _ChosenCard)
{
	MainAlphabet = DefaultAlphabet;
	ChosenCard = _ChosenCard;

	// Reset the counter to zero.
	Counter = 0;

//	if (ChosenCard % 2  == 0)
		MemoryCard = new FrontEndObject(NULL, "LPMemCardOne");
//	else
//		MemoryCard = new FrontEndObject(NULL, "LPMemCardTwo");

	MemoryCard->Draw(400,200,400.0f);

	MainMenu = new ButtonBank();

	MainMenu->Add(VMOptionsBank[0]);
	MainMenu->Add(VMOptionsBank[1]);

	MainMenu->DrawAll();

	CurrentSelection = VMO_SAVEGAME;

	ScreenHeaders[HEADER_VMOPTIONS]->Draw();

	SetDefaultHelp();

	DeriveLocation();
	CardPosition->Write(400,290,800.0f);

	LoadFilenames();

	MainMenu->Highlight(CurrentSelection);

	if (SlotEmpty[0] && SlotEmpty[1] && SlotEmpty[2] && SlotEmpty[3])
	{
		Mode = MCCSM_SAVING;
		DeleteHelp();
		SetHelpText(TranslateLocale(51)," ");
		MainMenu->Lowlight(VMO_SAVEGAME);
		MainMenu->Select(VMO_SAVEGAME);
		CurrentSelection = LastSlot;
		Highlight();
	}
	else
		Mode = MCCSM_CHOOSING;

	NextCommand = 0;
	DrawVMIcon(-1, Selecting);

	WarningFlagged = false;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void MemoryCardChosenScreen::DeriveLocation()
{
	char LocationString[80];

	sprintf(LocationString, "%s %c %s %c", TranslateLocale(44), (int)(ChosenCard*0.5f)+'A', TranslateLocale(45), ChosenCard%2 + '1');
	CardPosition = new TextWord(LocationString, MainAlphabet);
}

void MemoryCardChosenScreen::LoadFilenames()
{
	char *PlayerNames[4];
	GetDirectory(ChosenCard, PlayerNames);
	int InitialFreeSlots = GetFreeSlots(ChosenCard);
	int FreeSlots = InitialFreeSlots;
	bool SlotAvailable=false;

	for (int i=0 ; i<4 ; i++)
	{
		SlotUsed[i] = false;
		SlotEmpty[i] = false;
		if (!PlayerNames[i])
		{
			if (FreeSlots >= 3)
			{
				SlotEmpty[i] = true;
				PlayerNames[i] = strdup(TranslateLocale(86));
				FreeSlots -= 3;
			}
			else
			{
				PlayerNames[i] = NULL;
			}
		}
		else
			SlotUsed[i] = true;

		if (PlayerNames[i])
		{
			SlotContents[i] = new TextWord(PlayerNames[i], MainAlphabet);
			YellowContents[i] = new TextWord(PlayerNames[i], YellowAlphabet);
			SlotContents[i]->Write  (400, 320+i*20, 1000.0f);
			YellowContents[i]->Write(400, 320+i*20, 1000.0f);
			YellowContents[i]->Hide();

			free(PlayerNames[i]);
			SlotAvailable = true;
		}
		else
		{
			SlotContents[i] = NULL; YellowContents[i] = NULL;
		}
	}

	if (InitialFreeSlots < 3 && !SlotAvailable)
	{
		DeleteHelp();
		SetHelpText(TranslateLocale(46),TranslateLocale(47));
#if defined(JAPAN)
		SetJapanText(0);		
#endif
	}
}

void MemoryCardChosenScreen::SetDefaultHelp()
{
	SetHelpText(TranslateLocale(48),TranslateLocale(1));
}

void MemoryCardChosenScreen::Lowlight()
{
	YellowContents[CurrentSelection]->Hide();
	SlotContents[CurrentSelection]->Write();
}

void MemoryCardChosenScreen::Highlight()
{
	SlotContents[CurrentSelection]->Hide();
	YellowContents[CurrentSelection]->Write();
}

void MemoryCardChosenScreen::Update(struct Instruction *ScreenCommand)
{
	if (NextCommand)
	{
		if (NextCommand & (1 << 7))
			NextCommand &= ~(1 << 7);
		else
		{
			bool Error=false;
			switch (NextCommand)
			{
			case 1:
				// Load
				if (Error=(!LoadMainGameInfo(&MainGameInfo, ChosenCard, CurrentSelection)))
				{
					DeleteHelp();
					SetHelpText(TranslateLocale(49)," ");
				}
				MainMenu->Unselect(VMO_LOADGAME);
				break;
			case 2:
				// Save
				if (Error=(!SaveMainGameInfo(&MainGameInfo, ChosenCard, CurrentSelection)))
				{
					DeleteHelp();
					SetHelpText(TranslateLocale(50)," ");
				}
				MainMenu->Unselect(VMO_SAVEGAME);
				break;
			}
			if (!Error)
			{
				GlobalCar = NULL;
				GlobalTrack = NULL;
				
				Destroy();
				delete MainMenu;
				MainMenu = NULL;
				
				for (int i=0 ; i<8 ; i++)
				{
					if (MemoryCardSlot[i])
					{
						MemoryCardSlot[i]->Release();
						MemoryCardSlot[i] = NULL;
					}
				}
				// Change screen.
				QueuedCommand = 'S';
				
				if (MainGameInfo.RaceType == RT_CHAMPIONSHIP && MainGameInfo.Track != T_TRIPOLI || MainGameInfo.RaceMode != 0)
				{
					MainGameInfo.RaceMode = RM_RACE;
					MainGameInfo.RaceType = 0xFF;
					QueuedValue = (Screen *) new ChampionshipScreen(MainAlphabet);
				}
				else
					QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
				NextCommand = 0;
			}
			else
			{
				NextCommand = 0;
				switch (Mode)
				{
				case MCCSM_SAVING:
					MainMenu->Select(VMO_SAVEGAME);
					break;
				case MCCSM_LOADING:
					MainMenu->Select(VMO_LOADGAME);
					break;
				}
				WarningFlagged = true;
			}
		}
	}
	
	if (Counter > -1)
	{
		MemoryCard->Rotate(80.0f+(float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
		Counter++;
	}
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int MemoryCardChosenScreen::ControlPressed(int ControlPacket)
{
	if (NextCommand)
		return 0;

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		if (Mode == MCCSM_CHOOSING)
		{
			if (SlotContents[0] || SlotContents[1] || SlotContents[2] || SlotContents[3])
			{
				switch (CurrentSelection)
				{
				case VMO_LOADGAME:
					DeleteHelp();
					SetHelpText(TranslateLocale(51)," ");
					Mode = MCCSM_LOADING;
					MainMenu->Lowlight(VMO_LOADGAME);
					MainMenu->Select(VMO_LOADGAME);
					break;
				case VMO_SAVEGAME:
					DeleteHelp();
					SetHelpText(TranslateLocale(51)," ");
					Mode = MCCSM_SAVING;
					MainMenu->Lowlight(VMO_SAVEGAME);
					MainMenu->Select(VMO_SAVEGAME);
					break;
				}
				
				CurrentSelection = LastSlot;
				Highlight();
			}
		}
		else
		{
			switch (Mode)
			{
			case MCCSM_LOADING:
				if (SlotEmpty[CurrentSelection])
					break;
				DeleteHelp();
				SetHelpText(TranslateLocale(52)," ");
				NextCommand = 1 | (1 << 7);
				LastSlot = CurrentSelection;
				break;
			case MCCSM_SAVING:
				if (SlotUsed[CurrentSelection])
				{
					DeleteHelp();
					SetHelpText(TranslateLocale(53),TranslateLocale(54));
#if defined(JAPAN)
					SetJapanText(1);
#endif
					Mode = MCCSM_CONFIRMING;
				}
				else
				{
					DeleteHelp();
					SetHelpText(TranslateLocale(55)," ");
#if defined(JAPAN)
					SetJapanText(2);
#endif
					NextCommand = 2 | (1 << 7);
				}
				LastSlot = CurrentSelection;
				break;
			case MCCSM_CONFIRMING:
				DeleteHelp();
				SetHelpText(TranslateLocale(55)," ");
#if defined(JAPAN)
				SetJapanText(2);
#endif
				NextCommand = 2 | (1 << 7);
				Mode = MCCSM_SAVING;
				break;
			}

		}
	}
	
	// Show the warning message if necessary.
	if (ReEnumCards && WarningFlagged)
		Sleep(4000);

	if (ControlPacket & 1 << CON_B || ReEnumCards)
	{
		if (!ReEnumCards && Mode != MCCSM_CHOOSING && !(Mode == MCCSM_SAVING && SlotEmpty[0] && SlotEmpty[1] && SlotEmpty[2] && SlotEmpty[3]))
		{
			switch (Mode)
			{
			case MCCSM_LOADING:
				MainMenu->Unselect(VMO_LOADGAME);
				MainMenu->Highlight(VMO_LOADGAME);
				Lowlight();
				CurrentSelection = VMO_LOADGAME;
				Mode = MCCSM_CHOOSING;	
				break;
			case MCCSM_SAVING:
				MainMenu->Unselect(VMO_SAVEGAME);
				MainMenu->Highlight(VMO_SAVEGAME);
				Lowlight();
				CurrentSelection = VMO_SAVEGAME;
				Mode = MCCSM_CHOOSING;	
				break;
			case MCCSM_CONFIRMING:
				DeleteHelp();
				SetDefaultHelp();
				Mode = MCCSM_SAVING;
				break;
			}
		}
		else
		{
			if (Mode != MCCSM_CHOOSING && Mode != MCCSM_LOADING)
			{
				MainMenu->Unselect(VMO_SAVEGAME);
			}
			else if (Mode == MCCSM_LOADING)
				MainMenu->Unselect(VMO_LOADGAME);
			else
				MainMenu->Lowlight(CurrentSelection);
			Destroy();
			
			delete MainMenu;
			MainMenu = NULL;
			
			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new MemoryCardScreen(MainAlphabet);
		}
	}

	if (ControlPacket & 1 << CON_THROTTLE)
	{
		if (Mode != MCCSM_CHOOSING)
		{
			Lowlight();
			do
			{	if (--CurrentSelection < 0) CurrentSelection = 3;}
			while (!SlotContents[CurrentSelection]);
			
			Highlight();		
		}
		else
		{
			MainMenu->Lowlight(CurrentSelection);
			if (--CurrentSelection < VMO_SAVEGAME ) CurrentSelection = VMO_LOADGAME;
			MainMenu->Highlight(CurrentSelection);
		}
	}

	if (ControlPacket & 1 << CON_BRAKE)
	{
		if (Mode != MCCSM_CHOOSING)
		{
			Lowlight();
			do
			{	if (++CurrentSelection > 3) CurrentSelection = 0;}
			while (!SlotContents[CurrentSelection]);
			Highlight();		
		}
		else
		{
			MainMenu->Lowlight(CurrentSelection);
			if (++CurrentSelection > VMO_LOADGAME) CurrentSelection = VMO_SAVEGAME;
			MainMenu->Highlight(CurrentSelection);
		}
	}

	return 0;
}

// Not needed as we're using the default stuff.
void MemoryCardChosenScreen::DrawBackground()
{
	DrawFullScreen2D((DWORD *)VMOptionsBackdrop, true);
}

MemoryCardChosenScreen::~MemoryCardChosenScreen()
{
}

void MemoryCardChosenScreen::Destroy()
{
	Counter = -1;

	delete MemoryCard;
	delete CardPosition;

	ScreenHeaders[HEADER_VMOPTIONS]->Hide();

	DeleteHelp();

	for (int i=0 ; i<4 ; i++)
	{
		if (SlotContents[i])
			delete SlotContents[i]; delete YellowContents[i];
	}

	DrawVMIcon(-1,SOSLogo);

	return;
}