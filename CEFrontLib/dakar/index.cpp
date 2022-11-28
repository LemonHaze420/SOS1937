// $Header$

// $Log$
// Revision 1.59  2000-05-26 09:12:33+01  img
// Gotta be final version now...
//
// Revision 1.58  2000-05-16 16:13:24+01  img
// FINAL release build???
//
// Revision 1.57  2000-05-11 14:17:04+01  img
// Smaller start
//
// Revision 1.56  2000-05-11 10:42:48+01  img
// Static "Device removed" message when no controllers left
//
// Revision 1.55  2000-05-09 15:50:53+01  jcf
// FrontEndObject ctors now correct in LoadControllers
//
// Revision 1.54  2000-05-09 12:10:31+01  jcf
// Vibration pack detection in LoadDefaultSettings
//
// Revision 1.53  2000-05-08 18:20:32+01  jjs
// Fixes memory leaks.
//
// Revision 1.52  2000-05-05 18:13:01+01  img
// Build 19
//
// Revision 1.51  2000-05-05 12:17:20+01  img
// Build 19
//
// Revision 1.50  2000-05-04 16:25:30+01  jcf
// Cacheing used for some archives. language archives push/popped in LoadAllCars
//
// Revision 1.49  2000-05-04 11:58:59+01  img
// American Language
//
// Revision 1.48  2000-04-28 11:09:10+01  jcf
// <>
//
// Revision 1.47  2000-04-26 10:08:53+01  img
// <>
//
// Revision 1.46  2000-04-25 11:50:04+01  img
// Bug fixes
//
// Revision 1.45  2000-04-24 20:27:36+01  img
// Bug fixes
//
// Revision 1.44  2000-04-22 18:26:10+01  img
// Added C button to joystick support
//
// Revision 1.43  2000-04-21 19:43:46+01  img
// DestroyScreen() added
//
// Revision 1.42  2000-04-21 12:59:30+01  img
// bug fiz
//
// Revision 1.41  2000-04-20 20:19:57+01  img
// better memory handling
//
// Revision 1.40  2000-04-20 16:23:24+01  img
// Removed L_* #define conflict
//
// Revision 1.39  2000-04-20 15:17:50+01  img
// Memory leak fixed
//
// Revision 1.38  2000-04-20 13:31:22+01  img
// More memory fixes
//
// Revision 1.37  2000-04-20 12:56:47+01  img
// Memory leaks fixed
//
// Revision 1.36  2000-04-19 21:40:49+01  img
// fixes
//
// Revision 1.35  2000-04-19 21:38:06+01  img
// Localisation.
//
// Revision 1.34  2000-04-17 14:53:01+01  img
// Bug fixes
//
// Revision 1.33  2000-04-17 10:11:13+01  jcf
// Progress bars
//
// Revision 1.32  2000-04-17 10:03:21+01  img
// Bug fix
//
// Revision 1.31  2000-04-12 18:11:27+01  img
// bugfixes
//
// Revision 1.30  2000-04-12 12:20:25+01  img
// Force dreampad
//
// Revision 1.29  2000-04-11 16:13:53+01  img
// REP_QUICKRACEOVER implemented
//
// Revision 1.28  2000-04-10 17:20:46+01  img
// Preloads all controllers
//
// Revision 1.27  2000-04-07 17:32:12+01  img
// Force dreampad
//
// Revision 1.26  2000-04-07 15:58:27+01  img
// Controllers loaded here now.  LoadDefaultSettings added.
//
// Revision 1.25  2000-04-07 15:29:13+01  img
// Better management of frontscreen's memory card
//
// Revision 1.24  2000-04-07 14:38:17+01  img
// Help support, generic background support
//
// Revision 1.23  2000-04-05 17:46:54+01  img
// More preloadings
//
// Revision 1.22  2000-04-04 18:39:46+01  img
// Debugging version
//
// Revision 1.21  2000-04-03 14:54:05+01  img
// Preloadings, and no PlayerScreen
//
// Revision 1.20  2000-03-30 14:48:28+01  img
// More preloadings
//
// Revision 1.19  2000-03-28 17:57:01+01  img
// Preloading grid models and bug fixes.
//
// Revision 1.18  2000-03-27 10:26:33+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.17  2000-03-22 12:47:12+00  jjs
// <>
//
// Revision 1.16  2000-03-22 09:59:46+00  jjs
// <>
//
// Revision 1.15  2000-03-22 09:29:09+00  img
// <>
//
// Revision 1.14  2000-03-21 17:28:29+00  img
// Pre-loading more now...
//
// Revision 1.13  2000-03-20 18:01:01+00  img
// Beta build
//
// Revision 1.12  2000-03-09 10:33:01+00  img
// Use 512x512 texture again
//
// Revision 1.11  2000-03-08 10:32:16+00  img
// DefaultAlphabet uses Courier. Also uses 3df backdrops
//
// Revision 1.10  2000-03-07 16:02:55+00  img
// Use re-entry point
//
// Revision 1.9  2000-03-07 14:23:41+00  img
// ReEntryPoint now works
//
// Revision 1.8  2000-03-07 14:02:34+00  img
// Uses pvr files
//
// Revision 1.7  2000-03-06 13:03:17+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.6  2000-02-29 11:28:29+00  jjs
// Added Header and Log lines.
//

