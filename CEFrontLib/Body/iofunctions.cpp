// $Header$

// $Log$
// Revision 1.26  2000-07-03 15:16:30+01  jjs
// Added Japanese support.
//
// Revision 1.25  2000-05-31 11:45:43+01  img
// Final build!! (please!!)
//
// Revision 1.24  2000-05-06 13:48:31+01  img
// Sean build
//
// Revision 1.23  2000-05-04 17:39:22+01  img
// Build 18
//
// Revision 1.22  2000-04-24 20:27:34+01  img
// Bug fixes
//
// Revision 1.21  2000-04-23 16:08:06+01  img
// Redone error message
//
// Revision 1.20  2000-04-14 09:45:22+01  img
// Bugfix
//
// Revision 1.19  2000-04-13 12:46:49+01  img
// Error checking
//
// Revision 1.18  2000-04-12 18:02:41+01  img
// bugfixes
//
// Revision 1.17  2000-04-12 13:59:53+01  img
// Multiple file names
//
// Revision 1.16  2000-04-12 12:54:09+01  img
// Multiple file support
//
// Revision 1.15  2000-04-07 17:32:34+01  img
// Don`t automatically load defaults
//
// Revision 1.14  2000-04-04 18:38:52+01  img
// Sanity check on the memorycard variable
//
// Revision 1.13  2000-04-03 14:50:35+01  img
// New defaults.
//
// Revision 1.12  2000-03-22 12:47:10+00  jjs
// <>
//
// Revision 1.11  2000-03-20 18:00:41+00  img
// Beta build
//
// Revision 1.10  2000-03-13 18:13:14+00  img
// New Defaults
//
// Revision 1.9  2000-03-09 14:48:26+00  jjs
// Changed Motlhery to Montana for Quick Race.
//
// Revision 1.8  2000-03-03 15:16:46+00  img
// New defaults
//
// Revision 1.7  2000-03-02 16:43:52+00  img
// Preliminary steering wheel support
//
// Revision 1.6  2000-03-01 11:31:26+00  img
// Properly working memory card screen again.
//
// Revision 1.5  2000-02-29 08:43:35+00  jjs
// Added Header & Log lines.
//

/* The control functions for the controller etc.

	Ian Gledhill 31/01/2000 
	Broadsword Interactive Ltd. */

#if !defined(UNDER_CE)
#  include "IOFunctions_PC.cpp"
#else

// Now back to where we were.
#include <windows.h>

#include <mapledev.h>
#include <perstore.h>
#include <lcd.h>
#include <ceddcdrm.h>
#include <segagdrm.h>

#include <errorlib.h>
#include "../ConsoleFrontLib.h"

#include "../SOS1937/cross.h"
#include "../SOS1937/exclamation.h"
#include "../SOS1937/tick.h"
#include "../SOS1937/SOSLogo.h"

// Globally store the flash device so we only select it once.
IFlashDevice *ActiveMemoryCard = NULL;
IFlashFile *SettingsFile = NULL;
ILcd *ActiveLCD;
ILcd *LCDScreen[8];
extern IFlashDevice *MemoryCardSlot[8];
extern FSFILEICON SOSFileIcon;
#if defined(JAPAN)
const char IDString[]={0x1a,0x35,0x4b,0x26,0x29,0x0a,0x37,0x1a,0x35,0x04,0x2a,0x55,0x5d,0x57,0x5b,0x00};
#endif

BOOL CALLBACK FlashDeviceEnumerationProc(IFlashDevice *pIFlashDevice, FSFILEID fsfileid, LPCFSFILEDESC lpcfsfiledesc, void *pvContext)
{
	dprintf("Found file name %s for game %s", lpcfsfiledesc->szFileName, lpcfsfiledesc->szGameName);

	return true;
}

// Find the particular settings file.
BOOL CALLBACK FindSettingsFileEnumerationProc(IFlashDevice *pIFlashDevice, FSFILEID fsfileid, LPCFSFILEDESC lpcfsfiledesc, void *pvContext)
{
	HRESULT hr;

#if defined(JAPAN)
	if (!memcmp(lpcfsfiledesc->szGameName, IDString, 15))
#else
	if (!strncmp(lpcfsfiledesc->szGameName, "SOS1937", 7))
#endif
	{
		dprintf("Found a Spirit of Speed file!");
		if (!strncmp(lpcfsfiledesc->szFileName, "SOSSetng.sav", 12))
		{
			dprintf("It's a default settings file!");
			hr = ActiveMemoryCard->OpenFlashFile(&SettingsFile, fsfileid);
			if (hr) NonFatalError("Error opening file...");
		}
		else
			dprintf("It's a save game file....");
	}

	return true;
}

