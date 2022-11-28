// $Header$

// $Log$
// Revision 1.7  2000-03-20 18:00:57+00  img
// Beta build
//
// Revision 1.6  2000-03-09 11:40:30+00  img
// Now with arrows!
//
// Revision 1.5  2000-03-08 10:31:51+00  img
// Difficulty Level on this screen now
//
// Revision 1.4  2000-03-02 14:26:39+00  img
// Controller screen now working
//
// Revision 1.3  2000-02-29 11:28:24+00  jjs
// Added Header and Log lines.
//

/* The options screen */

class RaceTypeScreen : public Screen
{
public:
	RaceTypeScreen(Alphabet *);
	virtual ~RaceTypeScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:

	// Show or hide the difficulty level.
	void ShowDifficulty();
	void HideDifficulty();

	void DrawRaceTypeIcon();

	FrontEndObject *RaceTypeIcon;
	FrontEndObject *Championship, *HeadToHead, *Network, *Quick, *Scenario, *Single;
	TextWord *Difficulty[PDS_HARD+1], *RaceTypeArray[6];
	int CurrentRaceType;

	int DifficultyLevel;
};