/* First screen of the front end
	SOS_Index.cpp

	29/11/1999 */

#include "ConsoleFrontLib.h"

// The main menu of buttons.
// Needs to be static and exist in all screens.
class ButtonBank *MainMenu;
extern IFlashFile *SettingsFile;

FrontEndObject *CarGridModels[15];
FrontEndObject *CarTableModels[15];
FrontEndCar *GlobalCar = NULL, *AllCars[15];
FrontEndTrack *GlobalTrack = NULL;
FrontEndObject *GlobalController=NULL, *GlobalWheel=NULL, *GlobalStick=NULL;
FrontEndObject *ControllerLowModel[3][9], *ControllerHighModel[3][9];
FrontEndObject *RaceTypeModels[6];
FrontEndObject *LeftArrow, *RightArrow, *UpArrow, *DownArrow; 
FrontEndObject *_MemoryCardOne, *_MemoryCardTwo, *_MemoryCardBlack, *GlobalMemoryCard;
FrontEndObject *SoundObjects[2][3];
FrontEndObject *_SlideBar, *_SlideKnob;
FrontEndObject *ScreenHeaders[7];
FrontEndObject *StartButton;
TextWord *StartText;

Button *RaceOptionsBank[7], *ChampionshipBank[7], *SingleRaceABank[3], *VMOptionsBank[2], *ScenarioBank[4];
FrontEndObject *TrackModels[9], *TrackStatsModels[9], *TrackNameModels[9];
Alphabet *YellowAlphabet;
BorisMaterial ChampionshipBackdrop[8], ScenarioBackdrop[8], SingleRaceBackdrop[8], VMOptionsBackdrop[8];
FrontEndObject *EndLetter, *DelLetter, *Cursor, *Space;

bool ShowingStart;

extern void IncrementProgressBar();	// jcf
extern void EndLoadingScreen();
extern int reallanguage;

FSFILEICON SOSFileIcon;

#include "SOSLogo.h"

Game::Game(int ReEntryPoint)
{
	ShowingStart = false;
	
	DrawVMIcon(-1, SOSLogo);
	
	MainMenu = NULL;

	GlobalCar = NULL;
	GlobalTrack = NULL;
	GlobalController = NULL; GlobalWheel = NULL; GlobalStick = NULL;

	for (int i=0 ; i<3 ; i++)
		for (int j=0 ; j<9 ; j++)
		{
			ControllerLowModel[i][j] = NULL;
			ControllerHighModel[i][j] = NULL;
		}

	for (i=0 ; i<6 ; i++)
		RaceTypeModels[i] = NULL;

	TARC_SetCacheMode(TRUE);

	// Load in the default font.

	DefaultAlphabet = new Alphabet(DISKNAME"\\SOS1937FrontEnd\\ArielWhite.tc", 1.0f);
	YellowAlphabet = new Alphabet(DISKNAME"\\SOS1937FrontEnd\\ArielYellow.tc", 1.0f);

	IncrementProgressBar();

	LoadFileIcon();
	LoadBackdrop(DefaultBackdrop, "Generic");
	LoadRaceTypeModels();

	TARC_SetCacheMode(FALSE);

	LoadAllCars();

	LoadController();
	LoadSoundObjects();
	LoadMemoryCards();
	LoadAllButtons();
	LoadAllScenarios();
	LoadAllTracks();
	LoadCarGridModels();
	LoadAllBackdrops();
	LoadLetters();

	TARC_SetCacheMode(FALSE);

	if (!ReEntryPoint)
		MainGameInfo.ControllerID = 0;

	switch (ReEntryPoint)
	{
	default:
	case REP_QUICKRACEOVER:
		CurrentScreen = (Screen *)new StartQuickRaceScreen(DefaultAlphabet);
		break;
	case REP_START:
	case REP_NODEFAULTS:
		LoadDefaultSettings();
		CurrentScreen = (Screen *)new FrontScreen(DefaultAlphabet);
		break;
	case REP_SINGLERACEOVER:
		if (MainGameInfo.RaceMode == RM_RACE)
			CurrentScreen = (Screen *)new SingleRaceOverScreen(DefaultAlphabet);
		else
			CurrentScreen = (Screen *)new SingleRaceScreen(DefaultAlphabet);
		// Force search for a settings file.
		SettingsFile = NULL;
		break;
	case REP_CHAMPIONSHIPRACEOVER:
		if (MainGameInfo.Track != T_MONZA)
			CurrentScreen = (Screen *)new ChampionshipScreen(DefaultAlphabet);
		else
			CurrentScreen = (Screen *)new ChampionshipOverScreen(DefaultAlphabet);
		SettingsFile = NULL;
		break;
	case REP_SCENARIOOVER:
		LoadGlobalCar();
		LoadGlobalTrack();
		CurrentScreen = (Screen *)new ScenarioDetailsScreen(DefaultAlphabet);
		SettingsFile = NULL;
		break;
	}

	arcPush(DISKNAME"\\SOS1937FrontEnd\\CommonButtons.tc");
	LeftArrow = new FrontEndObject(NULL, "ArrowLeft");
	RightArrow = new FrontEndObject(NULL, "ArrowRight");
	UpArrow = new FrontEndObject(NULL, "ArrowUp");
	DownArrow = new FrontEndObject(NULL, "ArrowDown");
	arcPop();
	
	EndLoadingScreen();

	FlashCounter = 70;
}