BOOL CALLBACK FlashEnumerationProc( LPCMAPLEDEVICEINSTANCE lpcmapledevinstance, LPVOID lpContext )
{
	HRESULT hr;
	
	if (lpcmapledevinstance->devType == MDT_STORAGE)
	{
		dprintf("Found memory at port %d device %d", lpcmapledevinstance->dwPort, lpcmapledevinstance->dwDevNum);

		hr = MapleCreateDevice( &lpcmapledevinstance->guidDevice, (struct IUnknown **) &ActiveMemoryCard);

		hr = ActiveMemoryCard->CheckFormat();

		if (!hr) dprintf("Card is formatted...");
		else NonFatalError("Card is not formatted!!!");

		FSDEVICEDESC CardDescription;
		CardDescription.dwSize = sizeof(FSDEVICEDESC);
		CardDescription.dwFlags = FSDD_TOTAL_BLOCKS | FSDD_FREE_BLOCKS | FSDD_BLOCK_SIZE | FSDD_COLOR_VMS | FSDD_VOLUME_ICON | FSDD_DATE_TIME_CREATED;
		hr= ActiveMemoryCard->GetDeviceDesc( &CardDescription);
		if (hr) NonFatalError("Error reading card properties port %d device %d", lpcmapledevinstance->dwPort, lpcmapledevinstance->dwDevNum);
	
		FSDATETIME CardDate = CardDescription.fsdatetimeCreated;
		dprintf("Memory card holds %d total blocks, %d free blocks, with a block size of %d bytes. Created on %d/%d/%2d%02d at %d:%d", CardDescription.dwTotalBlocks,
				CardDescription.dwFreeBlocks, CardDescription.dwBlockSize, CardDate.bDay, CardDate.bMonth, CardDate.bYearTop, CardDate.bYearBottom, 
				CardDate.bHour, CardDate.bMinute);

	}

	if (lpcmapledevinstance->devType == MDT_LCD)
	{
		dprintf("Found LCD display at port %d device %d", lpcmapledevinstance->dwPort, lpcmapledevinstance->dwDevNum);

		ILcd *TempLCD;

		hr = MapleCreateDevice( &lpcmapledevinstance->guidDevice, (struct IUnknown **) &TempLCD);
		if (hr) NonFatalError("Error creating LCD device");

		// Dunno why we need to do this but the help file says we should... weird....
		hr = TempLCD->QueryInterface(IID_ILcd, (void **) &ActiveLCD);
		if (hr) NonFatalError("Error querying ILcd interface");

		if (ActiveLCD->IsStandardLcd()) dprintf("LCD is standard SEGA spec");
		else dprintf("LCD is non-standard spec");

		LCDScreen[lpcmapledevinstance->dwPort*2 + lpcmapledevinstance->dwDevNum-1] = ActiveLCD;

		LCD_INFO LCDInfo;

		// Get the info of the first LCD.
		ActiveLCD->GetLcdInfo(0, &LCDInfo);	
		
		dprintf("Number of LCD devices on this controller: %d", LCDInfo.PT);
	
		DWORD LCDBufferID=0;
		BYTE *LCDBuffer;
			
		hr = ActiveLCD->GetLcdBuffer((BYTE **)&LCDBuffer, &LCDBufferID, LCDInfo.actualBlockSize);
		if (hr) NonFatalError("Error loading the LCD Buffer");

		for (int i=0 ; i<LCDInfo.actualBlockSize ; i++)
		{
			LCDBuffer[i] = 0x00;
		}
//		LCDBuffer[0] = 0x01; LCDBuffer[1] = 0xC0; LCDBuffer[2] = 0x02;

		hr = ActiveLCD->SendLcdBuffer(LCDBufferID, 0, 0, 0, NULL);
		if (hr) NonFatalError("Error sending the LCD buffer");

		hr = ActiveLCD->FreeLcdBuffer(LCDBufferID);
	}

	return true;
}

void InitialiseMemories()
{
	for (int i=0 ; i<8 ; i++)
		LCDScreen[i] = NULL;
	MapleEnumerateDevices(MDT_LCD, FlashEnumerationProc, NULL, 0); 
	DrawVMIcon(-1, exclamation);
}

