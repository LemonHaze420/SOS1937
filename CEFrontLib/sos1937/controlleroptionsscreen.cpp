// $Header$

// $Log$
// Revision 1.33  2000-05-26 09:12:31+01  img
// Gotta be final version now...
//
// Revision 1.32  2000-05-16 16:13:22+01  img
// FINAL release build???
//
// Revision 1.31  2000-05-09 14:07:57+01  img
// Release Candidate 2
//
// Revision 1.30  2000-05-09 10:56:28+01  jcf
// <>
//
// Revision 1.29  2000-05-05 12:17:15+01  img
// Build 19
//
// Revision 1.28  2000-05-04 17:39:26+01  img
// Build 18
//
// Revision 1.27  2000-05-02 17:03:38+01  img
// Build 17
//
// Revision 1.26  2000-04-22 18:26:03+01  img
// Added C button to joystick support
//
// Revision 1.25  2000-04-21 19:06:01+01  img
// Bug fixes
//
// Revision 1.24  2000-04-20 12:56:46+01  img
// Memory leaks fixed
//
// Revision 1.23  2000-04-19 21:40:48+01  img
// fixes
//
// Revision 1.22  2000-04-19 21:37:59+01  img
// Localisation.
//
// Revision 1.21  2000-04-12 18:02:30+01  img
// re-enabled controllers
//
// Revision 1.20  2000-04-12 12:20:46+01  img
// Force dreampad
//
// Revision 1.19  2000-04-11 16:12:14+01  img
// Rumble pack support
//
// Revision 1.18  2000-04-10 17:20:12+01  img
// Removed force of dreampad
//
// Revision 1.17  2000-04-07 17:31:30+01  img
// Force dreampad
//
// Revision 1.16  2000-04-07 14:38:05+01  img
// Help support, generic background support
//
// Revision 1.15  2000-04-05 17:47:07+01  img
// Force dreampad
//
// Revision 1.14  2000-04-04 18:41:09+01  img
// Bug fix
//
// Revision 1.13  2000-04-03 14:52:48+01  img
// Proper choice of controller
//
// Revision 1.12  2000-03-21 17:28:25+00  img
// Pre-loading more now...
//
// Revision 1.11  2000-03-20 18:00:51+00  img
// Beta build
//
// Revision 1.10  2000-03-10 16:08:52+00  img
// Uses steering wheel properly (has toggle on Status)
//
// Revision 1.9  2000-03-06 13:03:12+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.8  2000-03-03 16:00:45+00  img
// Detects Steering wheel and uses correct model
//
// Revision 1.7  2000-03-03 15:25:24+00  img
// Support for steering wheel
//
// Revision 1.6  2000-03-02 16:43:53+00  img
// Preliminary steering wheel support
//
// Revision 1.5  2000-03-02 14:26:37+00  img
// Controller screen now working
//
// Revision 1.4  2000-02-29 11:28:15+00  jjs
// Added Header and Log lines.
//

