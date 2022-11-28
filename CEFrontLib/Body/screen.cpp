// $Header$

// $Log$
// Revision 1.18  2000-07-04 12:59:18+01  img
// Permedia 3 support!
//
// Revision 1.17  2000-07-04 10:48:40+01  img
// More Abstraction
//
// Revision 1.16  2000-05-26 09:12:31+01  img
// Gotta be final version now...
//
// Revision 1.15  2000-05-04 16:24:58+01  jcf
// Only 2 static screens now.
//
// Revision 1.14  2000-05-04 10:53:33+01  img
// Changed text positions
//
// Revision 1.13  2000-04-28 11:09:18+01  jcf
// <>
//
// Revision 1.12  2000-04-26 10:08:55+01  img
// <>
//
// Revision 1.11  2000-04-07 14:37:12+01  img
// Added help support
//
// Revision 1.10  2000-04-03 14:51:38+01  img
// No change. Wouldn`t let me undo!
//
// Revision 1.9  2000-03-22 09:29:06+00  img
// <>
//
// Revision 1.8  2000-03-21 17:28:18+00  img
// Pre-loading more now...
//
// Revision 1.7  2000-03-09 10:35:09+00  img
// Use 512x512
//
// Revision 1.6  2000-03-08 10:32:34+00  img
// Temporarily use 3df backdrops
//
// Revision 1.5  2000-03-07 13:59:50+00  img
// Uses 5 pvr textures instead of 6 3dfs
//
// Revision 1.4  2000-03-06 13:03:11+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.3  2000-02-29 14:03:00+00  jjs
// Modified to work with Boris3.
//
// Revision 1.2  2000-02-18 16:14:29+00  img
// Pretty much complete for 18th
// _B_ack button implemented, Start button implemented, no crash when no memory card, Controller on FrontScreen and other minor changes.
//
// Revision 1.1  2000-01-19 17:53:09+00  img
// <>
//

/* A Screen entity holds the data for each er- screen.

   1/12/1999 Broadsword Interactive Ltd.

   Ian Gledhill */
#define D3D_OVERLOADS
#include "ConsoleFrontLib.h"

#if defined (UNDER_CE)
extern FrontEndObject *StartButton;
extern TextWord *StartText;
#endif

char *static_screens[5] = {NULL,NULL,NULL,NULL,NULL};
int static_screen_ct=0;

int Counter=0;

void InitStaticScreens()
{
	static_screens[0] = (char *)malloc(10000);
	static_screens[1] = (char *)malloc(10000);
}

Screen::Screen()
{
	UseDefaultBackdrop = true;
}

Screen::~Screen()
{
}

void Screen::SetCounter(int NewCounter)
{
	Counter = NewCounter;
}

#define BACK 0.9999999999f
#define RBACK 0.000001f

D3DTLVERTEX smallbackv[5][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
};

#if !defined(PROPER_FRONTEND)

#define LARGEVERTICES 8
D3DTLVERTEX largebackv[LARGEVERTICES][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
};
#else

#define LARGEVERTICES 6
#define X1 1.0f/512.0f
#define X2 1.0f-1.0f/512.0f
#define Y1 1.0f/512.0f
#define Y2 1.0f-1.0f/512.0f
#define Xa1 1.0f/256.0f
#define Xa2 1.0f-1.0f/256.0f

D3DTLVERTEX largebackv[LARGEVERTICES][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,Xa1,Y2)
	}
};
#endif

void Screen::DrawFullScreen2D(DWORD _Background[], bool MoreTextures)
{
	int i;
	
	if (MoreTextures)
	{
		for(i=0;i<LARGEVERTICES;++i)
		{
			renderSetCurrentMaterial(&(((BorisMaterial *)(_Background))[i]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, largebackv[i],4,D3DDP_DONOTCLIP );
		}
	}
	else
	{
		for(i=0;i<5;++i)
		{
			renderSetCurrentMaterial(&(((BorisMaterial *)(_Background))[i]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, smallbackv[i],4,D3DDP_DONOTCLIP );
		}
	}
}

#if !defined (PROPER_FRONTEND)
extern bool ShowingStart;

void Screen::SetStartState(bool State)
{
	if (State)
	{
		StartButton->Draw();
		StartText->Write();
		ShowingStart = true;
	}
	else
	{
		StartButton->Hide();
		StartText->Hide();
		ShowingStart = false;
	}
}
#endif

extern int reallanguage;
#if defined(JAPAN)
extern int JapanHelp;
#endif

void Screen::SetHelpText(char *Text1, char *Text2)
{
	HelpText[0] = new TextWord(Text1, MainAlphabet);
	HelpText[1] = new TextWord(Text2, MainAlphabet);

	HelpText[0]->Justify(JUSTIFY_LEFT); HelpText[1]->Justify(JUSTIFY_LEFT);
	if (reallanguage > 1)
	{
		HelpText[0]->Write(10,417,1800.0f);
		HelpText[1]->Write(10,429,1800.0f);
	}
	else
	{
		HelpText[0]->Write(10,417,1200.0f);
		HelpText[1]->Write(10,434,1200.0f);
	}
}

void Screen::DeleteHelp()
{
	delete HelpText[0];
	delete HelpText[1];
#if defined(JAPAN)
	JapanHelp = -1;
#endif
}

void Screen::ChangeHelpLine(int Line, bool State)
{
	if (State)
		HelpText[Line]->Write();
	else
		HelpText[Line]->Hide();
}


void LoadBackdrop(BorisMaterial *BackdropVariable, char *BackdropName, bool MoreTextures)
{
	int i;
	textureData* backs[8];
	char Filename[80];
	char ArchiveName[256];

#if !defined(PROPER_FRONTEND)
	sprintf(ArchiveName, DISKNAME"\\SOS1937FrontEnd\\Backdrops.tc", BackdropName);
	arcPush(ArchiveName);
#else
	sprintf(ArchiveName, "c:\\work\\dakarfedemo\\FrontLibData\\%s.tc", BackdropName);
	arcPush(ArchiveName);
#endif

	if (MoreTextures)
	{
#if !defined (PROPER_FRONTEND)
		for (i='a' ; i<='d' ;  i++)
		{
			sprintf(Filename, "%s%c.3df", BackdropName, i);
			backs[i-'a']= getTexturePtr(Filename,false,false);
		}
		for (i=0 ; i<4 ; i++)
		{
			sprintf(Filename, "%s%d.3df", BackdropName, i+2);
			backs[i+4]= getTexturePtr(Filename,false,false);
		}
#else
		for (i='a' ; i<='f' ; i++)
		{
			sprintf(Filename, "%s_%c.3df", BackdropName, i);
			backs[i-'a'] = getTexturePtr(Filename,false, false);
		}
#endif
		for(i=0;i<LARGEVERTICES;++i)
		{
			BackdropVariable[i].textureHandle = backs[i]->textureHandle;

		}

	}
	else
	{
		for (i=0 ; i<5 ; i++)
		{
			sprintf(Filename, "%s%d.3df", BackdropName, i+1);
			backs[i]= getTexturePtr(Filename,false,false);
		}
		for(i=0;i<5;++i)
		{
			BackdropVariable[i].textureHandle = backs[i]->textureHandle;
			BackdropVariable[i].flags = 0;
		}
	}

	arcPop();
}