void Game::LoadFileIcon()
{
	// Palette starts at 54.
	// Data starts at 118

	arcPush(DISKNAME "\\SOS1937FrontEnd\\Icons.tc");

	char Buffer[1000];
	archivestack.ExtractIntoArea("VMFileIcon", FMT_BMP_8, Buffer, 632);
	arcPop();

	SOSFileIcon.bAnimationFrames = 1;

	// Load in the palette.
	char *Pointer = Buffer+54;
	for (int i=0 ; i<16 ; i++)
	{
		SOSFileIcon.palette[i] = MAKE_FSARGB4(15,*(Pointer+2) / 16,*(Pointer+1) / 16,*(Pointer) / 16);
		Pointer+=4;
	}

	int x,y;
	Pointer = Buffer+118;

	for (i=0 ; i<1024 ; i++)
	{
		y = (int)(i/32);
		x = (int)((i - (y*32)) / 2);
		y = 31 - y;
		if (!(i%2))
			SOSFileIcon.pixelsFrame1[y][x] = (*Pointer & 0xF0);
		else
			SOSFileIcon.pixelsFrame1[y][x] |= (*Pointer++ & 0x0F);
	}
}

BOOL CALLBACK
FEVibEnumProc(LPCMAPLEDEVICEINSTANCE pMDI, LPVOID pvContext)
{
	dprintf("FE enumerating vibrator at %lx",pMDI->dwPort);

    if (pMDI->devType == MDT_VIBRATION) // it's a rumble pack and it's there
	{
		MainGameInfo.ControllerID = (unsigned short)(MainGameInfo.ControllerID | (1 << RUMBLE_BIT));
	}

    return TRUE;
}


void Game::LoadDefaultSettings()
{
	MainGameInfo.MagicNumber = SOS_MAGICNUMBER;
	MainGameInfo.ControllerID = 0;
	
	MapleEnumerateDevices(MDT_VIBRATION,FEVibEnumProc,NULL,0);

	for (int i=0 ; i<10 ; i++)
	{
		MainGameInfo.ChampionshipPoints[i] = 0;
		MainGameInfo.ChampionshipPosition[i] = i-1;
	}
	MainGameInfo.ChampionshipPosition[0] = 9;

	if (ReEntryPoint == REP_NODEFAULTS)
		return;

	MainGameInfo.MusicVolume = 90;
	MainGameInfo.EngineVolume = 100;
	MainGameInfo.EffectsVolume = 100;

	strcpy(MainGameInfo.PlayerName[0], "PLAYER");
	MainGameInfo.Track = T_MONTANA;
	strcpy(MainGameInfo.CarName[0], "Mercedes154_15");
	MainGameInfo.CarTypes[0]=15;
	MainGameInfo.CarTypes[1]=4;
	
	for (i=0 ; i<2 ; i++)
	{
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_ACCELERATE] = GS_RTRIGGER;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_BRAKE] = GS_LTRIGGER;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_GEARUP] = GS_A;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_GEARDOWN] = GS_X;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_REARVIEW] = GS_Y;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_VIEW] = GS_B;
		MainGameInfo.ButtonDefines[i][CCT_DREAMPAD][GS_STATUS] = GS_UP;
		
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_ACCELERATE] = GS_RTRIGGER;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_BRAKE] = GS_LTRIGGER;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_GEARUP] = GS_Y;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_GEARDOWN] = GS_X;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_REARVIEW] = GS_B;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_VIEW] = GS_A;
		MainGameInfo.ButtonDefines[i][CCT_WHEEL][GS_STATUS] = GS_START;
		
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_ACCELERATE] = GS_A;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_BRAKE] = GS_B;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_GEARUP] = GS_Y;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_GEARDOWN] = GS_X;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_REARVIEW] = GS_Z;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_VIEW] = GS_C;
		MainGameInfo.ButtonDefines[i][CCT_STICK][GS_STATUS] = GS_START;
	}
	MainGameInfo.RaceType = RT_QUICK;
	
	MainGameInfo.Difficulty = PDS_EASY;
	MainGameInfo.GearBox[0] = GEARBOX_AUTOMATIC;
	MainGameInfo.GearBox[1] = GEARBOX_AUTOMATIC;
	MainGameInfo.ScenarioID = 0;
}

void Game::LoadRaceTypeModels()
{
	arcPush(DISKNAME"\\SOS1937FrontEnd\\RaceTypeIcons.tc");
	RaceTypeModels[RT_CHAMPIONSHIP] = new FrontEndObject(NULL, "Championship");
	RaceTypeModels[RT_QUICK]		= new FrontEndObject(NULL, "QuickRace");
	RaceTypeModels[RT_SCENARIO]		= new FrontEndObject(NULL, "Snenario");
	RaceTypeModels[RT_SINGLE]		= new FrontEndObject(NULL, "SingleRace");
	arcPop();

}