/* The race type screen of the game.

	Ian Gledhill 26/01/2000 :-)
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

#include <dinput.h>

class CVibPack
{
	// What the &^$£% is this class doing in MAIN.CPP?!?!?!?! <yeesh>
	int a;
};

extern class ButtonBank *MainMenu;
extern bool PreventLeftRightRepetition;
extern struct GlobalStructure MainGameInfo;
extern LPDIRECTINPUTDEVICE2 Controller[4];
extern CVibPack *vibpacks[];
extern bool ReEnumCards;

#define LOADMODELPART(c,x)		LowModel[x] = ControllerLowModel[c][x]; \
								HighModel[x] = ControllerHighModel[c][x]; \
	 						    LowModel[x]->Draw(); if (HighModel[x]) HighModel[x]->Hide();

#define LOADFUNCTIONWORD(x,y,z) LowlightedWord[z] = new TextWord(y, MainAlphabet); HighlightedWord[z] = new TextWord(y, YellowAlphabet); \
 								LowlightedWord[z]->Justify(JUSTIFY_RIGHT); HighlightedWord[z]->Justify(JUSTIFY_RIGHT);

#define LOADBUTTONWORD(x,y,z)	LowButtonWord[z] = new TextWord(y, MainAlphabet); HighButtonWord[z] = new TextWord(y, YellowAlphabet); \
 								LowButtonWord[z]->Justify(JUSTIFY_LEFT); HighButtonWord[z]->Justify(JUSTIFY_LEFT); \
								LowButtonWord[z]->Write(0,0,-100.0f); LowButtonWord[z]->Hide(); \
								HighButtonWord[z]->Write(0,0,-100.0f); HighButtonWord[z]->Hide();

#define POSITIONWORD(n,x,y,z) LowlightedWord[ n ]->Write( x , y , z); \
						  HighlightedWord[ n ]->Write( x , y , z ); HighlightedWord[ n ]->Hide();

#define POSITIONBUTTONWORD(n,x,y,z)		if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][ n ] < 65535) { LowButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][ n ]]->Write( x , y , z ); \
											HighButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][ n ]]->Write( x , y , z ); \
											HighButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][ n ]]->Hide(); }

void ControllerOptionsScreen::ConstructControllerTable(int ControllerType)
{
	switch (ControllerType)
	{
	case CCT_DREAMPAD:
		POSITIONWORD(GS_ACCELERATE, 310,330, 2000.0f);
		POSITIONWORD(GS_BRAKE, 310, 350, 2000.0f);
		POSITIONWORD(GS_GEARUP, 310, 370, 2000.0f);
		POSITIONWORD(GS_GEARDOWN, 310, 390, 2000.0f);
		POSITIONWORD(GS_REARVIEW, 505,330, 2000.0f);
		POSITIONWORD(GS_VIEW, 505,350, 2000.0f);
		POSITIONWORD(GS_STATUS, 505, 370, 2000.0f);
		POSITIONWORD(GS_RUMBLE, 380, 310, 2000.0f);
		OKWhite->Write(485,390, 2000.0f);
		OKYellow->Write(485,390, 2000.0f);
		OKYellow->Hide();

		POSITIONBUTTONWORD(GS_ACCELERATE, 325, 330, 2000.0f); 
		POSITIONBUTTONWORD(GS_BRAKE, 325, 350, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARUP, 325, 370, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARDOWN, 325, 390, 2000.0f);
		POSITIONBUTTONWORD(GS_REARVIEW, 520, 330, 2000.0f);
		POSITIONBUTTONWORD(GS_VIEW, 520, 350, 2000.0f);
		POSITIONBUTTONWORD(GS_STATUS, 520, 370, 2000.0f);
	
		DrawStatusToggle();

		break;
	case CCT_WHEEL:
		POSITIONWORD(GS_ACCELERATE, 310,330, 2000.0f);
		POSITIONWORD(GS_BRAKE, 310, 350, 2000.0f);
		POSITIONWORD(GS_GEARUP, 310, 370, 2000.0f);
		POSITIONWORD(GS_GEARDOWN, 310, 390, 2000.0f);
		POSITIONWORD(GS_REARVIEW, 505,330, 2000.0f);
		POSITIONWORD(GS_VIEW, 505,350, 2000.0f);
		POSITIONWORD(GS_STATUS, 505, 370, 2000.0f);

		OKWhite->Write(485,390, 2000.0f);
		OKYellow->Write(485,390, 2000.0f);
		OKYellow->Hide();

		POSITIONBUTTONWORD(GS_ACCELERATE, 325, 330, 2000.0f); 
		POSITIONBUTTONWORD(GS_BRAKE, 325, 350, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARUP, 325, 370, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARDOWN, 325, 390, 2000.0f);
		POSITIONBUTTONWORD(GS_REARVIEW, 520, 330, 2000.0f);
		POSITIONBUTTONWORD(GS_VIEW, 520, 350, 2000.0f);

		DrawStatusToggle();

		break;
	case CCT_STICK:
		POSITIONWORD(GS_ACCELERATE, 310,330, 2000.0f);
		POSITIONWORD(GS_BRAKE, 310, 350, 2000.0f);
		POSITIONWORD(GS_GEARUP, 310, 370, 2000.0f);
		POSITIONWORD(GS_GEARDOWN, 310, 390, 2000.0f);
		POSITIONWORD(GS_REARVIEW, 505,330, 2000.0f);
		POSITIONWORD(GS_VIEW, 505,350, 2000.0f);
		POSITIONWORD(GS_STATUS, 505, 370, 2000.0f);

		OKWhite->Write(485,390, 2000.0f);
		OKYellow->Write(485,390, 2000.0f);
		OKYellow->Hide();

		POSITIONBUTTONWORD(GS_ACCELERATE, 325, 330, 2000.0f); 
		POSITIONBUTTONWORD(GS_BRAKE, 325, 350, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARUP, 325, 370, 2000.0f);
		POSITIONBUTTONWORD(GS_GEARDOWN, 325, 390, 2000.0f);
		POSITIONBUTTONWORD(GS_REARVIEW, 520, 330, 2000.0f);
		POSITIONBUTTONWORD(GS_VIEW, 520, 350, 2000.0f);

		DrawStatusToggle();

		break;
	default:
		break;
	}
}
//#define POSITIONING

void ControllerOptionsScreen::DrawStatusToggle()
{
	OnButtonWhite->Hide();	OffButtonWhite->Hide();
	OnButtonYellow->Hide(); OffButtonYellow->Hide();
	
	if (ControllerType == CCT_DREAMPAD)
	{
		if (MainGameInfo.ControllerID & (1 << RUMBLE_BIT))
		{
			if (SelectedWord == GS_RUMBLE)
				OnButtonYellow->Write();
			else
				OnButtonWhite->Write();
			LowButtonWord[GS_RUMBLE] = OnButtonWhite;
			HighButtonWord[GS_RUMBLE] = OnButtonYellow;
		}
		else
		{
			if (SelectedWord == GS_RUMBLE)
				OffButtonYellow->Write();
			else
				OffButtonWhite->Write();
			LowButtonWord[GS_RUMBLE] = OffButtonWhite;
			HighButtonWord[GS_RUMBLE] = OffButtonYellow;
		}
	}
	else
	{
		if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][GS_STATUS])
		{
			if (SelectedWord == GS_STATUS)
				OnButtonYellow->Write(555,370,2000.0f);
			else
				OnButtonWhite->Write(555,370,2000.0f);
			LowButtonWord[GS_UP] = OnButtonWhite;
			HighButtonWord[GS_UP] = OnButtonYellow;
		}
		else
		{
			if (SelectedWord == GS_STATUS)
				OffButtonYellow->Write(555,370,2000.0f);
			else
				OffButtonWhite->Write(555,370,2000.0f);
			LowButtonWord[GS_UP] = OffButtonWhite;
			HighButtonWord[GS_UP] = OffButtonYellow;
		}
	}
}

void ControllerOptionsScreen::InitialiseRotations()
{
	for (int i=0 ; i<3 ; i++)
	{
		for (int j=0 ; j<14 ; j++)
		{
			RotationPosition[i][j] = vector3(0.0f,0.0f,0.0f);
		}
	}
	RotationPosition[CCT_DREAMPAD][GS_RTRIGGER] = vector3(10.0f,120.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_LTRIGGER] = vector3(10.0f,-120.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_A]		= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_B]		= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_X]		= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_Y]		= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_DIRECTION]= vector3(36.0f,-66.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_START]	= vector3(36.0f,0.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_UP]		= vector3(36.0f,0.0f,0.0f);
	RotationPosition[CCT_DREAMPAD][GS_RUMBLE]	= vector3(36.0f,0.0f,0.0f);

	RotationPosition[CCT_WHEEL][GS_RTRIGGER]	= vector3(10.0f,120.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_LTRIGGER]	= vector3(10.0f,-120.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_A]			= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_B]			= vector3(36.0f,-60.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_X]			= vector3(36.0f,-60.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_Y]			= vector3(36.0f,60.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_DIRECTION]	= vector3(36.0f,-66.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_START]		= vector3(36.0f,0.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_UP]			= vector3(36.0f,0.0f,0.0f);
	RotationPosition[CCT_WHEEL][GS_RUMBLE]		= vector3(36.0f,0.0f,0.0f);

	RotationPosition[CCT_STICK][GS_A]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_B]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_C]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_X]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_Y]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_Z]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_START]		= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_UP]			= vector3(40.0f,0.0f,0.0f);
	RotationPosition[CCT_STICK][GS_RUMBLE]		= vector3(36.0f,0.0f,0.0f);
}

void ControllerOptionsScreen::RotateController(int ControllerType, int Button)
{
	if (Button < GS_UP)
	{
		CurrentRotation = &RotationPosition[ControllerType][Button];
	}
	else
	{
		CurrentRotation = &RotationPosition[ControllerType][GS_DIRECTION];
	}
}

extern int AllControllerTypes[4];

ControllerOptionsScreen::ControllerOptionsScreen(Alphabet *DefaultAlphabet)
{
	CurrentPlayer = 0;
		
	CurrentRotation = NULL;

	MainAlphabet = DefaultAlphabet;

	DIDEVCAPS DeviceCapabilities;
	DeviceCapabilities.dwSize = sizeof(DeviceCapabilities);

	// Use the wheel type if we have a wheel connected.
	ControllerType = AllControllerTypes[MainGameInfo.ControllerID & 3];
	
	if (ControllerType == -1) ControllerType = CCT_DREAMPAD;
	if (ControllerType != CCT_WHEEL && ControllerType != CCT_STICK && ControllerType != CCT_DREAMPAD)
		ControllerType = CCT_DREAMPAD;

//	ControllerType = CCT_DREAMPAD;

	// Initialise the Controller's rotation
	InitialiseRotations();

	for (int i=0 ; i<14 ; i++)
	{
		LowModel[i] = NULL;
		HighModel[i] = NULL;
		LowButtonWord[i] = NULL;
		HighButtonWord[i] = NULL;
	}

	VibrationPossible = false;
	if (vibpacks[MainGameInfo.ControllerID & 3]) VibrationPossible = true;

	switch (ControllerType)
	{
	case CCT_DREAMPAD:
		ControllerModel = GlobalController;
		
		LOADMODELPART(CCT_DREAMPAD, GS_ANALOGUE);
		LOADMODELPART(CCT_DREAMPAD, GS_DIRECTION);
		LOADMODELPART(CCT_DREAMPAD, GS_A);
		LOADMODELPART(CCT_DREAMPAD, GS_B);
		LOADMODELPART(CCT_DREAMPAD, GS_X);
		LOADMODELPART(CCT_DREAMPAD, GS_Y);
		LOADMODELPART(CCT_DREAMPAD, GS_LTRIGGER);
		LOADMODELPART(CCT_DREAMPAD, GS_RTRIGGER);
		
		LOADFUNCTIONWORD(Accelerate, TranslateLocale(5), GS_ACCELERATE);
		LOADFUNCTIONWORD(Brake, , TranslateLocale(6), GS_BRAKE);
		LOADFUNCTIONWORD(GearUp, TranslateLocale(7), GS_GEARUP);
		LOADFUNCTIONWORD(GearDown, TranslateLocale(8), GS_GEARDOWN);
		LOADFUNCTIONWORD(RearView, TranslateLocale(9), GS_REARVIEW);
		LOADFUNCTIONWORD(View, TranslateLocale(10), GS_VIEW);
		LOADFUNCTIONWORD(Status, TranslateLocale(11), GS_STATUS);
		LOADFUNCTIONWORD(RumblePack, TranslateLocale(12), GS_RUMBLE);

		LOADBUTTONWORD(LTrigger, TranslateLocale(13), GS_LTRIGGER);
		LOADBUTTONWORD(RTrigger, TranslateLocale(14), GS_RTRIGGER);
		LOADBUTTONWORD(A, TranslateLocale(15), GS_A);
		LOADBUTTONWORD(B, TranslateLocale(16), GS_B);
		LOADBUTTONWORD(X, TranslateLocale(17), GS_X);
		LOADBUTTONWORD(Y, TranslateLocale(18), GS_Y);
		LOADBUTTONWORD(Up, TranslateLocale(19), GS_UP);
		LOADBUTTONWORD(Down, TranslateLocale(20), GS_DOWN);
		LOADBUTTONWORD(Left, TranslateLocale(21), GS_LEFT);
		LOADBUTTONWORD(Right, TranslateLocale(22), GS_RIGHT);
	
		OnButtonWhite = new TextWord(TranslateLocale(23), MainAlphabet);	OnButtonYellow = new TextWord(TranslateLocale(23), YellowAlphabet);
		OffButtonWhite = new TextWord(TranslateLocale(24), MainAlphabet);	OffButtonYellow = new TextWord(TranslateLocale(24), YellowAlphabet);

		OnButtonWhite->Write(440,310,2000.0f); OffButtonWhite->Write(440,310,2000.0f);
		OnButtonYellow->Write(440,310,2000.0f); OffButtonYellow->Write(440,310,2000.0f);
		OnButtonWhite->Hide();  OnButtonYellow->Hide();
		OffButtonWhite->Hide();	OffButtonYellow->Hide();

		LeftArrow->Draw(420, 310, 1400.0f);
		RightArrow->Draw(460, 310, 1400.0f);

		if (!VibrationPossible)
		{
			LeftArrow->Hide(); RightArrow->Hide();
		}

		break;
	case CCT_WHEEL:
		ControllerModel = GlobalWheel;

		LOADMODELPART(CCT_WHEEL, GS_A);
		LOADMODELPART(CCT_WHEEL, GS_B);
		LOADMODELPART(CCT_WHEEL, GS_X);
		LOADMODELPART(CCT_WHEEL, GS_Y);
		LOADMODELPART(CCT_WHEEL, GS_LTRIGGER);
		LOADMODELPART(CCT_WHEEL, GS_RTRIGGER);
		LOADMODELPART(CCT_WHEEL, GS_START);
		
		LOADFUNCTIONWORD(Accelerate, TranslateLocale(5), GS_ACCELERATE);
		LOADFUNCTIONWORD(Brake, , TranslateLocale(6), GS_BRAKE);
		LOADFUNCTIONWORD(GearUp, TranslateLocale(7), GS_GEARUP);
		LOADFUNCTIONWORD(GearDown, TranslateLocale(8), GS_GEARDOWN);
		LOADFUNCTIONWORD(RearView, TranslateLocale(9), GS_REARVIEW);
		LOADFUNCTIONWORD(View, TranslateLocale(10), GS_VIEW);
		LOADFUNCTIONWORD(Status, TranslateLocale(11), GS_STATUS);
		
		LOADBUTTONWORD(LTrigger, TranslateLocale(25), GS_LTRIGGER);
		LOADBUTTONWORD(RTrigger, TranslateLocale(26), GS_RTRIGGER);
		LOADBUTTONWORD(A, TranslateLocale(15), GS_A);
		LOADBUTTONWORD(B, TranslateLocale(16), GS_B);
		LOADBUTTONWORD(X, TranslateLocale(27), GS_X);
		LOADBUTTONWORD(Y, TranslateLocale(28), GS_Y);
		LOADBUTTONWORD(Up, TranslateLocale(19), GS_START);

		OnButtonWhite = new TextWord(TranslateLocale(23), MainAlphabet);	OnButtonYellow = new TextWord(TranslateLocale(23), YellowAlphabet);
		OffButtonWhite = new TextWord(TranslateLocale(24), MainAlphabet);	OffButtonYellow = new TextWord(TranslateLocale(24), YellowAlphabet);

		OnButtonWhite->Write(555,370,2000.0f); OffButtonWhite->Write(555,370,2000.0f);
		OnButtonYellow->Write(555,370,2000.0f); OffButtonYellow->Write(555,370,2000.0f);
		OnButtonWhite->Hide();  OnButtonYellow->Hide();
		OffButtonWhite->Hide();	OffButtonYellow->Hide();

		LeftArrow->Draw(525, 370, 1400.0f);
		RightArrow->Draw(590, 370, 1400.0f);

		break;

	case CCT_STICK:
		ControllerModel = GlobalStick;

		LOADMODELPART(CCT_STICK, GS_A);
		LOADMODELPART(CCT_STICK, GS_B);
		LOADMODELPART(CCT_STICK, GS_X);
		LOADMODELPART(CCT_STICK, GS_Y);
		LOADMODELPART(CCT_STICK, GS_LTRIGGER);
		LOADMODELPART(CCT_STICK, GS_RTRIGGER);
		LOADMODELPART(CCT_STICK, GS_START);
		
		LOADFUNCTIONWORD(Accelerate, TranslateLocale(5), GS_ACCELERATE);
		LOADFUNCTIONWORD(Brake, , TranslateLocale(6), GS_BRAKE);
		LOADFUNCTIONWORD(GearUp, TranslateLocale(7), GS_GEARUP);
		LOADFUNCTIONWORD(GearDown, TranslateLocale(8), GS_GEARDOWN);
		LOADFUNCTIONWORD(RearView, TranslateLocale(9), GS_REARVIEW);
		LOADFUNCTIONWORD(View, TranslateLocale(10), GS_VIEW);
		LOADFUNCTIONWORD(Status, TranslateLocale(11), GS_STATUS);
		
		LOADBUTTONWORD(A, TranslateLocale(15), GS_A);
		LOADBUTTONWORD(B, TranslateLocale(16), GS_B);
		LOADBUTTONWORD(C, TranslateLocale(29), GS_C);
		LOADBUTTONWORD(X, TranslateLocale(17), GS_X);
		LOADBUTTONWORD(Y, TranslateLocale(18), GS_Y);
		LOADBUTTONWORD(Z, TranslateLocale(30), GS_Z);
		LOADBUTTONWORD(Up, TranslateLocale(19), GS_START);

		OnButtonWhite = new TextWord(TranslateLocale(23), MainAlphabet);	OnButtonYellow = new TextWord(TranslateLocale(23), YellowAlphabet);
		OffButtonWhite = new TextWord(TranslateLocale(24), MainAlphabet);	OffButtonYellow = new TextWord(TranslateLocale(24), YellowAlphabet);

		OnButtonWhite->Write(555,370,2000.0f); OffButtonWhite->Write(555,370,2000.0f);
		OnButtonYellow->Write(555,370,2000.0f); OffButtonYellow->Write(555,370,2000.0f);
		OnButtonWhite->Hide();  OnButtonYellow->Hide();
		OffButtonWhite->Hide();	OffButtonYellow->Hide();

		LeftArrow->Draw(525, 370, 1400.0f);
		RightArrow->Draw(590, 370, 1400.0f);

		break;
	default:
		break;
	}
	
	OKWhite = new TextWord(TranslateLocale(31), MainAlphabet);
	OKYellow = new TextWord(TranslateLocale(31), YellowAlphabet);
	OKWhite->Hide(); OKYellow->Hide();
	LowlightedWord[GS_OK] = OKWhite; HighlightedWord[GS_OK] = OKYellow;

	switch (ControllerType)
	{
	case CCT_STICK:
	case CCT_DREAMPAD:
		ControllerModel->Draw(415,225,600.0f);
		break;
	case CCT_WHEEL:
		ControllerModel->Draw(415,195,600.0f);
		break;
	}

	ControllerModel->Rotate(30.0f, 0.0f, 0.0f);

	ConstructControllerTable(ControllerType);	
	
	SelectedWord = GS_STATUS;
//	SelectedWord = GS_OK;
	HighlightWord(SelectedWord);
	if (ControllerType == CCT_DREAMPAD) HighlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
	TransitionPosition = 0;
	if (SelectedWord == GS_STATUS && ControllerType != CCT_DREAMPAD)
		RotateController(ControllerType, GS_START);
	else
		RotateController(ControllerType, MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
	

	for (i=0 ; i<GS_OK ; i++)
	{
		OldButtons[i] = MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][i];
	}
	OldButtons[GS_RUMBLE] = MainGameInfo.ControllerID & (1 << RUMBLE_BIT);

	// Reset the counter to zero.
	Counter = 0;

	WaitingForButton = false;
	
	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(32),TranslateLocale(33));

	// Repeat L/R
	PreventLeftRightRepetition = true;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void ControllerOptionsScreen::HighlightWord(int Word)
{
	LowlightedWord[Word]->Hide();
	HighlightedWord[Word]->Write();

	if (ControllerType != CCT_DREAMPAD && Word == GS_STATUS)
	{
		LowButtonWord[GS_UP]->Hide();
		HighButtonWord[GS_UP]->Write();
	}
	else if (ControllerType == CCT_DREAMPAD && Word == GS_RUMBLE)
	{
		LowButtonWord[GS_RUMBLE]->Hide();
		HighButtonWord[GS_RUMBLE]->Write();
	}
	else if (Word != GS_OK && MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word] < 65535)
	{
		LowButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word]]->Hide();
		HighButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word]]->Write();
	}
}

void ControllerOptionsScreen::LowlightWord(int Word)
{
	HighlightedWord[Word]->Hide();
	LowlightedWord[Word]->Write();

	if (ControllerType != CCT_DREAMPAD && Word == GS_STATUS)
	{
		HighButtonWord[GS_UP]->Hide();
		LowButtonWord[GS_UP]->Write();
	}
	else if (ControllerType == CCT_DREAMPAD && Word == GS_RUMBLE)
	{
		HighButtonWord[GS_RUMBLE]->Hide();
		LowButtonWord[GS_RUMBLE]->Write();
	}
	else if (Word != GS_OK && MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word] < 65535)
	{
		HighButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word]]->Hide();
		LowButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Word]]->Write();
	}
}

void ControllerOptionsScreen::HighlightModel(int ModelNumber)
{
	// No seperate model for up,down,left and right.
	if (ModelNumber > 8)
		ModelNumber = GS_DIRECTION;
	if (LowModel[ModelNumber])  LowModel[ModelNumber]->Hide();
	if (HighModel[ModelNumber]) HighModel[ModelNumber]->Draw();
}

void ControllerOptionsScreen::LowlightModel(int ModelNumber)
{
	if (ModelNumber > 8)
		ModelNumber = GS_DIRECTION;
	if (HighModel[ModelNumber])	HighModel[ModelNumber]->Hide();
	if (LowModel[ModelNumber])	LowModel[ModelNumber]->Draw();
}

void ControllerOptionsScreen::RemoveButtonText(int Function)
{
	if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Function] < 65535)
		HighButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Function]]->Hide();
}

void ControllerOptionsScreen::Update(struct Instruction *ScreenCommand)
{
	static bool JustEnummed = false;
	if (JustEnummed)
	{
		if (vibpacks[MainGameInfo.ControllerID & 3]) VibrationPossible = true; else VibrationPossible = false;

		if (VibrationPossible)
		{
			DrawStatusToggle();
			LeftArrow->Draw(); RightArrow->Draw();
		}	
		else
		{
			MainGameInfo.ControllerID &= ~(1 << RUMBLE_BIT);
			DrawStatusToggle();
			LeftArrow->Hide(); RightArrow->Hide();
			
			if (SelectedWord == GS_RUMBLE)
			{
				LowlightWord(SelectedWord);
				SelectedWord = 0;
				HighlightWord(SelectedWord);
			}
		}
	}
	JustEnummed = ReEnumCards;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	if (ControllerModel)
	{
		if (!CurrentRotation)
			ControllerModel->Rotate(30.0f+(float)cos(Counter*0.01f)*4.0f,(float)sin(Counter*0.013f)*3.0f,0.0f);
		else
		{
			if (!TransitionPosition)
			{
				ControllerModel->Rotate(CurrentRotation->x+(float)cos(Counter*0.01f)*4.0f, CurrentRotation->y+(float)sin(Counter*0.013f)*3.0f,CurrentRotation->z);
			}
			else
			{
				vector3 TransitionVector, PathVector;

				PathVector.x = CurrentRotation->x-StartRotation.x;
				PathVector.y = CurrentRotation->y-StartRotation.y;
				PathVector.z = CurrentRotation->z-StartRotation.z;

				TransitionVector.x = StartRotation.x + (float)cos(TransitionPosition/360.0f*2*PI)*PathVector.x;
				TransitionVector.y = StartRotation.y + (float)cos(TransitionPosition/360.0f*2*PI)*PathVector.y;
				TransitionVector.z = StartRotation.z + (float)cos(TransitionPosition/360.0f*2*PI)*PathVector.z;

				ControllerModel->Rotate(TransitionVector.x+(float)cos(Counter*0.01f)*4.0f, TransitionVector.y+(float)sin(Counter*0.013f)*3.0f,TransitionVector.z);

				TransitionPosition-=2;
			}
		}
#ifdef POSITIONING
		dprintf("Rotation %f %f %f", CurrentRotation->x, CurrentRotation->y, CurrentRotation->z);
#endif
	}

	if (Counter >= 0)
		Counter++;
}

void ControllerOptionsScreen::SetBasicHelp()
{
	DeleteHelp();
	SetHelpText(TranslateLocale(32),TranslateLocale(33));
}

void ControllerOptionsScreen::SetDefineHelp()
{
	DeleteHelp();
	SetHelpText(TranslateLocale(34), " ");
}

void ControllerOptionsScreen::AssignButton(int Function, int Button)
{
	for (int i=0 ; i<=GS_STATUS ; i++)
	{
		if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][i] == Button)
		{
			MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][i] = -1;
			if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Function] < 65535)
				LowButtonWord[MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Function]]->Hide();
		}
	}
	MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][Function] = Button;

	WaitingForButton = false;
}

void ControllerOptionsScreen::SetNewButton(int Function, int ControlPacket)
{
	int Control = MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord];
	if		(ControlPacket & 1 << CON_A)			AssignButton(Function, GS_A);
	else if (ControlPacket & 1 << CON_B)			AssignButton(Function, GS_B);
	else if (ControlPacket & 1 << CON_C)			AssignButton(Function, GS_C);
	else if (ControlPacket & 1 << CON_X)			AssignButton(Function, GS_X);
	else if (ControlPacket & 1 << CON_Y)			AssignButton(Function, GS_Y);
	else if (ControlPacket & 1 << CON_Z)			AssignButton(Function, GS_Z);
	else if (ControlPacket & 1 << CON_RTRIGGER)		AssignButton(Function, GS_RTRIGGER);
	else if (ControlPacket & 1 << CON_LTRIGGER)		AssignButton(Function, GS_LTRIGGER);

	if (ControllerType == CCT_DREAMPAD)
	{
		if		(ControlPacket & 1 << CON_DPAD_LEFT)	AssignButton(Function, GS_LEFT);
		else if (ControlPacket & 1 << CON_DPAD_RIGHT)	AssignButton(Function, GS_RIGHT);
		else if (ControlPacket & 1 << CON_DPAD_UP)		AssignButton(Function, GS_UP);
		else if (ControlPacket & 1 << CON_DPAD_DOWN)	AssignButton(Function, GS_DOWN);
	}

	if (!WaitingForButton)
	{
		LowlightModel(Control);
		ConstructControllerTable(ControllerType);

		HighlightWord(SelectedWord);
		RotateController(ControllerType, MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
		SetBasicHelp();
		HighlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
	}
}

int ControllerOptionsScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket && WaitingForButton)
	{
		SetNewButton(SelectedWord, ControlPacket);
	}
	else
	{
#ifdef POSITIONING
		if (ControlPacket & 1 << CON_BRAKE)
		{
			CurrentRotation->x+=2;
		}
		if (ControlPacket & 1 << CON_THROTTLE)
		{
			CurrentRotation->x-=2;
		}
		if (ControlPacket & 1 << CON_LEFT)
		{
			CurrentRotation->y-=2;
		}
		if (ControlPacket & 1 << CON_RIGHT)
		{
			CurrentRotation->y+=2;
		}
		if (ControlPacket & 1 << CON_GEARUP)
		{
			CurrentRotation->z-=2;
		}
		if (ControlPacket & 1 << CON_GEARDN)
		{
			CurrentRotation->z+=2;
		}
#else
		if (ControlPacket & 1 << CON_BRAKE)
		{
			LowlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
			LowlightWord(SelectedWord);
			SelectedWord++;
			if (SelectedWord > (ControllerType == CCT_DREAMPAD && VibrationPossible ? GS_RUMBLE : GS_OK)) SelectedWord = 0;
			if (SelectedWord != GS_RUMBLE && SelectedWord > GS_OK) SelectedWord = GS_RUMBLE;
			HighlightWord(SelectedWord);
			if (SelectedWord != GS_RUMBLE && (ControllerType == CCT_DREAMPAD && VibrationPossible ? 1 : SelectedWord != GS_STATUS) && SelectedWord != GS_OK && MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord] < 65535)
				HighlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
			
			StartRotation = *CurrentRotation;
			TransitionPosition = 90;
			
			if (SelectedWord == GS_OK || (ControllerType == CCT_DREAMPAD ? 0 : SelectedWord == GS_STATUS) || SelectedWord == GS_RUMBLE)
				RotateController(ControllerType, GS_START);
			else
				RotateController(ControllerType, MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
		}
		
		if (ControlPacket & 1 << CON_THROTTLE)
		{
			LowlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
			LowlightWord(SelectedWord);
			SelectedWord--;
			if (SelectedWord < 0) SelectedWord = (ControllerType == CCT_DREAMPAD && VibrationPossible ? GS_RUMBLE : GS_OK);
			if (SelectedWord != GS_RUMBLE && SelectedWord > GS_OK) SelectedWord = GS_OK;
			HighlightWord(SelectedWord);
			if (SelectedWord != GS_RUMBLE && (ControllerType == CCT_DREAMPAD && VibrationPossible ? 1 : SelectedWord != GS_STATUS) && SelectedWord != GS_OK && MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord] < 65535)
				HighlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);

			StartRotation = *CurrentRotation;
			TransitionPosition = 90;
	
			if (SelectedWord == GS_OK || (ControllerType == CCT_DREAMPAD ? 0 : SelectedWord == GS_STATUS) || SelectedWord == GS_RUMBLE)
				RotateController(ControllerType, GS_START);
			else
				RotateController(ControllerType, MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
		}

		if (ControlPacket & 1 << CON_LEFT || ControlPacket & 1 << CON_RIGHT)
		{
			if (SelectedWord == GS_RUMBLE && ControllerType == CCT_DREAMPAD)
			{
				MainGameInfo.ControllerID = (unsigned short)(MainGameInfo.ControllerID ^ (1 << RUMBLE_BIT));
				DrawStatusToggle();
			}
			else if (SelectedWord == GS_STATUS && ControllerType != CCT_DREAMPAD)
			{
				MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][GS_STATUS] = (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][GS_STATUS] ? 0 : GS_UP);
				DrawStatusToggle();
			}
		}
#endif
		if (ControlPacket & 1 << CON_GLANCE || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
		{
			switch (SelectedWord)
			{
			case GS_OK:
				{
					bool AllDefined = true;
					
					for (int i=0 ; i<GS_OK ; i++)
					{
						if (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][i] == 65535) AllDefined = false;
					}
					if (AllDefined)
					{
						MainMenu->Unselect(CONTROL);
						
						Destroy();
						// Inhibit L/R repetition
						PreventLeftRightRepetition = true;
						
						// Change screen.
						QueuedCommand = 'S';
						QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
						
						((FrontScreen *)QueuedValue)->SetHighlighted(CONTROL);
					}
					break;
				}
			case GS_RUMBLE:
				if (ControllerType == CCT_DREAMPAD)
				{
					MainGameInfo.ControllerID = (unsigned short)(MainGameInfo.ControllerID ^ (1 << RUMBLE_BIT));
					DrawStatusToggle();
					break;
				}
				break;
			case GS_STATUS:
				if (ControllerType != CCT_DREAMPAD)
				{
					MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][GS_STATUS] = (MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][GS_STATUS] ? 0 : GS_UP);
					DrawStatusToggle();
					break;
				}
				// No we do _not_ want a break; here.
			default:
				SetDefineHelp();
				RemoveButtonText(SelectedWord);
				WaitingForButton = true;
				break;
			}
		}
		
		if (ControlPacket & 1 << CON_B)
		{
			LowlightWord(SelectedWord);
			LowlightModel(MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][SelectedWord]);
			// Restore buttons to their original values.
			for (int i=0 ; i<GS_OK ; i++)
			{
				MainGameInfo.ButtonDefines[CurrentPlayer][ControllerType][i] = OldButtons[i];
			}
			if (!OldButtons[GS_RUMBLE])
				MainGameInfo.ControllerID &= ~(1 << RUMBLE_BIT);
			else
				MainGameInfo.ControllerID |= (1 << RUMBLE_BIT);
			MainMenu->Unselect(CONTROL);
			
			Destroy();
			// Inhibit L/R repetition
			PreventLeftRightRepetition = true;
			
			// Change screen.
			QueuedCommand = 'S';
			QueuedValue = (Screen *) new FrontScreen(MainAlphabet);
			
			((FrontScreen *)QueuedValue)->SetHighlighted(CONTROL);
		}
	}
	return 0;
}

// Not needed as we're using the default stuff.
void ControllerOptionsScreen::DrawBackground()
{
}

ControllerOptionsScreen::~ControllerOptionsScreen()
{
}

void ControllerOptionsScreen::Destroy()
{
	ControllerModel->Hide();

	for (int i=0 ; i<7 ; i++)
	{
		delete LowlightedWord[i];
		delete HighlightedWord[i];
	}
	if (ControllerType == CCT_DREAMPAD)
	{
		delete LowlightedWord[GS_RUMBLE]; delete HighlightedWord[GS_RUMBLE];
	}

	for (i=0 ; i<13 ; i++)
	{
		// Don`t want to delete GS_UP as this is a flag on the Race controller, not a value as such.
		if (ControllerType == CCT_DREAMPAD || i != GS_UP)
		{
			if (LowButtonWord[i]) delete LowButtonWord[i];
			if (HighButtonWord[i]) delete HighButtonWord[i];
		}
	}

	if (OnButtonWhite)
	{
		delete OnButtonWhite; 
		delete OffButtonWhite;
		delete OnButtonYellow;
		delete OffButtonYellow;

		LeftArrow->Hide();
		RightArrow->Hide();
	}

	delete OKWhite;
	delete OKYellow;

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	DeleteHelp();

	return;
}