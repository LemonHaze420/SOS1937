// $Header$

// $Log$
// Revision 1.52  2000-07-04 17:08:35+01  jjs
// Added Japanese support.
//
// Revision 1.51  2000-07-04 09:49:11+01  img
// changes for proper abstraction from SOS
//
// Revision 1.50  2000-06-07 13:49:46+01  jjs
// Really put the correct prototype in.
//
// Revision 1.49  2000-06-07 09:58:21+01  jjs
// Changed prototype for GetAControl.
//
// Revision 1.48  2000-05-17 10:25:31+01  img
// ReEnumCards set to false
//
// Revision 1.47  2000-05-16 17:05:09+01  img
// Insert Valid Controller message put in
//
// Revision 1.46  2000-05-15 12:03:35+01  img
// Fixed soft reset
//
// Revision 1.45  2000-05-15 09:42:44+01  img
// Pop lid when saving then reset.
//
// Revision 1.44  2000-05-11 12:13:39+01  img
// Memory leak fixes
//
// Revision 1.43  2000-05-11 10:43:10+01  img
// Static message when last controller removed
//
// Revision 1.42  2000-05-10 14:32:59+01  img
// RC4
//
// Revision 1.41  2000-05-09 14:07:56+01  img
// Release Candidate 2
//
// Revision 1.40  2000-05-08 19:53:12+01  img
// Final? build
//
// Revision 1.39  2000-05-06 13:16:51+01  img
// rationalised controller enum'ing code
//
// Revision 1.38  2000-05-04 17:39:22+01  img
// Build 18
//
// Revision 1.37  2000-05-02 17:03:37+01  img
// Build 17
//
// Revision 1.36  2000-04-23 20:48:38+01  img
// Bug fixes
//
// Revision 1.35  2000-04-23 12:50:24+01  img
// key ignored when blanker cancelled
//
// Revision 1.34  2000-04-22 21:32:20+01  img
// VM texts
//
// Revision 1.33  2000-04-22 18:29:01+01  img
// <>
//
// Revision 1.32  2000-04-22 13:26:27+01  jcf
// <>
//
// Revision 1.31  2000-04-21 19:47:10+01  jcf
// <>
//
// Revision 1.30  2000-04-20 16:59:55+01  img
// commented StartTextureStore()
//
// Revision 1.29  2000-04-20 16:23:18+01  img
// Removed L_* #define conflict
//
// Revision 1.28  2000-04-20 12:56:45+01  img
// Memory leaks fixed
//
// Revision 1.27  2000-04-19 21:37:56+01  img
// Localisation.
//
// Revision 1.26  2000-04-13 12:47:01+01  img
// Initial LCD checking
//
// Revision 1.25  2000-04-12 18:02:36+01  img
// screensaver
//
// Revision 1.24  2000-04-11 16:14:38+01  img
// CON_* now noticed
//
// Revision 1.23  2000-04-07 17:32:42+01  img
// Removed initial loading
//
// Revision 1.22  2000-04-04 18:41:21+01  img
// Better cleaning up
//
// Revision 1.21  2000-04-03 14:50:43+01  img
// Keyboard support.
//
// Revision 1.20  2000-03-30 11:50:51+01  img
// Added startMusic()
//
// Revision 1.19  2000-03-28 17:53:17+01  img
// Disabled analogue pad
//
// Revision 1.18  2000-03-27 10:26:27+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.17  2000-03-20 18:00:42+00  img
// Beta build
//
// Revision 1.16  2000-03-09 11:40:54+00  img
// RemoveTextureStore now works.
//
// Revision 1.15  2000-03-09 10:34:59+00  img
// Doesn`t use RemoveTextureStore()
//
// Revision 1.14  2000-03-07 14:24:05+00  img
// ReEntryPoint implemented.
//
// Revision 1.13  2000-03-07 13:59:36+00  img
// Works with emulation again
//
// Revision 1.12  2000-03-06 13:03:11+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.11  2000-03-03 14:25:07+00  jjs
// Fixed for Boris3.
//
// Revision 1.10  2000-03-02 16:43:52+00  img
// Preliminary steering wheel support
//
// Revision 1.9  2000-03-02 14:26:36+00  img
// Controller screen now working
//
// Revision 1.8  2000-02-29 14:03:01+00  jjs
// Modified to work with Boris3.
//

/* Main body file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		09/11/1999
	Rev:		0.01 */

#define D3D_OVERLOADS
#include <windows.h>

#include <BS2All.h>

#if !defined(PROPER_FRONTEND)
#include <AllCAR.h>
#endif

#include <dinput.h>

#if defined(UNDER_CE)
#  include <platutil.h>
#endif

# include <errorlib.h>

#include "ConsoleFrontLib.h"