void Game::LoadMemoryCards()
{
	arcPush(DISKNAME"\\SOS1937FrontEnd\\MemoryCardScreen.tc");
	_MemoryCardOne = new FrontEndObject(NULL, "LPMemCardOne");
	_MemoryCardTwo = new FrontEndObject(NULL, "LPMemCardTwo");
	_MemoryCardBlack = new FrontEndObject(NULL, "LPMemCardBlack");
	arcPop();
	
	GlobalMemoryCard = new FrontEndObject(NULL, "LPMemCardOne");

	IncrementProgressBar();
}

void Game::LoadLetters()
{
	switch (reallanguage)
	{
	case LG_AMERICAN:
	case LG_ENGLISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\EnglishPlayerDetailsScreen.tc");
		break;
	case LG_FRENCH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\FrenchPlayerDetailsScreen.tc");
		break;
	case LG_GERMAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\GermanPlayerDetailsScreen.tc");
		break;
	case LG_SPANISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SpanishPlayerDetailsScreen.tc");
		break;
	}
	arcPush(DISKNAME"\\SOS1937FrontEnd\\PlayerDetailsScreen.tc");
	EndLetter = new FrontEndObject(NULL, "End", 1.0f, NULL, false);
	DelLetter = new FrontEndObject(NULL, "Del", 1.0f, NULL, false);
	Cursor = new FrontEndObject(NULL, "Cursor");
	Space = new FrontEndObject(NULL, "Spc");
	arcPop();
	arcPop();
	IncrementProgressBar();
}

void Game::LoadAllTracks()
{
	arcPush(DISKNAME "\\SOS1937FrontEnd\\FrontEndTracks.tc");
	arcPush(DISKNAME "\\SOS1937FrontEnd\\GlobalSelectionTexts.tc");
	switch (reallanguage)
	{
	case LG_AMERICAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\AmericanSelectionTexts.tc");
		break;
	case LG_ENGLISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SelectionTexts.tc");
		break;
	case LG_FRENCH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\FrenchSelectionTexts.tc");
		break;
	case LG_GERMAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\GermanSelectionTexts.tc");
		break;
	case LG_SPANISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SpanishSelectionTexts.tc");
		break;
	}

	char Buffer[256];
	for (int i=0 ; i<9 ; i++)
	{
		TrackModels[i] = new FrontEndObject(NULL, TrackNumbers[i]);

		sprintf(Buffer, "%sStats", TrackNumbers[i]);
		TrackStatsModels[i] = new FrontEndObject(NULL, Buffer); 

		sprintf(Buffer, "%sText", TrackNumbers[i]);
		TrackNameModels[i] = new FrontEndObject(NULL, Buffer);
		IncrementProgressBar();
	}
	arcPop();
	arcPop();
	arcPop();
}

void Game::LoadAllCars()
{
	int CarIDs[15];
	
	CarIDs[0] = 12;		CarIDs[1] = 66;		CarIDs[2] = 8;
	CarIDs[3] = 1;		CarIDs[4] = 9;		CarIDs[5] = 10;
	CarIDs[6] = 7;		CarIDs[7] = 28;		CarIDs[8] = 3;
	CarIDs[9] = 22;		CarIDs[10] = 16;	CarIDs[11] = 15;
	CarIDs[12] = 2;		CarIDs[13] = 19;	CarIDs[14] = 5;

	arcPush(DISKNAME "\\SOS1937FrontEnd\\GlobalSelectionTexts.tc");
	switch (reallanguage)
	{
	case LG_AMERICAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\AmericanSelectionTexts.tc");
		break;
	case LG_ENGLISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SelectionTexts.tc");
		break;
	case LG_FRENCH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\FrenchSelectionTexts.tc");
		break;
	case LG_GERMAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\GermanSelectionTexts.tc");
		break;
	case LG_SPANISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SpanishSelectionTexts.tc");
		break;
	}

	for (int i=0 ; i<15 ; i++)
	{
		AllCars[i] = new FrontEndCar(CarIDs[i]);
		IncrementProgressBar();
	}

	arcPop();
	arcPop();
}

void Game::LoadGlobalCar()
{
	int CarIDs[15];
	
	CarIDs[0] = 12;		CarIDs[1] = 66;		CarIDs[2] = 8;
	CarIDs[3] = 1;		CarIDs[4] = 9;		CarIDs[5] = 10;
	CarIDs[6] = 7;		CarIDs[7] = 28;		CarIDs[8] = 3;
	CarIDs[9] = 22;		CarIDs[10] = 16;	CarIDs[11] = 15;
	CarIDs[12] = 2;		CarIDs[13] = 19;	CarIDs[14] = 5;
	
	for (int i=0 ; i<=15 && strnicmp(CarNumbers[CarIDs[i]-1], CarNumbers[MainGameInfo.CarTypes[0]-1], strchr(CarNumbers[CarIDs[i]-1], '_')-CarNumbers[CarIDs[i]-1]); i++);
	GlobalCar = AllCars[i];
	GlobalCar->SetCarPosition(400,230,9.0f);
	GlobalCar->SetNamePosition(420,117,500.0f);
	GlobalCar->SetGraphPosition(425,365,700.0f);
}

void Game::LoadGlobalTrack()
{
	GlobalTrack = new FrontEndTrack(MainGameInfo.Track);
	GlobalTrack->SetTrackPosition(395,230,18.0f);
	GlobalTrack->SetNamePosition(415,115,900.0f);
	GlobalTrack->SetGraphPosition(425,365,700.0f);
}

