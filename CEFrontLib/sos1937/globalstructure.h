// $Header$

// $Log$
// Revision 1.23  2000-05-09 12:09:34+01  jcf
// Increased ButtonDefines array size to [2]
//
// Revision 1.22  2000-04-22 18:29:01+01  img
// <>
//
// Revision 1.21  2000-04-20 16:23:21+01  img
// Removed L_* #define conflict
//
// Revision 1.20  2000-04-20 12:56:47+01  img
// Memory leaks fixed
//
// Revision 1.19  2000-04-19 21:38:01+01  img
// Localisation.
//
// Revision 1.18  2000-04-17 14:53:33+01  img
// Bug fixes
//
// Revision 1.17  2000-04-11 16:14:18+01  img
// CON_* updated
//
// Revision 1.16  2000-04-07 14:37:32+01  img
// Changed CS_* constant
//
// Revision 1.15  2000-04-03 14:52:35+01  img
// More constants
//
// Revision 1.14  2000-03-28 17:54:57+01  img
// Removed replay consts
//
// Revision 1.13  2000-03-27 11:55:11+01  jjs
// Beta build.
//
// Revision 1.12  2000-03-20 18:00:55+00  img
// Beta build
//
// Revision 1.11  2000-03-14 14:26:52+00  img
// More constants
//
// Revision 1.10  2000-03-13 18:15:19+00  img
// New constants; ScenarioID added to structure.
//
// Revision 1.9  2000-03-07 14:23:55+00  img
// Added constants for re-entry points, REP_*
//
// Revision 1.8  2000-03-07 14:00:22+00  img
// Extra members to the structure
//
// Revision 1.7  2000-03-06 13:07:22+00  img
// SO* constants included
//
// Revision 1.6  2000-03-03 15:24:07+00  img
// Constants moved here
//
// Revision 1.5  2000-03-02 16:43:53+00  img
// Preliminary steering wheel support
//
// Revision 1.4  2000-03-02 14:26:38+00  img
// Controller screen now working
//
// Revision 1.3  2000-02-29 08:44:48+00  jjs
// Added Header and Log lines
//

/* Global structure for communication between the main game and
   the front end.

	31/01/2000
	
	Ian Gledhill for
	Broadsword Interactive Ltd.
*/

#ifndef __GLOBALSTRUCTURE_H__
#define __GLOBALSTRUCTURE_H__

// Define the magic number to be the most important day in Broadsword's history! ;-)
#define SOS_MAGICNUMBER 0x18011998

#define GS_ACCELERATE	0
#define GS_BRAKE		1
#define GS_GEARUP		2
#define GS_GEARDOWN		3
#define GS_REARVIEW		4
#define GS_VIEW			5
#define GS_STATUS		6
#define GS_OK			7
#define GS_RUMBLE		13

#define GS_ANALOGUE		0
#define GS_LTRIGGER		1
#define GS_RTRIGGER		2
#define GS_C			1
#define GS_Z			2
#define GS_DIRECTION	3
#define GS_A			4
#define GS_B			5
#define GS_X			6
#define GS_Y			7
#define GS_START		8

// The following are particular directions on the D pad.
#define GS_UP			9
#define GS_DOWN			10
#define GS_LEFT			11
#define GS_RIGHT		12

#define RT_CHAMPIONSHIP 0
//#define RT_HEADTOHEAD	1
#define RT_QUICK		1
#define RT_SCENARIO		2
#define RT_SINGLE		3
#define RT_NETWORK		4

#ifndef CON_THROTTLE
# define	CON_THROTTLE	0
# define	CON_BRAKE		1
# define	CON_LEFT		2
# define	CON_RIGHT		3
# define	CON_GEARUP		4
# define	CON_GEARDN		5
# define	CON_START		6
# define	CON_MORE		7
# define	CON_GLANCE		8
# define	CON_A			9
# define	CON_B			10
# define	CON_X			11
# define	CON_Y			12
# define	CON_DPAD_LEFT	13
# define	CON_DPAD_RIGHT	14
# define	CON_DPAD_UP		15
# define	CON_DPAD_DOWN	16
# define	CON_LTRIGGER	17
# define	CON_RTRIGGER	18
# define	CON_KEY			19 
# define	CON_C			20
# define	CON_Z			21
#endif