#if !defined(PROPER_FRONTEND)
#  include "../SOS1937/SOSLogo.h"
#endif

int GetAControl(char *name, int *Direction=NULL, bool doEnum = true);

#define BLANKER_TIMER 5*60*60

int CurrentCar;
float RotY, RotX, PosX, PosY, PosZ;
float ConstantRotY;
vector3 CameraPoint;
bool PreventLeftRightRepetition = true;
struct GlobalStructure MainGameInfo;
extern int ReEntryPoint;
extern bool reset_when_ready;
extern int AllControllerTypes[];
extern LPDIRECTINPUTDEVICE2 Controller[];
#if defined(JAPAN)
// Should really put these in the classes but I don't have time.
BorisMaterial japanMat;
textureData* japanTex;
int JapanHelp=-1;
#endif

#if defined(JAPAN)
void RenderJapanText(void)
{
	if(JapanHelp>=0)
	{
		D3DTLVERTEX jappoly[4]={
			_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),1.0f/0.00002f,0xffffffff,0xffffffff,0.0f,0.0f),
				_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),1.0f/0.00002f,0xffffffff,0xffffffff,1.0f,0.0f),
				_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),1.0f/0.00002f,0xffffffff,0xffffffff,1.0f,1.0f),
				_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),1.0f/0.00002f,0xffffffff,0xffffffff,0.0f,1.0f)
		};
		struct sxy {
			short x,y,w,h;
			float u1,v1,u2,v2;
		};
		static sxy japxy[]={
			{280,100,351,81,0.0f,0.0f,341.0f/512.0f,73.0f/512.0f},
			{320,100,298,54,0.0f,79.0f/512.0f,288.0f/512.0f,125.0f/512.0f},
			{320,100,304,57,0.0f,128.0f/512.0f,294.0f/512.0f,177.0f/512.0f},
			{300,100,319,56,0.0f,181.0f/512.0f,311.0f/512.0f,229.0f/512.0f},
			{190,100,265,56,0.0f,235.0f/512.0f,255.0f/512.0f,283.0f/512.0f},
			{320,100,293,54,0.0f,288.0f/512.0f,283.0f/512.0f,334.0f/512.0f},
			{290,100,346,58,0.0f,338.0f/512.0f,336.0f/512.0f,388.0f/512.0f},
			{147,100,346,31,0.0f,392.0f/512.0f,336.0f/512.0f,415.0f/512.0f}
		};
		DarkenScreen(japxy[JapanHelp].x,japxy[JapanHelp].y,japxy[JapanHelp].w,japxy[JapanHelp].h,D3DRGBA(0.0f,0.0f,0.0f,1.0f),.00003f);	
		jappoly[0].sx = jappoly[3].sx = japxy[JapanHelp].x + 5;
		jappoly[0].sy = jappoly[1].sy = japxy[JapanHelp].y + 4;
		jappoly[1].sx = jappoly[2].sx = japxy[JapanHelp].x + japxy[JapanHelp].w - 5;
		jappoly[2].sy = jappoly[3].sy = japxy[JapanHelp].y + japxy[JapanHelp].h - 4;
		jappoly[0].tu = jappoly[3].tu = japxy[JapanHelp].u1;
		jappoly[0].tv = jappoly[1].tv = japxy[JapanHelp].v1;
		jappoly[1].tu = jappoly[2].tu = japxy[JapanHelp].u2;
		jappoly[2].tv = jappoly[3].tv = japxy[JapanHelp].v2;
		renderSetCurrentMaterial(&japanMat);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, jappoly,4,D3DDP_DONOTCLIP );
	}
}
#endif

ConsoleFrontLib::ConsoleFrontLib()
{
//	SetDebugMemFlags(true,true,true,true);
}

extern int reallanguage;
bool UseKeys=false;

#if defined(UNDER_CE)
extern bool ReEnumCards;
extern bool SoftResetTriggered;
#endif
bool ReallyReset;