#define POSITION_1 133
#define POSITION_2 172
#define POSITION_3 212
#define POSITION_4 251
#define POSITION_5 291
#define POSITION_6 332
#define POSITION_7 372
#define POSITION_Z 950.0f

void Game::LoadAllButtons()
{
	// Load in the constant objects, such as buttons etc.
#if defined(UNDER_CE)
	switch (reallanguage)
	{
	case LG_AMERICAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\AmericanButtons.tc");
		break;
	case LG_ENGLISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\AllButtons.tc");
		break;
	case LG_FRENCH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\FrenchButtons.tc");
		break;
	case LG_GERMAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\GermanButtons.tc");
		break;
	case LG_SPANISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SpanishButtons.tc");
		break;
	}
#else
	arcPush(".\\Archives\\MainScreenButtons.tc");
#endif
	RaceOptionsBank[0] = new Button("RaceOptionsButton1", 0.68f, 0, POSITION_1, POSITION_Z);
	RaceOptionsBank[1] = new Button("RaceOptionsButton2", 0.68f, 0, POSITION_2, POSITION_Z);
	RaceOptionsBank[2] = new Button("RaceOptionsButton3", 0.68f, 0, POSITION_3, POSITION_Z);
	RaceOptionsBank[3] = new Button("RaceOptionsButton4", 0.68f, 0, POSITION_4, POSITION_Z);
	RaceOptionsBank[4] = new Button("RaceOptionsButton5", 0.68f, 0, POSITION_5, POSITION_Z);
	RaceOptionsBank[5] = new Button("RaceOptionsButton6", 0.68f, 0, POSITION_6, POSITION_Z);
	RaceOptionsBank[6] = new Button("RaceOptionsButton7", 0.68f, 0, POSITION_7, POSITION_Z);

	ChampionshipBank[0] = new Button("ChampAButton1", 0.68f, 0, POSITION_1, POSITION_Z);
	ChampionshipBank[1] = new Button("ChampAButton2", 0.68f, 0, POSITION_2, POSITION_Z);
	ChampionshipBank[2] = new Button("ChampAButton3", 0.68f, 0, POSITION_3, POSITION_Z);
	ChampionshipBank[3] = new Button("ChampAButton4", 0.68f, 0, POSITION_4, POSITION_Z);
	ChampionshipBank[4] = new Button("ChampAButton5", 0.68f, 0, POSITION_5, POSITION_Z);
	ChampionshipBank[5] = new Button("ChampAButton6", 0.68f, 0, POSITION_6, POSITION_Z);
	ChampionshipBank[6] = new Button("RaceOptionsButton7", 0.68f, 0, POSITION_7, POSITION_Z);

	SingleRaceABank[0] = new Button("SingleRaceAButton1", 0.68f, 0, POSITION_1, POSITION_Z);
	SingleRaceABank[1] = new Button("SingleRaceAButton2", 0.68f, 0, POSITION_2, POSITION_Z);
	SingleRaceABank[2] = new Button("SingleRaceAButton3", 0.68f, 0, POSITION_3, POSITION_Z);

	VMOptionsBank[0] = new Button("VMOptionsButton3", 0.68f, 0, POSITION_3, POSITION_Z);
	VMOptionsBank[1] = new Button("VMOptionsButton4", 0.68f, 0, POSITION_4, POSITION_Z);

	ScenarioBank[0] = new Button("ScenarioAButton1", 0.68f, 0, POSITION_1, POSITION_Z);
	ScenarioBank[1] = new Button("ScenarioAButton2", 0.68f, 0, POSITION_2, POSITION_Z);
	ScenarioBank[2] = new Button("ScenarioAButton3", 0.68f, 0, POSITION_3, POSITION_Z);
	ScenarioBank[3] = new Button("ScenarioAButton4", 0.68f, 0, POSITION_4, POSITION_Z);

	IncrementProgressBar();
	arcPop();
}

extern void ConvertCarToDescription(char *, char *);
extern char *CarNumbers[100];