void DrawVMIcon(int LcdID, char *IconData)
{
	DWORD LCDBufferID=0;
	BYTE *LCDBuffer;
	HRESULT hr;
	LCD_INFO LCDInfo;
	
	for (int n=(LcdID==-1 ? 0 : LcdID) ; n<=(LcdID==-1 ? 7 : LcdID) ; n++)
	{	
		if (LCDScreen[n])
		{
			LCDScreen[n]->GetLcdInfo(0, &LCDInfo);	
			
			hr = LCDScreen[n]->GetLcdBuffer((BYTE **)&LCDBuffer, &LCDBufferID, LCDInfo.actualBlockSize);
			if (hr) NonFatalError("Error loading the LCD Buffer");
			
			for (int i=0 ; i<LCDInfo.actualBlockSize ; i++)
			{
				LCDBuffer[i] = IconData[i];
			}
			
			hr = LCDScreen[n]->SendLcdBuffer(LCDBufferID, 0, 0, 0, NULL);
			if (hr) NonFatalError("Error sending the LCD buffer");
			
			hr = LCDScreen[n]->FreeLcdBuffer(LCDBufferID);
		}
	}
}

int FindChecksum(struct GlobalStructure *Source)
{
	int Sum=0;
	for (int i=(int)Source+4 ; i<(int)(((int)Source) + sizeof(struct GlobalStructure)) ; i++)
	{
		Sum += *((BYTE *)i);
	}
	dprintf("Found checksum of %d", Sum);
	return Sum;
}