#define GEARBOX_AUTOMATIC	0
#define GEARBOX_MANUAL		1


#define PDS_DIFFICULTY		0
#define PDS_GEARBOX1		1
#define PDS_GEARBOX2		2
#define PDS_PLAYERNAME1		3
#define PDS_PLAYERNAME2		4
#define PDS_OK				5

#define PDS_EASY			0
#define PDS_MEDIUM			1
#define PDS_HARD			2

// The controller types.
#define CCT_DREAMPAD 0
#define CCT_WHEEL 1
#define CCT_STICK 2
#define CCT_KEYBOARD 3
#define CCT_FISHINGROD 4
#define CCT_OTHER	10

#define SO_MUSIC	0
#define SO_ENGINE	1
#define SO_EFFECTS	2

//#define VMO_SAVEREPLAY	1
//#define VMO_SHOWREPLAY	2
#define VMO_SAVEGAME	1
#define VMO_LOADGAME	2

#define REP_START					0
#define REP_SINGLERACEOVER			1
#define REP_CHAMPIONSHIPRACEOVER	2
#define REP_QUICKRACEOVER			3
#define REP_SCENARIOOVER			4
#define REP_HEADTOHEADOVER			5
#define REP_NODEFAULTS				6

#define JUSTIFY_CENTRE 0
#define JUSTIFY_LEFT 1
#define JUSTIFY_RIGHT 2
#define JUSTIFY_TOP 3
#define JUSTIFY_BOTTOM 4

#define CS_STARTGRID	1
#define CS_TRACKINFO	2
#define CS_CARINFO		3
#define CS_STANDINGS	4
#define CS_PRACTICE		5
#define CS_QUALIFY		6
#define CS_MEMORY		7

#define SD_OBJECTIVE	1
#define SD_STARTGRID	2
#define SD_TRACKINFO	3
#define SD_CARINFO		4

#define RL_SHORT		0
#define RL_MEDIUM		1
#define RL_LONG			2

#define T_AVUS			0
#define T_BROOKLANDS	1
#define T_DONINGTON		2
#define T_MONTANA		3
#define T_MONTLHERY		4
#define T_MONZA			5
#define T_PAU			6
#define T_ROOSEVELT		7
#define T_TRIPOLI		8

#define RM_RACE			0
#define RM_PRACTICE		1
#define RM_QUALIFY		2

#define HEADER_RACEOPTIONS	0
#define HEADER_CHAMPIONSHIP	1
#define HEADER_QUICKRACE	2
#define HEADER_SCENARIO		3
#define HEADER_SINGLERACE	4
#define HEADER_STARTRACE	5
#define HEADER_VMOPTIONS	0

#define LG_ENGLISH		0
#define LG_AMERICAN		1
#define LG_GERMAN		2
#define LG_FRENCH		3
#define LG_SPANISH		4

#define RUMBLE_BIT	2

struct GlobalStructure
{
	int MagicNumber;
	char PlayerName[2][32];
	char CarName[10][32];
	char TrackName[15];

	// Lap time measured in 100ths of a second.
	int LapTime[10];
	int RaceTime[10];

	// Each car has an ID; we have 10 cars available.
	int CarTypes[10];

	// The place on the grid for each car.
	int QualifiedPosition[10];
	
	// The positions of each car after the race.
	int Positions[10];

	int ChampionshipPoints[10];
	int ChampionshipPosition[10];

	int Laps;

	unsigned short MusicVolume;
	unsigned short EngineVolume;
	unsigned short EffectsVolume;

	// Three definitions - one per controller.
	// And two players.
	unsigned short ButtonDefines[2][CCT_STICK+1][7];
	unsigned short RaceType;
	unsigned short Difficulty;
	unsigned short GearBox[2];
	unsigned short ScenarioID;
	unsigned short Track;

	// Set to RM_* for practice/qualify/race.
	unsigned short RaceMode;

	// The current controller.
	unsigned short ControllerID;
};

#endif