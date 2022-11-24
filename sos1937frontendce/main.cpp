// $Header$

// $Log$
// Revision 1.11  2000-04-04 18:41:36+01  img
// Load and start on race over
//
// Revision 1.10  2000-03-20 18:01:16+00  img
// Beta build
//
// Revision 1.9  2000-03-07 14:24:20+00  img
// Now emulates a re-entry point of REP_SINGLERACEOVER
//
// Revision 1.8  2000-03-06 12:38:36+00  jjs
// Added renderDeInitialise and DestroyWindow to cleanup for WinCE 2.1
//
// Revision 1.7  2000-03-03 16:00:10+00  img
// Bug fixed when controller is removed.
//
// Revision 1.6  2000-03-03 15:15:40+00  jjs
// Fixed Light and reduced memory usage.
//

// Dreamcast front end!!! 

// Started 17/01/2000 Ian Gledhill

#define D3D_OVERLOADS
#define STRICT

#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <dinput.h>
#include <maplusag.h>
#include <platutil.h>

#include "rgraphics.h"
#include "allcar.h"

// Front end stuff.
#include <ConsoleFrontLib.h>

// ******** Globals ***************
HWND main_hwnd;
HINSTANCE g_hinst;
LPDIRECTINPUT  g_lpdi;
LPDIRECTINPUTDEVICE g_lpdid;
LPDIRECTINPUTDEVICE2 Controller[4];
int AllControllerTypes[4];
GUID g_guid;
extern struct GlobalStructure MainGameInfo;
// Set to 0 for the initial run. Set to 1 for Single race over.
int ReEntryPoint=0;//REP_SINGLERACEOVER;//0;

// ******** Locals  ***************
TCHAR	g_tszAppName[] = TEXT("SOS1937");

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_CHAR:
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		case VK_F12:
			DestroyWindow(hWnd);
			return 0;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

bool make_window( HINSTANCE hInstance, int nCmdShow )
{
    WNDCLASS  cls;          

	//	Register a class for the main application window
	cls.hCursor 	   = NULL;
	cls.hIcon		   = NULL;
	cls.lpszMenuName   = NULL;
	cls.hbrBackground  = NULL;
	cls.hInstance	   = g_hinst;
	cls.lpszClassName  = g_tszAppName;
	cls.lpfnWndProc    = (WNDPROC)WndProc;
	cls.style		   = 0;
	cls.cbWndExtra	   = 0;
	cls.cbClsExtra	   = 0;

	RegisterClass(&cls);

	main_hwnd = CreateWindowEx (0, g_tszAppName, g_tszAppName, WS_VISIBLE, 0, 0, 640, 480, NULL, NULL, g_hinst, NULL);

	return true;
}

BOOL CALLBACK
DIEnumProc(LPCDIDEVICEINSTANCE pdidi, LPVOID pvContext)
{
	HRESULT hr;

	OutputDebugString(pdidi->tszProductName);
	hr = g_lpdi->CreateDevice(pdidi->guidInstance, &g_lpdid, NULL);
	if (hr) exit(-1);

	// Find the port number.
	int Port;
	DIPROPDWORD dipdw; // DIPROPDWORD contains a DIPROPHEADER structure.
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0; // device property
	dipdw.diph.dwHow = DIPH_DEVICE;
	
	hr = g_lpdid->GetProperty(DIPROP_PORTNUMBER, &dipdw.diph);
	if (hr) FatalError("Couldn`t GetProperty() on device");
	Port = dipdw.dwData;

	if (Controller[Port])
	{
		dprintf("Controller already enum'ed");
		return true;
	}

	dprintf("Controller is in port %d", Port);

	AllControllerTypes[Port] = CCT_DREAMPAD;

	char Buffer[80];

	UNItoASCII((unsigned short *)pdidi->tszProductName, Buffer);

	if (!strnicmp("Racing Controller", Buffer, strlen("Racing Controller")))
		AllControllerTypes[Port] = CCT_WHEEL;
	if (!strnicmp("Arcade Stick", Buffer, strlen("Arcade Stick")))
		AllControllerTypes[Port] = CCT_STICK;
	if (!strnicmp("Keyboard", Buffer, strlen("Keyboard")))
		AllControllerTypes[Port] = CCT_KEYBOARD;
	if (!strnicmp("Dreamcast Fishing Controller", Buffer, strlen("Dreamcast Fishing Controller")))
		AllControllerTypes[Port] = CCT_FISHINGROD;

	hr = g_lpdid->QueryInterface(IID_IDirectInputDevice2, (LPVOID*)&Controller[Port]);
	if (hr) exit(hr);
	g_lpdid->Release();

	if (AllControllerTypes[Port] == CCT_KEYBOARD)
		hr = Controller[Port]->SetDataFormat(&c_dfDIKeyboard);
	else hr = Controller[Port]->SetDataFormat(&c_dfDIJoystick);
	if (hr) exit(hr);

	hr = Controller[Port]->Acquire();
	if (hr) FatalError("Erroring acquiring controller %d: %d", Port, hr);

	// Continue enum-ing.
	return true;
}

