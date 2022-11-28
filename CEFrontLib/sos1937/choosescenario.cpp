// $Header$

// $Log$
// Revision 1.21  2000-05-05 12:17:15+01  img
// Build 19
//
// Revision 1.20  2000-05-04 17:39:26+01  img
// Build 18
//
// Revision 1.19  2000-04-28 11:09:07+01  jcf
// <>
//
// Revision 1.18  2000-04-26 10:08:55+01  img
// <>
//
// Revision 1.17  2000-04-23 20:48:41+01  img
// Bug fixes
//
// Revision 1.16  2000-04-22 18:54:24+01  img
// bug fix
//
// Revision 1.15  2000-04-22 18:26:03+01  img
// Added C button to joystick support
//
// Revision 1.14  2000-04-22 13:12:47+01  jcf
// <>
//
// Revision 1.13  2000-04-20 20:18:40+01  img
// memory leak fix
//
// Revision 1.12  2000-04-19 21:37:59+01  img
// Localisation.
//
// Revision 1.11  2000-04-17 16:04:21+01  img
// Bug fix
//
// Revision 1.10  2000-04-07 14:38:04+01  img
// Help support, generic background support
//
// Revision 1.9  2000-04-05 17:45:45+01  img
// Loads more data
//
// Revision 1.8  2000-03-30 14:47:58+01  img
// More preloadings
//
// Revision 1.7  2000-03-28 17:55:25+01  img
// Scenario loading works.
//
// Revision 1.6  2000-03-22 12:47:11+00  jjs
// <>
//
// Revision 1.5  2000-03-21 17:28:23+00  img
// Pre-loading more now...
//
// Revision 1.4  2000-03-20 18:00:49+00  img
// Beta build
//
// Revision 1.3  2000-03-15 09:33:54+00  img
// Sets up player car according to scenario.
//
// Revision 1.2  2000-03-14 14:26:40+00  img
// Arrows added.
//
// Revision 1.1  2000-03-13 18:15:38+00  img
// First working version.
//
// Revision 1.0  2000-03-10 17:25:59+00  img
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

struct Scenario *AllScenarios[3][15], *ChampionshipScenarios[3][15];
extern "C"
{
	void ReadDiskError(char *,int i);
}

static char lastname[256];

FILE *dc_fopen(char *s)
{
	FILE *a;

	strcpy(lastname,s);
	for(int i=0;i<10;i++)
	{
		if(a=fopen(s,"r"))
			return a;

		Sleep(200);
	}
	ReadDiskError(s,10);
	return NULL;
}

size_t dc_fread(void *s,size_t p,size_t q,FILE *a)
{
	unsigned long pos = ftell(a);

	for(int i=0;i<20;i++)
	{
		size_t r = fread(s,p,q,a);
		if(r==q)return r;

		fseek(a,pos,SEEK_SET);
		Sleep(200);
	}
	ReadDiskError(lastname,11);
	return 0;
}
	