bool SaveMainGameInfo(struct GlobalStructure *OldStructure, int CardID, int SaveGameNumber)
{
	bool Error = true;

	// Create the icon.
	HRESULT hr;
	DWORD dwReturned;

	if (SettingsFile)
	{
		hr = SettingsFile->Flush();
		if (hr) SettingsFile = NULL;
	}
	DrawVMIcon(-1, exclamation);

	HANDLE    g_hGDROM = NULL;                          // Handle of the GDROM device
	SEGACD_DOOR_BEHAVIOR doorbehavior;

	g_hGDROM = CreateFile(TEXT("\\Device\\CDROM0"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if (g_hGDROM == INVALID_HANDLE_VALUE)
    {
        NonFatalError("Error opening GD-ROM");
    }
	else
	{
		doorbehavior.dwBehavior = SEGACD_DOOR_NOTIFY_APP;
		if(!DeviceIoControl(g_hGDROM, IOCTL_SEGACD_SET_DOOR_BEHAVIOR, &doorbehavior, sizeof(doorbehavior), NULL, 0, &dwReturned, NULL))
		{
			NonFatalError("Error setting GD-ROM to not re-boot");
		}
	}
	
	hr = 0;
	
	ActiveMemoryCard = MemoryCardSlot[CardID];

	// Check if we have a memory card installed still.
	if (ActiveMemoryCard)
	{
		// We have a card.
		hr = ActiveMemoryCard->Flush();
	}

	hr = ActiveMemoryCard->EnumFlashFiles( FindSettingsFileEnumerationProc, 0);

	if (SettingsFile)
	{
		hr = SettingsFile->Flush();
		SettingsFile->Release();
		SettingsFile = NULL;
	}

	if (!SettingsFile)
	{
		// Time to create a new file.
		FSFILEDESC FileDescription;

		FileDescription.dwSize = sizeof(FSFILEDESC);
		FileDescription.dwFlags = FSFD_CREATE_FILE;
		FileDescription.dwBytesRequired = sizeof(struct GlobalStructure);
		sprintf(FileDescription.szFileName, "SOS_1937_00%c", SaveGameNumber+'1');
		strcpy(FileDescription.szVMSComment, "SOS Settings");
		strcpy(FileDescription.szBootROMComment, "Spirit of Speed 1937 Settings");
#if defined(JAPAN)
		strcpy(FileDescription.szGameName, IDString);
#else
		strcpy(FileDescription.szGameName, "SOS1937");
#endif
		FileDescription.bStatus = FS_STATUS_DATA_FILE;
		FileDescription.bCopy = 0;
		FileDescription.fsfileicon = SOSFileIcon;

		if (ActiveMemoryCard)
		{
			if (!ActiveMemoryCard->OpenFlashFileByName(&SettingsFile, FileDescription.szFileName))
			{
				hr = SettingsFile->Delete();
				NonFatalError("Error deleting file");
				SettingsFile->Release();
				SettingsFile = NULL;
			}
			hr = ActiveMemoryCard->CreateFlashFile(&SettingsFile, &FileDescription);
			if (hr) 
			{
				NonFatalError("Error creating new file.");
				DrawVMIcon(-1, cross);
				Sleep(1000);
			}
		}

		if (SettingsFile)
		{
			hr = SettingsFile->Flush();
			if (hr) 
			{
				NonFatalError("Error flushing buffer");
				DrawVMIcon(-1, cross);
				Sleep(1000);
				SettingsFile = NULL;
			}
		}
		else
			NonFatalError("Error creating file - disk full?");
	}

	if (SettingsFile)
	{
		OldStructure->MagicNumber = FindChecksum(OldStructure);
		hr = SettingsFile->Write(0,sizeof(struct GlobalStructure), (unsigned char *)OldStructure);
		if (hr) NonFatalError("Error saving game info.");
		else dprintf("File saved OK.");
		DrawVMIcon(-1, tick);
		Sleep(1000);
		SettingsFile->Flush();
		Sleep(300);
		SettingsFile->Release();
		Error = false;
	}

	SettingsFile = NULL;

	DrawVMIcon(-1, SOSLogo);

	g_hGDROM = CreateFile(TEXT("\\Device\\CDROM0"), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
    if (g_hGDROM == INVALID_HANDLE_VALUE)
    {
        NonFatalError("Error opening GD-ROM");
    }
	else
	{
		doorbehavior.dwBehavior = SEGACD_DOOR_REBOOT;
		if(!DeviceIoControl(g_hGDROM, IOCTL_SEGACD_SET_DOOR_BEHAVIOR, &doorbehavior, sizeof(doorbehavior), NULL, 0, &dwReturned, NULL))
		{
			NonFatalError("Error setting GD-ROM to re-boot");
		}
	}
	return !Error;
}

bool LoadMainGameInfo(struct GlobalStructure *NewStructure, int CardID, int SaveGameNumber)
{
	bool Error=true;
	SettingsFile = NULL;

	InitialiseMemories();

	DrawVMIcon(-1, exclamation);

	HRESULT hr;

	char Filename[13];

//	hr = MemoryCardSlot[CardID]->EnumFlashFiles( FindSettingsFileEnumerationProc, 0);

	sprintf(Filename, "SOS_1937_00%c", SaveGameNumber+'1');
	hr = MemoryCardSlot[CardID]->OpenFlashFileByName(&SettingsFile, Filename);

	if (SettingsFile)
	{
		struct GlobalStructure TempStructure;
		
		hr = SettingsFile->Read(0,sizeof(struct GlobalStructure), (unsigned char *)&TempStructure);

		int Checksum = FindChecksum(&TempStructure);
		int FoundChecksum = *((int *) &TempStructure);
		
		if (hr || Checksum != FoundChecksum || Checksum == 0) 
		{ 
			NonFatalError("Error loading game info.");	
			DrawVMIcon(-1, cross);
			Sleep(1000);
		}
		else 
		{
			dprintf("File loaded OK.");
			DrawVMIcon(-1, tick);
			Sleep(1000);
			Error = false;
			*NewStructure = TempStructure;
		}
		SettingsFile->Flush();
	}
	else
	{
		DrawVMIcon(-1, cross);
		Sleep(1000);
	}
	
	if (SettingsFile)
	{
		SettingsFile->Release();
		SettingsFile = NULL;
	}
	DrawVMIcon(-1, SOSLogo);
//MainGameInfo.ChampionshipPoints[0] = 80;
	return !Error;
}

void GetDirectory(int CardID, char *PlayerNames[])
{
	DrawVMIcon(CardID, exclamation);
	struct GlobalStructure LoadedStructure;
	IFlashFile *ThisFile;
	char Filename[13];
	HRESULT hr;

	for (int i=0 ; i<4 ; i++)
	{
		sprintf(Filename, "SOS_1937_00%c", i+'1');

		hr = MemoryCardSlot[CardID]->OpenFlashFileByName(&ThisFile, Filename);

		if (hr)
			PlayerNames[i] = NULL;
		else
		{
			hr = ThisFile->Read(0,sizeof(struct GlobalStructure), (unsigned char *)(&LoadedStructure));
			PlayerNames[i] = strdup(LoadedStructure.PlayerName[0]);
			if (!hr) ThisFile->Release();
		}
	}

	DrawVMIcon(CardID, SOSLogo);
}

int GetFreeSlots(int CardID)
{
	FSDEVICEDESC DeviceDescription;
	DeviceDescription.dwSize = sizeof(FSDEVICEDESC);
	DeviceDescription.dwFlags = FSDD_TOTAL_BLOCKS | FSDD_FREE_BLOCKS | FSDD_BLOCK_SIZE | FSDD_COLOR_VMS | FSDD_VOLUME_ICON | FSDD_DATE_TIME_CREATED;

	MemoryCardSlot[CardID]->GetDeviceDesc(&DeviceDescription);
	return DeviceDescription.dwFreeBlocks;
}

void DeinitialiseMemories()
{
	if (SettingsFile)
	{
		SettingsFile->Flush();
		SettingsFile->Release();
	}

	for (int i=0 ; i<8 ; i++)
		if (LCDScreen[i])
			LCDScreen[i]->Release();
}

#endif (UNDER_CE)