// $Header$

//$Log$
//Revision 1.2  2000-01-17 12:40:04+00  jjs
//Latest demo version.
//
//Revision 1.1  2000-01-13 17:29:23+00  jjs
//First version that supports the loading of objects.
//

#define D3D_OVERLOADS

#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "rgraphics.h"
#include "allcar.h"

// ++++ Global Variables ++++++++++++++++++++++++++++++++++++++++++++
HWND main_hwnd;
HINSTANCE g_hinst;                                  // HINSTANCE of the application
car* myCar;
camera  * boris2camera = 0;
textureData* backs[6];
textureData* test4444;
BorisMaterial backM[6];

#if !defined(UNDER_CE)
#define FULLSCREEN 1
#endif

#define ZBACK 0.99999f
#define ZBACK2 0.89999f
#define WBACK 0.000001f
#define WBACK2 0.000002f
D3DTLVERTEX backv[6][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.875f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.875f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.875f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.875f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,1.0f,0.875f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,480.0f,0.99999f),WBACK,0xffffffff,0xffffffff,0.0f,0.875f)
	},
};

// ++++ Local Variables +++++++++++++++++++++++++++++++++++++++++++++
TCHAR     g_tszAppName[] = TEXT("SOS1937");        // The App's Name

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

void LoadBackDrop(void)
{
	int i;

	arcPush("\\pc\\optionscreen.tc");
	backs[0]= getTexturePtr("1.3df",false,false);
	backs[1]= getTexturePtr("2.3df",false,false);
	backs[2]= getTexturePtr("3.3df",false,false);
	backs[3]= getTexturePtr("4.3df",false,false);
	backs[4]= getTexturePtr("5.3df",false,false);
	backs[5]= getTexturePtr("6.3df",false,false);
	arcPop();
	for(i=0;i<6;++i)
	{
		backM[i].textureHandle = backs[i]->textureHandle;
		backM[i].flags = MAT_SCLAMP | MAT_TCLAMP;
	}
}

void DrawBackDrop(void)
{
	int i;

	for(i=0;i<6;++i)
	{
		renderSetCurrentMaterial(&backM[i]);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, backv[i],4,D3DDP_DONOTCLIP );
	}
}

void UpdateFrame(void)
{
	renderBeginScene(FALSE);
	DrawBackDrop();
	boris2->processObjectUnlit(myCar->mainObject);
	renderEndScene();
	renderFlipSurface();
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

#if defined(UNDER_CE) || FULLSCREEN
	main_hwnd = CreateWindowEx (0, g_tszAppName, g_tszAppName, WS_VISIBLE, 0, 0, 640, 480, NULL, NULL, g_hinst, NULL);
#else
	main_hwnd = CreateWindowEx (0, g_tszAppName, g_tszAppName, WS_VISIBLE, 0, 0, 646, 505, NULL, NULL, g_hinst, NULL);
#endif
	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch (uMessage)
    {
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_ESCAPE:
				case VK_F12:
					DestroyWindow(hWnd);
					return 0;

			} // switch (wParam)
			break;

		case WM_DESTROY:
			// Cleanup DirectX structures
//			CleanUp();

			PostQuitMessage(0);
			return 0L;

    } // switch (message)

    return DefWindowProc (hWnd, uMessage, wParam, lParam);
}

extern "C" int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
	float rot = 0.0f;

    // Store Instance handle for use later...
    g_hinst = hInstance;

    // Call initialization procedure
	make_window(hInstance,nCmdShow);

#if defined(UNDER_CE)
	renderInitialise(main_hwnd);
#else
	renderInitialise(main_hwnd, true);
#endif

	AddLight(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

	startEngine(40000, 40000, 40000,	200.0f );

	boris2camera  = new camera( &vector3(0,3.0,0), 600.0f, 30000.0f ); // zeye 0 = 360 900 = 90 e.t.c.

	boris2->setCurrentCamera( boris2camera );							// associate camera to engine

	
	myCar = new car("\\pc\\AutoUnionC_4.tc", 1.0, 0);

vector3 t;
			float x=320.0f,y=240.0f,z=0.0003f;
//	myCar->setBodyPosition(&vector3(-1.1f, -0.5f, 12.0f));
	myCar->setBodyPosition(&t);

	myCar->mainObject->inheritTransformations();

	LoadBackDrop();
	// Main Message loop
    while (TRUE)
    {
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            // There's a message waiting in the queue for us.  Retrieve
            // it and dispatch it, unless it's a WM_QUIT.
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
        else
        {
            // If no messages pending, then update the frame.

            // Do game-related calculations (physics, AI, etc).
			rot +=0.05f;
            // ...
	boris2->getWorldFromScreen(&t,x,y,z);

	myCar->setBodyPosition(&t);
			myCar->mainObject->setAngle_Y(rot);
			myCar->mainObject->inheritTransformations();

            // Render the frame
            UpdateFrame();
        }
    }

    return msg.wParam;
}