ChooseScenarioScreen::ChooseScenarioScreen(Alphabet *DefaultAlphabet)
{
	MainAlphabet = DefaultAlphabet;

	// Reset the counter to zero.
	Counter = 0;

	CurrentScenario = 0;
//	ChooseScenarioText = new TextWord("CHOOSE A SCENARIO", MainAlphabet);
	ChooseScenarioText = new TextWord(TranslateLocale(3), MainAlphabet);
	ChooseScenarioText->Write(400,115,800.0f);

	HighlightScenario(CurrentScenario);

	LeftArrow->Draw(360,180,800.0f);
	RightArrow->Draw(440,180,800.0f);

	ScreenHeaders[HEADER_SCENARIO]->Draw();

//	SetHelpText("USE DIRECTION PAD TO CHOOSE A SCENARIO","PRESS 'A' TO SELECT");
	SetHelpText(TranslateLocale(4),TranslateLocale(1));

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ChooseScenarioScreen::HighlightScenario(int ThisScenario)
{
	ScenarioName = new TextWord(AllScenarios[MainGameInfo.Difficulty][ThisScenario]->Name, MainAlphabet);
	ScenarioName->Write(400,160,1200.0f);
	WriteDescription(ThisScenario);
}

void ChooseScenarioScreen::LowlightScenario(int ThisScenario)
{
	for (int i=0 ; i<10 ; i++)
	{
		if (ScenarioDetails[i])
			delete ScenarioDetails[i];
	}
	delete ScenarioName;
}

void ChooseScenarioScreen::WriteDescription(int ThisScenario)
{
	int Length;
	char *Pointer;

	Pointer = AllScenarios[MainGameInfo.Difficulty][ThisScenario]->Description;
	
	for (int i=0 ; i<10 ; i++)
	{
		*ScenarioLines[i] = 0;
		if (*Pointer && !i ||  *(Pointer-1) && i)
		{
			Length = 0;
			for (int j=0 ; j<48 ; j++)
			{
				if (*(Pointer+j) == ' ')
					Length = j;
				if (*(Pointer+j) == 0)
				{
					Length = j;
					j = 60;
				}
			}
			// Length now holds the length up to the last space before the 23rd character.
			strncpy(ScenarioLines[i], Pointer, Length);
			ScenarioLines[i][Length] = 0;
			Pointer += Length+1;
		}
	}
	for (i=0 ; i<10 ; i++)
	{
		ScenarioDetails[i] = NULL;
		if (*ScenarioLines[i])
		{
			ScenarioDetails[i] = new TextWord(ScenarioLines[i], MainAlphabet);
			ScenarioDetails[i]->Justify(JUSTIFY_LEFT);
			ScenarioDetails[i]->Write(200,200+i*16, 1700.0f);
		}
	}
}

#define READFIELD(x,y) \
	Pointer = Buffer; \
	while (*Pointer && (strnicmp( x , Pointer, strlen( x )) || *(Pointer+strlen( x )) != 32)) if ((int)(Pointer = strchr(Pointer, '\n')+1) == 1) break; \
	if ((int)Pointer > 1) { Pointer += strlen( x ) + 1; \
	while (*Pointer==32) Pointer++; \
	strncpy(Data , Pointer, Length = (strchr(Pointer, '\n') - Pointer)); \
	Data[Length] = 0; \
	++Pointer+=Length; \
	AllScenarios[j][i]-> ## y = strdup(Data);}

#define READINT(x,y) \
	Pointer = Buffer; \
	while (*Pointer && (strnicmp( x , Pointer, strlen( x )) || *(Pointer+strlen( x )) != 32)) if ((int)(Pointer = strchr(Pointer, '\n')+1) == 1) break; \
    if ((int)Pointer > 1) { Pointer += strlen( x ) + 1; \
	AllScenarios[j][i]-> ## y = atoi(Pointer); \
	Length = strchr(Pointer, '\n') - Pointer; \
	++Pointer+=Length; }

#define READCHAMPFIELD(x,y) \
	Pointer = Buffer; \
	while (*Pointer && (strnicmp( x , Pointer, strlen( x )) || *(Pointer+strlen( x )) != 32)) if ((int)(Pointer = strchr(Pointer, '\n')+1) == 1) break; \
	if ((int)Pointer > 1) { Pointer += strlen( x ) + 1; \
	while (*Pointer==32) Pointer++; \
	strncpy(Data, Pointer, Length = (strchr(Pointer, '\n') - Pointer)); \
	Data[Length] = 0; \
	++Pointer+=Length; \
	ChampionshipScenarios[j][i]-> ## y = strdup(Data);}

#define READCHAMPINT(x,y) \
	Pointer = Buffer; \
	while (*Pointer && (strnicmp( x , Pointer, strlen( x )) || *(Pointer+strlen( x )) != 32)) if ((int)(Pointer = strchr(Pointer, '\n')+1) == 1) break; \
    if ((int)Pointer > 1) { Pointer += strlen( x ) + 1; \
	ChampionshipScenarios[j][i]-> ## y = atoi(Pointer); \
	Length = strchr(Pointer, '\n') - Pointer; \
	++Pointer+=Length; }

extern int reallanguage;

void LoadAllScenarios()
{
	char Filename[256], Buffer[2048], CarNumber[10], *Pointer=Buffer, Data[1024];
	char ScenarioPathString[32];

	switch (reallanguage)
	{
	default:
	case LG_AMERICAN:
		sprintf(ScenarioPathString, "AmericanScenarios");
		break;
	case LG_ENGLISH:
		sprintf(ScenarioPathString, "Scenarios");
		break;
	case LG_GERMAN:
		sprintf(ScenarioPathString, "GermanScenarios");
		break;
	case LG_FRENCH:
		sprintf(ScenarioPathString, "FrenchScenarios");
		break;
	case LG_SPANISH:
		sprintf(ScenarioPathString, "SpanishScenarios");
		break;
	}

	FILE *CurrentFile;
	int Length;
	
	for (int j=0 ; j<3 ; j++)
	{
		for (int i=0; i<15 ; i++, Pointer=Buffer)
		{
			switch (j)
			{
			default:
			case PDS_EASY:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Single_Race\\Easy\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			case PDS_MEDIUM:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Single_Race\\Medium\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			case PDS_HARD:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Single_Race\\Hard\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			}
		
			if ((CurrentFile = fopen(Filename, "r")))
			{
				AllScenarios[j][i] = new Scenario();
				
				fread(Buffer, 1, 2048, CurrentFile);

				AllScenarios[j][i]->GridPosition=-1;

				READFIELD("Name",Name);
				READFIELD("Description",Description);
				READINT("TrackPosition", GridPosition);
				READFIELD("Track", Track);
				READINT("Laps", Laps);
				READINT("Cars", Cars);
				READINT("SuccessValue", SuccessValue);
				READFIELD("SuccessString", SuccessString);
				READFIELD("FailureString", FailureString);
				READFIELD("Goal", LengthString);

				if (AllScenarios[j][i]->GridPosition <= 0) 
					AllScenarios[j][i]->GridPosition = 1;

				for (int k=0 ; k < AllScenarios[j][i]->Cars ; k++)
				{
					sprintf(CarNumber, "Car%d", k);
					READINT(CarNumber,Car[k]);
				}
				fclose(CurrentFile);
			}
			else
				AllScenarios[j][i] = NULL;
		}
	}
	
	for (j=0 ; j<3 ; j++)
	{
		for (int i=0; i<15 ; i++, Pointer=Buffer)
		{
			switch (j)
			{
			default:
			case PDS_EASY:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Full_Season\\Easy\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			case PDS_MEDIUM:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Full_Season\\Medium\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			case PDS_HARD:
				sprintf(Filename, DISKNAME "\\SOS1937FrontEnd\\%s\\Full_Season\\Hard\\Scenario%d.txt", ScenarioPathString, i+1);
				break;
			}
			
			if ((CurrentFile = fopen(Filename, "r")))
			{
				ChampionshipScenarios[j][i] = new Scenario;
				
				fread(Buffer, 1, 2048, CurrentFile);
				
				READCHAMPFIELD("Name",Name);
				READCHAMPFIELD("Description",Description);
				READCHAMPFIELD("Race",LengthString);
				READCHAMPINT("TrackPosition", GridPosition);
				READCHAMPINT("Cars", Cars);
				READCHAMPINT("SuccessValue", SuccessValue);
				READCHAMPFIELD("SuccessString", SuccessString);
				READCHAMPFIELD("FailureString", FailureString);

				ChampionshipScenarios[j][i]->Length = 0;
				if (!strnicmp(ChampionshipScenarios[j][i]->LengthString, "MEDIUM", 6)) ChampionshipScenarios[j][i]->Length = 1;
				if (!strnicmp(ChampionshipScenarios[j][i]->LengthString, "LONG", 4)) ChampionshipScenarios[j][i]->Length = 2;

				for (int k=0 ; k < 10 ; k++)
				{
					ChampionshipScenarios[j][i]->Car[k] = 0;
					if (k<ChampionshipScenarios[j][i]->Cars)
					{
						sprintf(CarNumber, "Car%d", k);
						READCHAMPINT(CarNumber,Car[k]);
					}
				}
				fclose(CurrentFile);
			}
		}
	}
}

void ChooseScenarioScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ChooseScenarioScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_LEFT || ControlPacket & 1 << CON_X && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		LowlightScenario(CurrentScenario);
		if (--CurrentScenario < 0) CurrentScenario = 14;
		while (!AllScenarios[MainGameInfo.Difficulty][CurrentScenario]) CurrentScenario--;
		HighlightScenario(CurrentScenario);
	}

	if (ControlPacket & 1 << CON_RIGHT || ControlPacket & 1 << CON_Y && (AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
	{
		LowlightScenario(CurrentScenario);
		if (++CurrentScenario > 14) CurrentScenario = 0;
		while (!AllScenarios[MainGameInfo.Difficulty][CurrentScenario])
		{
			CurrentScenario++;
			if (CurrentScenario > 14) CurrentScenario = 0;
		}
		HighlightScenario(CurrentScenario);
	}

	if (ControlPacket & 1 << CON_B)
	{
		Destroy();

		QueuedCommand = 'S';
		QueuedValue = (Screen *) new RaceTypeScreen(MainAlphabet);
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		MainMenu->Unselect(RACE_TYPE);
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';

		MainGameInfo.ScenarioID = CurrentScenario;
		for (int i=0 ; i<10 ; i++)
			MainGameInfo.CarTypes[i] = 0;

		for (i=0 ; i<AllScenarios[MainGameInfo.Difficulty][CurrentScenario]->Cars ; i++)
		{
			MainGameInfo.CarTypes[i] = AllScenarios[MainGameInfo.Difficulty][CurrentScenario]->Car[i];
		}
/*		int a, b=CurrentScenario;
		dprintf("Current Scenario is %d", b);
		for (i=0 ; i<=T_TRIPOLI && stricmp(AllScenarios[MainGameInfo.Difficulty][b]->Track, TrackNumbers[i]) ; i++) { dprintf("Searching Scenario %d for %d", i, b); a=i; }
		MainGameInfo.Track = a;
		dprintf("A: %d", a);
*/
		for (i=0 ; i<9 ; i++)
		{

			if (!stricmp(AllScenarios[MainGameInfo.Difficulty][CurrentScenario]->Track, TrackNumbers[i])
				&& strlen(AllScenarios[MainGameInfo.Difficulty][CurrentScenario]->Track) == strlen(TrackNumbers[i]))
			{
				MainGameInfo.Track = i;
				strcpy(MainGameInfo.TrackName, TrackNumbers[i]);
				dprintf("Set track to %d, %s", i, TrackNumbers[i]);
				i=10;
			}
		}

		if (GlobalTrack && GlobalTrack->GetNumber() != MainGameInfo.Track)
		{
			GlobalTrack = NULL;
		}

		if (GlobalCar && GlobalCar->GetNumber() != MainGameInfo.CarTypes[0])
		{
			GlobalCar = NULL;
		}

		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(RACE_TYPE);
	}

	return 0;
}

// Not needed as we're using the default stuff.
void ChooseScenarioScreen::DrawBackground()
{
}

ChooseScenarioScreen::~ChooseScenarioScreen()
{
}

void ChooseScenarioScreen::Destroy()
{
	LowlightScenario(CurrentScenario);

	delete ChooseScenarioText;

	ScreenHeaders[HEADER_SCENARIO]->Hide();

	LeftArrow->Hide(); RightArrow->Hide();

	DeleteHelp();

	return;
}