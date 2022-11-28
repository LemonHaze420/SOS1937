// $Header$

// $Log$
// Revision 1.4  2000-04-03 14:53:02+01  img
// Only one player.
//
// Revision 1.3  2000-03-30 14:48:23+01  img
// More preloadings
//
// Revision 1.2  2000-03-07 16:02:11+00  img
// Grid appears "over" the Player screen
//
// Revision 1.1  2000-03-02 09:49:55+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.0  2000-03-01 13:09:04+00  img
// Initial revision
//
// Revision 1.3  2000-02-29 11:28:24+00  jjs
// Added Header and Log lines.
//

/* The options screen */

class EnterPlayerNameScreen : public Screen
{
public:
	EnterPlayerNameScreen(Alphabet *, int _PlayerNumber);
	virtual ~EnterPlayerNameScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void DrawGrid();
	void DrawCursor();
	void DrawName();

	void AddLetter(char Letter);
	void Delete();

	char OldName[32];
	
	int Counter;
	int PlayerNumber, CurrentLetter;

	FrontEndObject *End, *Del;

	TextWord *Letter[40];
	TextWord *EnterName;
	TextWord *PlayerNameWord;
};