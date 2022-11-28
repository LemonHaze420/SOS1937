// $Header$

// $Log$
// Revision 1.28  2000-04-26 10:08:54+01  img
// <>
//
// Revision 1.27  2000-04-24 20:27:36+01  img
// Bug fixes
//
// Revision 1.26  2000-04-22 18:26:11+01  img
// Added C button to joystick support
//
// Revision 1.25  2000-04-21 19:43:47+01  img
// DestroyScreen() added
//
// Revision 1.24  2000-04-17 14:53:02+01  img
// Bug fixes
//
// Revision 1.23  2000-04-07 15:58:27+01  img
// Controllers loaded here now.  LoadDefaultSettings added.
//
// Revision 1.22  2000-04-07 15:29:13+01  img
// Better management of frontscreen's memory card
//
// Revision 1.21  2000-04-07 14:38:17+01  img
// Help support, generic background support
//
// Revision 1.20  2000-04-05 17:46:54+01  img
// More preloadings
//
// Revision 1.19  2000-04-03 14:54:06+01  img
// Preloadings, and no PlayerScreen
//
// Revision 1.18  2000-03-30 14:48:28+01  img
// More preloadings
//
// Revision 1.17  2000-03-28 17:57:02+01  img
// Preloading grid models and bug fixes.
//
// Revision 1.16  2000-03-22 09:29:09+00  img
// <>
//
// Revision 1.15  2000-03-21 17:28:30+00  img
// Pre-loading more now...
//
// Revision 1.14  2000-03-20 18:01:02+00  img
// Beta build
//
// Revision 1.13  2000-03-14 14:29:11+00  img
// Added header for ScenarioDetails
//
// Revision 1.12  2000-03-13 18:17:03+00  img
// Added headers.
//
// Revision 1.11  2000-03-08 10:32:18+00  img
// DefaultAlphabet uses Courier. Also uses 3df backdrops
//
// Revision 1.10  2000-03-07 16:02:55+00  img
// Use re-entry point
//
// Revision 1.9  2000-03-07 14:23:41+00  img
// ReEntryPoint now works
//
// Revision 1.8  2000-03-07 14:02:35+00  img
// Uses pvr files
//
// Revision 1.7  2000-03-02 09:49:58+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.6  2000-02-29 14:02:59+00  jjs
// Modified to work with Boris3.
//
// Revision 1.5  2000-02-29 11:28:30+00  jjs
// Added Header and Log lines.
//

/* First screen of the front end
	SOS_Index.h

	29/11/1999 */

#ifndef __SOS_INDEX_H__
#define __SOS_INDEX_H__

#include "FrontEndCar.h"
#include "FrontEndTrack.h"
#include "FrontScreen.h"
#include "SelectCarScreen.h"
#include "SelectTrackScreen.h"
#include "ControllerOptionsScreen.h"
#include "ChooseOptionsScreen.h"
#include "MemoryCardScreen.h"
#include "MemoryCardChosenScreen.h"
#include "OptionsScreen.h"
#include "SingleRace.h"
#include "RaceType.h"
#include "EnterPlayerName.h"
#include "SingleRaceOver.h"
#include "ChooseScenario.h"
#include "ScenarioDetails.h"
#include "Championship.h"
#include "ChampionshipOver.h"
#include "StartQuickRace.h"

extern FrontEndCar *GlobalCar, *AllCars[15];
extern FrontEndTrack *GlobalTrack;
extern FrontEndObject *GlobalController, *GlobalWheel, *GlobalStick;
extern FrontEndObject *ControllerLowModel[3][9], *ControllerHighModel[3][9];
extern FrontEndObject *RaceTypeModels[6];
extern FrontEndObject *TrackModels[9], *TrackStatsModels[9], *TrackNameModels[9];
extern char *CarNumbers[], *TrackNumbers[];
extern FrontEndObject *LeftArrow, *RightArrow, *UpArrow, *DownArrow;
extern FrontEndObject *CarTableModels[15];
extern FrontEndObject *CarGridModels[15];
extern FrontEndObject *EndLetter, *DelLetter, *Cursor, *Space;
extern FrontEndObject *SoundObjects[2][3];
extern FrontEndObject *ScreenHeaders[7];
extern FrontEndObject *GlobalMemoryCard;

extern Button *RaceOptionsBank[7], *ChampionshipBank[7], *SingleRaceABank[3], *VMOptionsBank[2], *ScenarioBank[4];
extern int ReEntryPoint;
extern struct GlobalStructure MainGameInfo;
extern BorisMaterial ChampionshipBackdrop[8], ScenarioBackdrop[8], SingleRaceBackdrop[8], VMOptionsBackdrop[8];

extern struct Scenario *AllScenarios[3][15], *ChampionshipScenarios[3][15];

extern void startMusic();
extern void stopMusic();

extern Alphabet *YellowAlphabet;

struct Instruction
{
	char Command;
	void *Value;
};

void FillInGameFields();

class Game
{
public:
	Game(int ReEntryPoint);

	// A control has been pressed!
	int ControlPressed(int ControlPacket);

	// Update the screen.
	void Update();

	// Draw the background
	void DrawBackground();

	// Destroy all trace of the front end.
	void Destroy();
	void DestroyScreen();

	Alphabet *DefaultAlphabet;
private:
	void LoadRaceTypeModels();
	void LoadCarGridModels();
	void LoadAllButtons();
	void LoadAllTracks();
	void LoadAllCars();
	void LoadAllBackdrops();
	void LoadMemoryCards();
	void LoadLetters();
	void LoadSoundObjects();
	void LoadController();

	void LoadGlobalCar();
	void LoadGlobalTrack();

	void LoadFileIcon();
	void LoadDefaultSettings();

	Screen *CurrentScreen;

	// if 0 < FlashCounter < 50 then show START
	// if 50 <= FlashCounter < 100 then hide START
	int FlashCounter;

	// The default backdrop.
	BorisMaterial DefaultBackdrop[8];
};

#endif