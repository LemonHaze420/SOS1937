// $Header$

// $Log$
// Revision 1.15  2000-05-04 17:39:29+01  img
// Build 18
//
// Revision 1.14  2000-04-24 20:34:20+01  jcf
// Build 12
//
// Revision 1.13  2000-04-22 18:26:07+01  img
// Added C button to joystick support
//
// Revision 1.12  2000-04-21 19:06:02+01  img
// Bug fixes
//
// Revision 1.11  2000-04-19 21:38:03+01  img
// Localisation.
//
// Revision 1.10  2000-04-17 14:53:01+01  img
// Bug fixes
//
// Revision 1.9  2000-04-07 14:38:10+01  img
// Help support, generic background support
//
// Revision 1.8  2000-04-03 14:53:11+01  img
// New models
//
// Revision 1.7  2000-03-06 13:03:14+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.6  2000-03-06 12:50:17+00  img
// Uses icons now.... New archive needed (SoundIcons.tc)
//
// Revision 1.5  2000-02-29 11:28:19+00  jjs
// Added Header and Log lines.
//

/* The options screen of the game.

	Ian Gledhill 20/01/2000 :-)
	Broadsword Interactive Ltd. */

#include "ConsoleFrontLib.h"

extern class ButtonBank *MainMenu;
extern bool PreventLeftRightRepetition;

#include "carsound.h"


OptionsScreen::OptionsScreen(Alphabet *DefaultAlphabet)
{
	LastChangedVal[0] = LastChangedVal[1] = LastChangedVal[2] = -100;

	// Load in the sliders.
	MusicSlider = new Slider();
	MusicSlider->Draw(420,180,200.0f);	

	EngineSlider = new Slider();
	EngineSlider->Draw(420,230,200.0f);

	EffectsSlider = new Slider();
	EffectsSlider->Draw(420,280,200.0f);

	Value[SO_MUSIC] = MainGameInfo.MusicVolume;
	Value[SO_ENGINE] = MainGameInfo.EngineVolume;
	Value[SO_EFFECTS] = MainGameInfo.EffectsVolume;

	MusicSlider->SetPosition(Value[0]);
	EngineSlider->SetPosition(Value[1]);
	EffectsSlider->SetPosition(Value[2]);

	PreventLeftRightRepetition = false;

	SoundObjects[0][SO_MUSIC]->Draw(200,180,600.0f);
	SoundObjects[0][SO_ENGINE]->Draw(200,230,600.0f);
	SoundObjects[0][SO_EFFECTS]->Draw(200,280,600.0f);
	SoundObjects[1][SO_MUSIC]->Draw(200,180,600.0f);
	SoundObjects[1][SO_ENGINE]->Draw(200,230,600.0f);
	SoundObjects[1][SO_EFFECTS]->Draw(200,280,600.0f);
	
	for (int i=SO_MUSIC ; i<=SO_EFFECTS ; i++)
	{
		SoundObjects[1][i]->Hide();
	}

	CurrentObject = SO_MUSIC;

	Objects[SO_MUSIC] = SoundObjects[0][SO_MUSIC];
	Objects[SO_ENGINE] = SoundObjects[0][SO_ENGINE];
	Objects[SO_EFFECTS] = SoundObjects[0][SO_EFFECTS];
	Highlight();

	MainAlphabet = DefaultAlphabet;

	ScreenHeaders[HEADER_RACEOPTIONS]->Draw();

	SetHelpText(TranslateLocale(59),TranslateLocale(60));

	// Reset the counter to zero.
	Counter = 0;

	// Use the default background.
	UseDefaultBackdrop = true;
}