bool ConsoleFrontLib::Start()
{
#if defined(BORISEMULATIONHACK)
	EmulationRemoveAllObjects();
#else
	boris2->removeAllObjects();
#endif
	startModelStore();
	//	removeTextureStore();
	//	startTextureStore();
	
#if defined(JAPAN)
	japanTex= getTexturePtr("japan.3df",false,false);
	japanMat.textureHandle = japanTex->textureHandle;
	japanMat.flags = 0;
	JapanHelp = -1;
#endif	
	ReallyReset = false;
	
	CurrentCar = 0; RotX = 0.0f; RotY = 0.0f;
	PosX = -1.1f; PosY = -0.5f; PosZ = 12.0f;
	CameraPoint = vector3(0.0f,0.0f,100.0f);
	
	ConstantRotY = 0.0f;

	camera *boris2camera  = new camera( &vector3(0,1000.0f,0),
								600.0f, 3200 ); // zeye 0 = 360 900 = 90 e.t.c.

#ifndef BORISEMULATIONHACK
	display* boris2display  = new display(640, 480, 0,0);	
#endif
	MainCamera = boris2camera;

	boris2camera->faceTowardPoint(&vector3(0.0f,1000.0f,10.0f));
	boris2->setCurrentCamera(boris2camera);
#ifndef BORISEMULATIONHACK
	boris2->setCurrentDisplay(boris2display);							// associate display to engine
#endif
	int ControlPressed, Direction, ControlPacket;
	char ControlName[256];
	bool LeaveLoop = false;
#if defined (UNDER_CE)
	ReEnumCards = false;
#endif
	InitialiseModels();
	InitialiseMemories();

	MainGame = new Game(ReEntryPoint);
	
	startMusic();
	
	int Result=0, BlankerCount=0;
	
	DWORD PreviousFrame=0, ThisTime;
	MSG msg;

	static HANDLE hNewDeviceEvent=NULL;
	if (!hNewDeviceEvent)
		hNewDeviceEvent = CreateEvent(NULL,false, false, TEXT("MAPLE_NEW_DEVICE"));
	static HANDLE hDeviceGoneEvent=NULL;
	if (!hDeviceGoneEvent)
		hDeviceGoneEvent = CreateEvent(NULL, false, false, TEXT("MAPLE_DEVICE_REMOVED"));

	while (!Result && !LeaveLoop && !ReallyReset)
	{
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( GetMessage( &msg, NULL, 0, 0 ) )
			{
#if 0//!defined(UNDER_CE)
				if(msg.message != WM_ACTIVATEAPP)
#endif
				{
					TranslateMessage(&msg); 
					DispatchMessage(&msg);
				}
			}
			else
			{
				return (msg.wParam ? true : false);
			}
		}
#if defined(UNDER_CE)
		if (reset_when_ready)
			ResetToFirmware();
#endif

		if (BlankerCount < BLANKER_TIMER+1*60)
			BlankerCount++;
		PreviousFrame = ThisTime;
		
		static TextWord *DeviceRemovedText = NULL;
		static bool IgnoreControl = false;

#if defined (UNDER_CE)		
		if (ReEnumCards)
		{
			InitialiseMemories();
			DrawVMIcon(-1, SOSLogo);
		}
		ReEnumCards = false;

		if (Controller[0] && AllControllerTypes[0]!=CCT_OTHER || Controller[1] && AllControllerTypes[1]!=CCT_OTHER
			|| Controller[2] && AllControllerTypes[2]!=CCT_OTHER || Controller[3] && AllControllerTypes[3]!=CCT_OTHER)
		{
			if (DeviceRemovedText)
			{
				delete DeviceRemovedText;
				DeviceRemovedText = NULL;
#if defined(JAPAN)
				SetJapanText(-1);
#endif
			}
		}
		else
		{
			if (DeviceRemovedText)
			{
				delete DeviceRemovedText;
				DeviceRemovedText = new TextWord(TranslateLocale(93), MainGame->DefaultAlphabet);
				DeviceRemovedText->Write(340,402,1300.0f);
#if defined(JAPAN)
				SetJapanText(7);
#endif
			}
		}

		if (WaitForSingleObject(hNewDeviceEvent, 0) != WAIT_TIMEOUT || 
			WaitForSingleObject(hDeviceGoneEvent, 0) != WAIT_TIMEOUT)
		{
			ReEnumCards = true;
			if (DeviceRemovedText)
				delete DeviceRemovedText;
			DeviceRemovedText = NULL;
			DeviceRemovedText = new TextWord(TranslateLocale(85), MainGame->DefaultAlphabet);
#if defined(JAPAN)
			SetJapanText(4);
#endif
			DeviceRemovedText->Write(340,402,1300.0f);
			boris2->beginFrame(false,0);
//			Result = MainGame->ControlPressed(0);
//			MainGame->Update();
			
			MainGame->DrawBackground();
#if defined(JAPAN)
			RenderJapanText();
#endif			
			boris2->render();
			renderEndScene();
			renderFlipSurface();
#if defined(JAPAN)
			SetJapanText(-1);
#endif
			Sleep(1000);
		}		
#endif
		static int PreviousControl=0;
		
		ControlPacket = 0;

#if defined (UNDER_CE)
		ReallyReset = SoftResetTriggered;
		SoftResetTriggered = false;
#endif
		ControlPressed = GetAControl(ControlName, &Direction);
#if defined (UNDER_CE)
		if (SoftResetTriggered && ReallyReset)
			ReallyReset = false;
#endif

		if (ControlPressed != PreviousControl && BlankerCount <=BLANKER_TIMER || ((ControlPressed == -1 && AllControllerTypes[MainGameInfo.ControllerID] == CCT_WHEEL) || ControlPressed == DIK_LEFT || ControlPressed == DIK_RIGHT) && !PreventLeftRightRepetition)
		{
			BlankerCount = 0;
			switch (ControlPressed)
			{
			case -1:
				if (AllControllerTypes[MainGameInfo.ControllerID] != CCT_WHEEL) break;
				if (Direction < 0) ControlPacket |= 1 << CON_LEFT;
				if (Direction > 0) ControlPacket |= 1 << CON_RIGHT;
				if (!PreventLeftRightRepetition)
					PreviousControl = 0;
				break;
				
			case -2:
				break;
				if (Direction > 0) ControlPacket |= 1 << CON_BRAKE;
				if (Direction < 0) ControlPacket |= 1 << CON_THROTTLE;
				break;
				
			case -3:
				if (Direction > 0) ControlPacket |= 1 << CON_RTRIGGER;
				if (Direction < 0) ControlPacket |= 1 << CON_LTRIGGER;
				break;

			case DIK_LEFT:
				ControlPacket |= (1 << CON_DPAD_LEFT | 1 << CON_LEFT);
				if (!PreventLeftRightRepetition)
					PreviousControl = 0;
				break;
			case DIK_RIGHT:
				ControlPacket |= (1 << CON_DPAD_RIGHT | 1 << CON_RIGHT);
				if (!PreventLeftRightRepetition)
					PreviousControl = 0;
				break;
			case DIK_UP:
				ControlPacket |= (1 << CON_DPAD_UP | 1 << CON_THROTTLE);
				break;
			case DIK_DOWN:
				ControlPacket |= (1 << CON_DPAD_DOWN | 1 << CON_BRAKE);
				break;
				
			case DIK_B:
				ControlPacket |= 1 << CON_B;
				break;

			case DIK_A:
				ControlPacket |= (1 << CON_GLANCE | 1 << CON_A);
				break;

			case DIK_C:
				ControlPacket |= 1 << CON_C;
				break;
			case DIK_Z:
				ControlPacket |= 1 << CON_Z;
				break;

			case DIK_X:
				ControlPacket |= 1 << CON_X;
				if ((AllControllerTypes[MainGameInfo.ControllerID & 0x03]) == CCT_WHEEL)
					ControlPacket |= 1 << CON_BRAKE;
				break;

			case DIK_Y:
				ControlPacket |= 1 << CON_Y;
				if ((AllControllerTypes[MainGameInfo.ControllerID & 0x03])  == CCT_WHEEL)
					ControlPacket |= 1 << CON_THROTTLE;
				break;
				
			case DIK_K:
				if (UseKeys)
				{
					ControlPacket |= 1 << CON_KEY;
					ControlPacket |= Direction;
				}
				break;

			case DIK_Q:
				LeaveLoop = true;
				break;
			case DIK_S:
			case DIK_ESCAPE:
				ControlPacket |= 1 << CON_START;
				break;
			}
		}
		if (ControlPressed != PreviousControl)
			BlankerCount = 0;

		PreviousControl = ControlPressed;

#if defined (UNDER_CE)
		if (!ReallyReset && ! SoftResetTriggered)
			Result = MainGame->ControlPressed(ControlPacket);
#endif

#if defined(BORISEMULATIONHACK)
		renderBeginScene(FALSE);
#else
		boris2->beginFrame(false,0);
#endif
		MainGame->Update();

		MainGame->DrawBackground();

		if (BlankerCount >= BLANKER_TIMER)
		{
			DarkenScreen(0,0,640,480,D3DRGBA(0.0f,0.0f,0.0f,((float)(BlankerCount-BLANKER_TIMER))/60.0f*0.70f),.00001f);
		}


#if defined(BORISEMULATIONHACK)
		EmulationRender();
#else
		boris2->render();
#endif

#if defined(JAPAN)
		RenderJapanText();
#endif
		renderEndScene();
		renderFlipSurface();

	}
#if !defined(PROPER_FRONTEND)
	FillInGameFields();
	DeinitialiseMemories();
#endif
	if (ReallyReset) MainGame->DestroyScreen();
	MainGame->Destroy();
	delete MainGame;

	removeModelStore();
	removeTextureStore();

	delete boris2camera;
	delete boris2display;

	CloseHandle(hNewDeviceEvent);	hNewDeviceEvent = NULL;
	CloseHandle(hDeviceGoneEvent);	hDeviceGoneEvent= NULL;

//	if (BlankerCount >= BLANKER_TIMER)
//		return false;

	if (ReallyReset)
		return false;
	else
		return true;
}

void ConsoleFrontLib::InitialiseModels()
{

}
