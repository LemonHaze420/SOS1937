// $Header$

// $Log$
// Revision 1.11  2000-04-11 16:12:04+01  img
// New icons on the screen
//
// Revision 1.10  2000-04-07 15:58:10+01  img
// Controller loadr moved to SOSIndex.cpp
//
// Revision 1.9  2000-04-07 15:29:12+01  img
// Better management of frontscreen's memory card
//
// Revision 1.8  2000-04-07 14:38:08+01  img
// Help support, generic background support
//
// Revision 1.7  2000-03-20 18:00:55+00  img
// Beta build
//
// Revision 1.6  2000-02-29 11:28:18+00  jjs
// Added Header and Log lines.
//

/* The front screen */

#define RACE_TYPE	1
#define CAR			2
#define TRACK		3
#define SOUND		4
#define CONTROL		5
#define PLAYER		6
#define	CONTINUE	7

#define SR_PRACTICE	1
#define SR_QUALIFY	2
#define SR_RACE		3

class FrontScreen : public Screen
{
public:
	FrontScreen(Alphabet *DefaultAlphabet);
	virtual ~FrontScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

	virtual void SetHighlighted(int _Highlighted);

private:
	// Load the car. This should be done on creation of the screen.
	void LoadCar();
	// Now draw it. Only done on the CAR button.
	void DrawCar();
	// Or hide it when the CAR button is left.
	void HideCar();
	// Rotate the car when needed.
	void RotateCar();

	// Load the track
	void LoadTrack();
	// Draw the track.
	void DrawTrack();
	// or hide it again.
	void HideTrack();
	// and rotate if necessary.
	void RotateTrack();

	// Load in the current race type text.
	void LoadRaceTypeText();

	void SetDefaultHelp();
	void SetUnavailableHelp();

	int Highlighted;

	FrontEndCar *CarObject;
	FrontEndTrack *TrackObject;
	FrontEndObject *RaceTypeIcon;

	TextWord *RaceTypeText, *EnterNameText;
};