void Game::LoadCarGridModels()
{
	arcPush(DISKNAME "\\SOS1937FrontEnd\\GridModels.tc");

	CarGridModels[0] = new FrontEndObject(NULL, "AlfaP3Grid");
	CarGridModels[1] = new FrontEndObject(NULL, "Alfa12CGrid");
	CarGridModels[2] = new FrontEndObject(NULL, "AlfaBimGrid");
	CarGridModels[3] = new FrontEndObject(NULL, "AutoCGrid");
	CarGridModels[4] = new FrontEndObject(NULL, "AutoDGrid");
	CarGridModels[5] = new FrontEndObject(NULL, "BonnerGrid");
	CarGridModels[6] = new FrontEndObject(NULL, "Bugatti35BGrid");
	CarGridModels[7] = new FrontEndObject(NULL, "Bugatti59Grid");
	CarGridModels[8] = new FrontEndObject(NULL, "DuesenbergGrid");
	CarGridModels[9] = new FrontEndObject(NULL, "ERAGrid");
	CarGridModels[10] = new FrontEndObject(NULL, "Merc125Grid");
	CarGridModels[11] = new FrontEndObject(NULL, "Merc154Grid");
	CarGridModels[12] = new FrontEndObject(NULL, "MillerGrid");
	CarGridModels[13] = new FrontEndObject(NULL, "NapierGrid");
	CarGridModels[14] = new FrontEndObject(NULL, "WatsonGrid");
	
	arcPop();

	arcPush(DISKNAME "\\SOS1937FrontEnd\\ReducedCars.tc");
	CarTableModels[0] = new FrontEndObject(NULL, "AlfaP3Side");
	CarTableModels[1] = new FrontEndObject(NULL, "Alfa12CSide");
	CarTableModels[2] = new FrontEndObject(NULL, "AlfaBimSide");
	CarTableModels[3] = new FrontEndObject(NULL, "AutoCSide");
	CarTableModels[4] = new FrontEndObject(NULL, "AutoDSide");
	CarTableModels[5] = new FrontEndObject(NULL, "BonnerSide");
	CarTableModels[6] = new FrontEndObject(NULL, "Bugatti35BSide");
	CarTableModels[7] = new FrontEndObject(NULL, "Bugatti59Side");
	CarTableModels[8] = new FrontEndObject(NULL, "DuesenbergSide");
	CarTableModels[9] = new FrontEndObject(NULL, "ERASide");
	CarTableModels[10] = new FrontEndObject(NULL, "Merc125Side");
	CarTableModels[11] = new FrontEndObject(NULL, "Merc154Side");
	CarTableModels[12] = new FrontEndObject(NULL, "MillerSide");
	CarTableModels[13] = new FrontEndObject(NULL, "NapierSide");
	CarTableModels[14] = new FrontEndObject(NULL, "WatsonSide");
	arcPop();
	IncrementProgressBar();

}