int InitDirectInput()
{
    // Create the global DirectInput object
    DirectInputCreate(g_hinst, DIRECTINPUT_VERSION, &g_lpdi, NULL);

	// Set each of the ports to NULL.
	for (int i=0 ; i<4 ; i++)
	{
		Controller[i] = NULL;
	}

	g_lpdi->EnumDevices(0, DIEnumProc, NULL, 0);

	return true;
}

extern "C" int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	g_hinst = hInstance;

	make_window(hInstance, nCmdShow);

	renderInitialise(main_hwnd);

	if (!InitDirectInput())
		return FALSE;

	AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
	startEngine(100, 100, 100,	200.0f );

	while (1)
	{
		ConsoleFrontLib *MainFrontLib = new ConsoleFrontLib();

		MainFrontLib->Start();
/*
		while (0 && TRUE)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
			}
		}
*/		
		delete MainFrontLib;

//		delete boris2;

		int NumberOfCars=0, i=-1;
		while (++i<10 && MainGameInfo.CarTypes[i]) NumberOfCars = i+1;
		bool AlreadyUsed = true;

//		MainGameInfo.Positions[0] = 5	;
		
		for (i=0 ; i<NumberOfCars ; i++)
		{
			while (AlreadyUsed)
			{
				MainGameInfo.Positions[i] = rand() % NumberOfCars;
				AlreadyUsed = false;
				for (int j=0 ; j<NumberOfCars ; j++)
				{
					if (i != j && MainGameInfo.Positions[j] == MainGameInfo.Positions[i]) AlreadyUsed = true;
				}
			}
			AlreadyUsed = true;
		}
		dprintf("Set player position to %d", MainGameInfo.Positions[0]);
	}

#ifndef BORISEMULATIONHACK
	renderDeInitialise();
#endif
	DestroyWindow( main_hwnd );

	return msg.wParam;
}

#define CHECKBUTTON(x,y)	if (DIJoyState.rgbButtons[ x ]) return y ;

int TranslateControl(int ThisController, DIJOYSTATE DIJoyState, int *Direction)
{
	switch (ThisController)
	{
	case CCT_DREAMPAD:
		if (   DIJoyState.rgbButtons[1] && DIJoyState.rgbButtons[0] && DIJoyState.rgbButtons[7] && DIJoyState.rgbButtons[8]
			&& DIJoyState.rgbButtons[2]) ResetToFirmware();

		if (DIJoyState.lX < 0x40) 
		{
			*Direction = -1;
			return -1;
		}
		if (DIJoyState.lX > 0xB0)
		{
			*Direction = 1;
			return -1;
		}
		if (DIJoyState.lY < 0x40)
		{
			*Direction = -1;
			return -2;
		}
		if (DIJoyState.lY > 0xB0)
		{
			*Direction = 1;
			return -2;
		}
		if (DIJoyState.rgbButtons[9])
		{
			*Direction = 1;
			return -3;
		}
		if (DIJoyState.rgbButtons[10])
		{
			*Direction = -1;
			return -3;
		}

		CHECKBUTTON(0,DIK_B);
		CHECKBUTTON(1,DIK_A);
		CHECKBUTTON(2,DIK_S);
		CHECKBUTTON(3,DIK_UP);
		CHECKBUTTON(4,DIK_DOWN);
		CHECKBUTTON(5,DIK_LEFT);
		CHECKBUTTON(6,DIK_RIGHT);
		CHECKBUTTON(7,DIK_Y);
		CHECKBUTTON(8,DIK_X);
		break;
	case CCT_WHEEL:
		// 0 is B
		// 1 is A
		// 2 is Start
		// 3 is Plus
		// 4 is Minus
		// 7 is RPaddle
		// 8 is LPaddle

		if (DIJoyState.lX < 0x40) 
		{
			*Direction = -1;
			return -1;
		}
		if (DIJoyState.lX > 0xB0)
		{
			*Direction = 1;
			return -1;
		}
		
		CHECKBUTTON(0,DIK_B);
		CHECKBUTTON(1,DIK_A);
		CHECKBUTTON(2,DIK_S);
		CHECKBUTTON(3,DIK_Y);
		CHECKBUTTON(4,DIK_X);
		if (DIJoyState.rgbButtons[7])
		{
			*Direction = 1;
			return -3;
		}
		if (DIJoyState.rgbButtons[8])
		{
			*Direction = -1;
			return -3;
		}
		break;
	case CCT_STICK:
		// 0 is C
		// 1 is B
		// 2 is A
		// 3 is Start
		// 4 is Up
		// 5 is Down
		// 6 is Left
		// 7 is Right
		// 8 is Z
		// 9 is Y
		// 10 is X
		if (   DIJoyState.rgbButtons[1] && DIJoyState.rgbButtons[2] && DIJoyState.rgbButtons[9] && DIJoyState.rgbButtons[10]
			&& DIJoyState.rgbButtons[3]) ResetToFirmware();

		CHECKBUTTON(0,DIK_C);
		CHECKBUTTON(1,DIK_B);
		CHECKBUTTON(2,DIK_A);
		CHECKBUTTON(3,DIK_S);
		CHECKBUTTON(4,DIK_UP);
		CHECKBUTTON(5,DIK_DOWN);
		CHECKBUTTON(6,DIK_LEFT);
		CHECKBUTTON(7,DIK_RIGHT);
		CHECKBUTTON(8,DIK_Z);
		CHECKBUTTON(9,DIK_Y);
		CHECKBUTTON(10,DIK_X);
		break;
	case CCT_KEYBOARD:
		break;
	case CCT_FISHINGROD:
		if (DIJoyState.rgbButtons[9] > 60) return DIK_A;
		if (DIJoyState.rgbButtons[10] > 200) return DIK_DOWN;
		if (DIJoyState.rgbButtons[10] < 56) return DIK_UP;
		if (DIJoyState.rgbButtons[11] > 200) return DIK_RIGHT;
		if (DIJoyState.rgbButtons[11] < 56) return DIK_LEFT;
		if (DIJoyState.rgbButtons[0]) return DIK_B;
//		if (DIJoyState.rgbButtons[2]) return DIK_S;

		break;
	default:
		break;
	}
	return 0;
}

