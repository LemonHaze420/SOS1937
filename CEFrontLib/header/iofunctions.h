// $Header$

// $Log$
// Revision 1.7  2000-07-03 15:11:24+01  img
// Added UNDER_CE
//
// Revision 1.6  2000-04-13 12:46:50+01  img
// Error checking
//
// Revision 1.5  2000-04-12 12:54:10+01  img
// Multiple file support
//
// Revision 1.4  2000-04-07 17:32:34+01  img
// Don`t automatically load defaults
//
// Revision 1.3  2000-03-20 18:00:45+00  img
// Beta build
//
// Revision 1.2  2000-02-29 08:44:44+00  jjs
// Added Header and Log lines
//

/* The control functions for the controller etc.

	Ian Gledhill 31/01/2000 
	Broadsword Interactive Ltd. */

#ifndef __IOFUNCTINOS_H__
#define __IOFUNCTIONS_H__

#if defined (UNDER_CE)
#  include <mapledev.h>
#  include <perstore.h>

// EnumProc for each file.
BOOL CALLBACK FlashDeviceEnumerationProc (IFlashDevice *pIFlashDevice, FSFILEID fsfileid, LPCFSFILEDESC lpcfsfiledesc, void *pvContext);

// EnumProc for each flash memory.
BOOL CALLBACK FlashEnumerationProc( LPCMAPLEDEVICEINSTANCE lpcmapledevinstance, LPVOID lpContext );

#endif

// Enumerate all flash cards attached.
void InitialiseMemories();
// Flush and close all open files.
void DeinitialiseMemories();

// Save/Load the default memory settings.
bool SaveMainGameInfo(struct GlobalStructure *OldStructure, int CardID, int SaveGameNumber);
bool LoadMainGameInfo(struct GlobalStructure *NewStructure, int CardID, int SaveGameNumber);

void GetDirectory(int CardID, char *PlayerNames[]);
int GetFreeSlots(int CardID);

void DrawVMIcon(int LcdID, char *IconData);

#endif