void Game::LoadController()
{
	arcPush(DISKNAME"\\SOS1937FrontEnd\\ControllerOptionsModels.tc");
	GlobalController = new FrontEndObject(NULL, "Dreampad");
	ControllerLowModel[CCT_DREAMPAD][GS_ANALOGUE] = new FrontEndObject(NULL, "AnalogueBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_DIRECTION] = new FrontEndObject(NULL, "DirectionBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_A] = new FrontEndObject(NULL, "ABasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_B] = new FrontEndObject(NULL, "BBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_X] = new FrontEndObject(NULL, "XBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_Y] = new FrontEndObject(NULL, "YBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_LTRIGGER] = new FrontEndObject(NULL, "LTriggerBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_RTRIGGER] = new FrontEndObject(NULL, "RTriggerBasic", 1.0f, GlobalController, true);
	ControllerLowModel[CCT_DREAMPAD][GS_START] = NULL;//new FrontEndObject(NULL, "StartBasic", 1.0f, GlobalController, true);
	IncrementProgressBar();

	ControllerHighModel[CCT_DREAMPAD][GS_ANALOGUE] = NULL;//new FrontEndObject(NULL, "AnalogueHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_DIRECTION] = new FrontEndObject(NULL, "DirectionHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_A] = new FrontEndObject(NULL, "AHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_B] = new FrontEndObject(NULL, "BHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_X] = new FrontEndObject(NULL, "XHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_Y] = new FrontEndObject(NULL, "YHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_LTRIGGER] = new FrontEndObject(NULL, "LTriggerHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_RTRIGGER] = new FrontEndObject(NULL, "RTriggerHiLite", 1.0f, GlobalController, true);
	ControllerHighModel[CCT_DREAMPAD][GS_START] = NULL;
	IncrementProgressBar();


	GlobalWheel = new FrontEndObject(NULL, "Wheel");
//	GlobalWheel = new FrontEndObject(NULL, "Dreampad");
	ControllerLowModel[CCT_WHEEL][GS_A] = new FrontEndObject(NULL, "ABasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_B] = new FrontEndObject(NULL, "BBasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_X] = new FrontEndObject(NULL, "MinusBasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_Y] = new FrontEndObject(NULL, "PlusBasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_LTRIGGER] = new FrontEndObject(NULL, "LPaddleBasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_RTRIGGER] = new FrontEndObject(NULL, "RPaddleBasicWheel", 1.0f, GlobalWheel, true);
	ControllerLowModel[CCT_WHEEL][GS_START] = new FrontEndObject(NULL, "StartBasicWheel", 1.0f, GlobalWheel, true);
	IncrementProgressBar();

	ControllerHighModel[CCT_WHEEL][GS_A] = new FrontEndObject(NULL, "AHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_B] = new FrontEndObject(NULL, "BHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_X] = new FrontEndObject(NULL, "MinusHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_Y] = new FrontEndObject(NULL, "PlusHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_LTRIGGER] = new FrontEndObject(NULL, "LPaddleHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_RTRIGGER] = new FrontEndObject(NULL, "RPaddleHiLiteWheel", 1.0f, GlobalWheel, true);
	ControllerHighModel[CCT_WHEEL][GS_START] = NULL;//new FrontEndObject(NULL, "StartHiLiteWheel", 1.0f, GlobalWheel, true);
	IncrementProgressBar();
	GlobalWheel->Hide();

	GlobalStick = new FrontEndObject(NULL, "JoystickJoystickBody");
//	GlobalStick = new FrontEndObject(NULL, "Dreampad");
	ControllerLowModel[CCT_STICK][GS_A] = new FrontEndObject(NULL, "JoystickAButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_B] = new FrontEndObject(NULL, "JoystickBButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_C] = new FrontEndObject(NULL, "JoystickCButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_X] = new FrontEndObject(NULL, "JoystickXButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_Y] = new FrontEndObject(NULL, "JoystickYButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_Z] = new FrontEndObject(NULL, "JoystickZButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_START] = new FrontEndObject(NULL, "JoystickStartButtonBasic", 1.0f, GlobalStick, true);
	ControllerLowModel[CCT_STICK][GS_DIRECTION] = new FrontEndObject(NULL, "JoystickStickBasic", 1.0f, GlobalStick, true);
	IncrementProgressBar();

	ControllerHighModel[CCT_STICK][GS_A] = new FrontEndObject(NULL, "JoystickAButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_B] = new FrontEndObject(NULL, "JoystickBButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_C] = new FrontEndObject(NULL, "JoystickCButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_X] = new FrontEndObject(NULL, "JoystickXButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_Y] = new FrontEndObject(NULL, "JoystickYButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_Z] = new FrontEndObject(NULL, "JoystickZButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_START] = NULL;//new FrontEndObject(NULL, "JoystickStartButtonHiLight", 1.0f, GlobalStick, true);
	ControllerHighModel[CCT_STICK][GS_DIRECTION] = NULL;//new FrontEndObject(NULL, "JoystickStickHiLight", 1.0f, GlobalStick, true);
	IncrementProgressBar();
	GlobalStick->Hide();

	arcPop();

	for (int i=0 ; i<9 ; i++) 
	{
		if (ControllerHighModel[CCT_DREAMPAD][i]) ControllerHighModel[CCT_DREAMPAD][i]->Hide();
	}
}

void Game::LoadSoundObjects()
{
	arcPush(DISKNAME"\\SOS1937FrontEnd\\SoundIcons.tc");
	SoundObjects[0][SO_MUSIC] = new FrontEndObject(NULL, "Note");
	SoundObjects[0][SO_ENGINE] = new FrontEndObject(NULL, "Car");
	SoundObjects[0][SO_EFFECTS] = new FrontEndObject(NULL, "SoundFX");
	SoundObjects[1][SO_MUSIC] = new FrontEndObject(NULL, "NoteHighlight");
	SoundObjects[1][SO_ENGINE] = new FrontEndObject(NULL, "CarHighlight");
	SoundObjects[1][SO_EFFECTS] = new FrontEndObject(NULL, "SoundFXHighlight");
	arcPop();

	arcPush(DISKNAME"\\SOS1937FrontEnd\\Sliders.tc");
	_SlideBar = new FrontEndObject(NULL, "SlideBar",1.0f,NULL, false);
	_SlideKnob = new FrontEndObject(NULL, "Knob",1.0f,_SlideBar, true);
	arcPop();
	IncrementProgressBar();

}

int Game::ControlPressed(int ControlPacket)
{
	return CurrentScreen->ControlPressed(ControlPacket);
}

void Game::Update()
{
	if (ShowingStart)
	{
		FlashCounter--;
		if (FlashCounter == 25)
			StartText->Hide();
		if (FlashCounter == 0)
		{
			StartText->Write();
			FlashCounter = 70;
		}
	}
	
	static struct Instruction ScreenCommand;

	// Default to no instruction.
	ScreenCommand.Command = 0;
	
	CurrentScreen->Update(&ScreenCommand);

	// Parse the command.
	switch (ScreenCommand.Command)
	{
	case 'S':
		delete CurrentScreen;
		CurrentScreen = (Screen *)ScreenCommand.Value;
		dprintf("screen changed to %lx",CurrentScreen);
		break;
	
	default:
		break;
	}
}

void Game::DrawBackground()
{
	if(!CurrentScreen)
		dprintf("no current screen");

	if (CurrentScreen->UseDefaultBackdrop)
	{
		CurrentScreen->DrawFullScreen2D((DWORD *)DefaultBackdrop);
	}
	else
	{
		dprintf("CurrentScreen %lx",CurrentScreen);
		CurrentScreen->DrawBackground();
	}
}

void Game::DestroyScreen()
{
	CurrentScreen->Destroy();
}

void Game::Destroy()
{
	delete CurrentScreen;

	int i;
	for (int j=0 ; j<3 ; j++)
		for (i=0 ; i<15 ; i++)
		{
			if (AllScenarios[j][i])
			{
				free(AllScenarios[j][i]->Name);
				free(AllScenarios[j][i]->Description);
				free(AllScenarios[j][i]->Track);
				free(AllScenarios[j][i]->LengthString);
				free(AllScenarios[j][i]->SuccessString);
				free(AllScenarios[j][i]->FailureString);
				delete AllScenarios[j][i];
			}
			if (ChampionshipScenarios[j][i])
			{
				free(ChampionshipScenarios[j][i]->Name);
				free(ChampionshipScenarios[j][i]->Description);
				free(ChampionshipScenarios[j][i]->LengthString);
				free(ChampionshipScenarios[j][i]->SuccessString);
				free(ChampionshipScenarios[j][i]->FailureString);
				delete ChampionshipScenarios[j][i];
			}
		}

	
	for (i=0 ; i<7 ; i++) delete RaceOptionsBank[i];
	for (i=0 ; i<7 ; i++) delete ChampionshipBank[i];
	for (i=0 ; i<3 ; i++) delete SingleRaceABank[i];
	for (i=0 ; i<2 ; i++) delete VMOptionsBank[i];
	for (i=0 ; i<4 ; i++) delete ScenarioBank[i];

	delete MainMenu;

	delete DefaultAlphabet;
	delete YellowAlphabet;

	for (i=0 ; i<15 ; i++) 
		delete AllCars[i];

	delete _SlideBar;
	delete _SlideKnob;

	for (i=0 ; i<15 ; i++)
	{
		delete CarGridModels[i];
		delete CarTableModels[i];
	}


	delete _MemoryCardOne;
	delete _MemoryCardTwo;
	delete _MemoryCardBlack;
	delete GlobalMemoryCard;

	for (i=0 ; i<9 ; i++)
	{
		delete TrackModels[i];
		delete TrackStatsModels[i];
		delete TrackNameModels[i];
	}

	delete GlobalTrack;

	delete LeftArrow;
	delete RightArrow;
	delete UpArrow;
	delete DownArrow;

	delete EndLetter;
	delete DelLetter;
	delete Cursor;
	delete Space;

	for (i=RT_CHAMPIONSHIP ; i<=RT_SINGLE ; i++)
		if (RaceTypeModels[i]) delete RaceTypeModels[i];

	for (i=0 ; i<2 ; i++)
		for (int j=0 ; j<3 ; j++)
			delete SoundObjects[i][j];

	for (i=HEADER_RACEOPTIONS ; i<=HEADER_SINGLERACE ; i++)
		delete ScreenHeaders[i];

	if (GlobalController)
	{
		for (i=0 ; i<3 ; i++)
			for (int j=0 ; j<9 ; j++)
			{
				if (ControllerLowModel[i][j])
				{
					delete ControllerLowModel[i][j];
					delete ControllerHighModel[i][j];
				}
			}


		delete GlobalController;
		if (GlobalWheel) delete GlobalWheel;
		if (GlobalStick) delete GlobalStick;
	}

	delete StartText;
	delete StartButton;
}

void Game::LoadAllBackdrops()
{
	arcPush(DISKNAME "\\SOS1937FrontEnd\\Backdrops.tc");
	switch (reallanguage)
	{
	case LG_AMERICAN:
	case LG_ENGLISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\EnglishBackdrops.tc");
		break;
	case LG_FRENCH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\FrenchBackdrops.tc");
		break;
	case LG_GERMAN:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\GermanBackdrops.tc");
		break;
	case LG_SPANISH:
		arcPush(DISKNAME"\\SOS1937FrontEnd\\SpanishBackdrops.tc");
		break;
	}
	ScreenHeaders[HEADER_RACEOPTIONS] = new FrontEndObject(NULL, "RaceOptionsTitle");
	ScreenHeaders[HEADER_CHAMPIONSHIP] = new FrontEndObject(NULL, "ChampionshipTitle");
	ScreenHeaders[HEADER_QUICKRACE] = new FrontEndObject(NULL, "QuickRaceTitle");
	ScreenHeaders[HEADER_SCENARIO] = new FrontEndObject(NULL, "ScenarioTitle");
	ScreenHeaders[HEADER_SINGLERACE] = new FrontEndObject(NULL, "SingleRaceTitle");
//	ScreenHeaders[HEADER_STARTRACE] = new FrontEndObject(NULL, "StartRaceTitle");
//	ScreenHeaders[HEADER_VMOPTIONS] = new FrontEndObject(NULL, "VMOptionsTitle");

	for (int i=HEADER_RACEOPTIONS ; i<= HEADER_SINGLERACE ; i++)
	{
		ScreenHeaders[i]->Justify(JUSTIFY_TOP);
		ScreenHeaders[i]->Draw(410,35,800.0f);
		ScreenHeaders[i]->Hide();
	}
	
	StartButton = new FrontEndObject(NULL, "StartButton");
	StartButton->Justify(JUSTIFY_RIGHT);
	StartButton->Draw(630,436,1600.0f);
	StartButton->Hide();
	StartText = new TextWord(TranslateLocale(80), DefaultAlphabet);
	StartText->Justify(JUSTIFY_RIGHT);
	StartText->Write(585,423,900.0f);
	StartText->Hide();
	arcPop();
	arcPop();
}

void FillInGameFields()
{
	strcpy(MainGameInfo.TrackName, TrackNumbers[MainGameInfo.Track]);

	for (int i=0 ; i<10 ; i++)
	{
		if (MainGameInfo.CarTypes[i])
			sprintf(MainGameInfo.CarName[i], "%s_%d", CarNumbers[MainGameInfo.CarTypes[i]-1], MainGameInfo.CarTypes[i]);
		else
			*MainGameInfo.CarName[i] = 0;
//		dprintf("Created car %d as a %s", i, MainGameInfo.CarName[i]);
	}

	strcpy(MainGameInfo.TrackName, TrackNumbers[MainGameInfo.Track]);
	dprintf("Setting track to %s", MainGameInfo.TrackName);
}