BYTE g_byDIKtoASCII[255] = {0x0, 27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x8, 0x9,
                            'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\r', 0x0,
                            'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 0x0, 0x0, '\\',
                            'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x0, '*', 0x0, ' '};

int GetAControl(char *Name, int *Direction)
{
	static HANDLE hNewDeviceEvent = NULL;

	// Set up an event which is used to detect a new controller.
	if (!hNewDeviceEvent)
		hNewDeviceEvent = CreateEvent(NULL, false, false, TEXT("MAPLE_NEW_DEVICE"));
	
	if (!hNewDeviceEvent)
		FatalError("Error creating hNewDeviceEvent");

	if (WaitForSingleObject(hNewDeviceEvent, 0) != WAIT_TIMEOUT)
	{
		Sleep(1000);
		for (int i=0 ; i<4 ; i++)
		{
			if (Controller[i])
			{
				Controller[i]->Unacquire();
				Controller[i]->Release();
				Controller[i] = NULL;
			}
		}
		g_lpdi->EnumDevices(0, DIEnumProc, NULL, 0);
	}

	DIJOYSTATE DIJoyState;
	HRESULT hr;
	DIDEVCAPS DeviceCapabilities;
	DeviceCapabilities.dwSize = sizeof(DIDEVCAPS);
	int Translated=0;

	for (int i=0 ; i<4 ; i++)
	{
		if (Controller[i])
			hr = Controller[i]->Poll();
		else
			hr = 0;

		if (hr == DIERR_UNPLUGGED)
		{
			Controller[i]->Unacquire();
			Controller[i]->Release();
			Controller[i] = NULL;
		}
		
		if (Controller[i])
		{
			if (AllControllerTypes[i] != CCT_KEYBOARD)
			{
				while ((hr = Controller[i]->GetDeviceState(sizeof(DIJoyState), &DIJoyState)) == E_PENDING);
				hr = Controller[i]->GetCapabilities(&DeviceCapabilities);
				if (hr) dprintf("Error %d", hr);
				
				Translated = TranslateControl(AllControllerTypes[i], DIJoyState, Direction);
				
				MainGameInfo.ControllerID = i | (MainGameInfo.ControllerID & (1 << RUMBLE_BIT));
				if (Translated) return Translated;
			}
			else
			{
				static BYTE KeyData[256];
				memset(KeyData, 0, 256);
				hr = Controller[i]->GetDeviceState(256,KeyData);

				for (int j=0 ; j<256 ; j++)
				{
					if (KeyData[j]) 
					{
						if (j == DIK_LEFT || j == DIK_RIGHT || j == DIK_UP || j == DIK_DOWN) return j;
						if (j == DIK_NUMPADENTER || j == DIK_RETURN) return DIK_A;
						*Direction = g_byDIKtoASCII[j];					
						return DIK_K;
					}
				}
			}
		}
	}
	return 0;
}

void startMusic()
{
}

void PlayFrontEndSound(int a)
{
}