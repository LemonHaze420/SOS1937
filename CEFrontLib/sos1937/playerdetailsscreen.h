// $Header$

// $Log$
// Revision 1.6  2000-03-07 16:02:25+00  img
// Removed gearbox etc, added OK.
//
// Revision 1.5  2000-03-02 16:43:54+00  img
// Preliminary steering wheel support
//
// Revision 1.4  2000-03-02 09:49:56+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.3  2000-03-01 12:36:01+00  img
// Now can choose difficulty level.
//
// Revision 1.2  2000-02-29 11:28:21+00  jjs
// Added Header and Log lines.
//

/* The player details screen */

class PlayerDetailsScreen : public Screen
{
public:
	PlayerDetailsScreen(Alphabet *);
	virtual ~PlayerDetailsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void LowlightSelection();
	void HighlightSelection();

	int CurrentSelection;
	
	Alphabet *MainAlphabet;

	TextWord *PlayerNameTitle;

	TextWord *PlayerName[2], *PlayerNameYellow[2], *OKButton, *OKYellowButton;
};