void OptionsScreen::Update(struct Instruction *ScreenCommand)
{
	if (Counter > -1)
	{
		Objects[CurrentObject]->Rotate((float)cos(Counter*0.01f)*14.0f,(float)sin(Counter*0.013f)*13.0f,0.0f);
		Counter++;
	}

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

void OptionsScreen::Lowlight()
{
	Objects[CurrentObject]->Hide();
	Objects[CurrentObject] = SoundObjects[0][CurrentObject];
	Objects[CurrentObject]->Draw();
}

void OptionsScreen::Highlight()
{
	Objects[CurrentObject]->Hide();
	Objects[CurrentObject] = SoundObjects[1][CurrentObject];
	Objects[CurrentObject]->Draw();
}

OptionsScreen::~OptionsScreen()
{
}

int OptionsScreen::ControlPressed(int ControlPacket)
{
	extern void PlayFrontEndSound(int);
	extern void SetMusicVol();

	if (ControlPacket & 1 << CON_THROTTLE)
	{
		Lowlight();
		if (--CurrentObject < 0) CurrentObject = SO_EFFECTS;
		Objects[CurrentObject]->Rotate();
		Highlight();
		Counter=0;
	}

	if (ControlPacket & 1 << CON_BRAKE)
	{
		Lowlight();
		if (++CurrentObject > SO_EFFECTS) CurrentObject = 0;
		Objects[CurrentObject]->Rotate();
		Highlight();
		Counter=0;
	}

	if (ControlPacket & 1 << CON_RIGHT)
	{
		if (CurrentObject == SO_MUSIC)
		{
			static int HacketyHack;
			HacketyHack=Value[0];
			HacketyHack++;
			if (HacketyHack > 100) HacketyHack = 100;
//			dprintf("Value 2a: %d", HacketyHack);
			MusicSlider->SetPosition(HacketyHack);
//			dprintf("Value 2b: %d", HacketyHack);
			Value[0] = HacketyHack;
		}
		else if (CurrentObject == SO_ENGINE)
		{
			static int HacketyHack;
			HacketyHack=Value[1];
			HacketyHack++;
			if (HacketyHack > 100) HacketyHack = 100;
//			dprintf("Value 2a: %d", HacketyHack);
			EngineSlider->SetPosition(HacketyHack);
//			dprintf("Value 2b: %d", HacketyHack);
			Value[1] = HacketyHack;
		}
		else if (CurrentObject == SO_EFFECTS)
		{
			static int HacketyHack;
			HacketyHack=Value[2];
			HacketyHack++;
			if (HacketyHack > 100) HacketyHack = 100;
//			dprintf("Value 2c: %d", HacketyHack);
			EffectsSlider->SetPosition(HacketyHack);
//			dprintf("Value 2d: %d", HacketyHack);
			Value[2] = HacketyHack;
/*
			Value[2]++;
			if (Value[2] > 100) Value[2] = 100;
			EffectsSlider->SetPosition(Value[2]);
*/		}

	}

	if (ControlPacket & 1 << CON_LEFT )
	{
		if (CurrentObject == SO_MUSIC)
		{
			static int HacketyHack;
			HacketyHack=Value[0];
			HacketyHack--;
			if (HacketyHack < 0) HacketyHack = 0;
//			dprintf("Value 2a: %d", HacketyHack);
			MusicSlider->SetPosition(HacketyHack);
//			dprintf("Value 2b: %d", HacketyHack);
			Value[0] = HacketyHack;
		}
		else if (CurrentObject == SO_ENGINE)
		{
			static int HacketyHack;
			HacketyHack=Value[1];
			HacketyHack--;
			if (HacketyHack < 0) HacketyHack = 0;
//			dprintf("Value 2a: %d", HacketyHack);
			EngineSlider->SetPosition(HacketyHack);
//			dprintf("Value 2b: %d", HacketyHack);
			Value[1] = HacketyHack;
		}
		else if (CurrentObject == SO_EFFECTS)
		{
			static int HacketyHack;
			HacketyHack = Value[2];
			HacketyHack--;
			if (HacketyHack < 0) HacketyHack = 0;
//			dprintf("Value 2a: %d", HacketyHack);
			EffectsSlider->SetPosition(HacketyHack);
//			dprintf("Value 2b: %d", HacketyHack);
			Value[2] = HacketyHack;
		}
	}

	int qqq = Value[CurrentObject] - LastChangedVal[CurrentObject];
	if(qqq<0)qqq= -qqq;

	if(qqq>10)
	{
		LastChangedVal[CurrentObject] = Value[CurrentObject];
		int temp = MainGameInfo.MusicVolume;


		switch(CurrentObject)
		{
			extern void SetMusicVol();

			case SO_MUSIC:
				MainGameInfo.MusicVolume = Value[SO_MUSIC];
				SetMusicVol();
				break;
			default:
				carsounds.SetEffectsVolume(Value[CurrentObject]);
				PlayFrontEndSound(2);
		}

		MainGameInfo.MusicVolume = temp;
	}

	if (ControlPacket & 1 << CON_A || ControlPacket & 1 << CON_B || ControlPacket & 1 << CON_C || ControlPacket & 1 << CON_START)
	{
		if (ControlPacket & 1 << CON_A)
		{
			MainGameInfo.EffectsVolume = Value[SO_EFFECTS];
			MainGameInfo.EngineVolume = Value[SO_ENGINE];
			MainGameInfo.MusicVolume = Value[SO_MUSIC];
		}
		SetMusicVol();
		carsounds.SetEffectsVolume(MainGameInfo.EffectsVolume);
		carsounds.SetEngineVolume(MainGameInfo.EngineVolume);

		MainMenu->Unselect(SOUND);
		
		Destroy();
	
		// Change screen.
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new FrontScreen(MainAlphabet);

		((FrontScreen *)QueuedValue)->SetHighlighted(SOUND);
	}

	return 0;
}

void OptionsScreen::DrawBackground()
{
}

void OptionsScreen::Destroy()
{
	Lowlight();

	delete MusicSlider;
	delete EffectsSlider;
	delete EngineSlider;

	for (int i=0 ; i<3 ; i++)
	{
		Objects[i]->Hide();
	}

	Counter = -1;
	PreventLeftRightRepetition = true;

	ScreenHeaders[HEADER_RACEOPTIONS]->Hide();

	DeleteHelp();

	return;
}