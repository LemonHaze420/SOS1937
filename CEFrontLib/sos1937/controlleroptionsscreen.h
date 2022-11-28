// $Header$

// $Log$
// Revision 1.11  2000-05-16 16:13:23+01  img
// FINAL release build???
//
// Revision 1.10  2000-04-11 16:12:15+01  img
// Rumble pack support
//
// Revision 1.9  2000-04-07 14:38:06+01  img
// Help support, generic background support
//
// Revision 1.8  2000-03-21 17:28:26+00  img
// Pre-loading more now...
//
// Revision 1.7  2000-03-20 18:00:52+00  img
// Beta build
//
// Revision 1.6  2000-03-10 16:08:52+00  img
// Uses steering wheel properly (has toggle on Status)
//
// Revision 1.5  2000-03-03 15:25:25+00  img
// Support for steering wheel
//
// Revision 1.4  2000-03-02 14:26:37+00  img
// Controller screen now working
//
// Revision 1.3  2000-02-29 11:28:16+00  jjs
// Added Header and Log lines.
//

/* The options screen */

#define FEObjectWord(x) TextWord * ## x ## White, * ## x ## Yellow;

class ControllerOptionsScreen : public Screen
{
public:
	ControllerOptionsScreen(Alphabet *);
	virtual ~ControllerOptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void ConstructControllerTable(int ControllerType);

	void HighlightWord(int Word);
	void LowlightWord(int Word);

	// Initialise the rotations for each controller.
	void InitialiseRotations();

	// Rotate the controller
	void RotateController(int ControllerType, int Button);

	// Highlight part of the model.
	void HighlightModel(int ModelNumber);
	void LowlightModel(int ModelNumber);

	// Remove the selected word for a button for a given function
	void RemoveButtonText(int Function);

	// Assign a button to a function according to the button pressed.
	void AssignButton(int Function, int Button);
	
	// Assign a button to a function according to the control packet.
	void SetNewButton(int Function, int ControlPacket);

	// Draw the text to say if status is on or off.
	void DrawStatusToggle();

	// Display normal help text or alternative text.
	void SetBasicHelp();
	void SetDefineHelp();

	// Usually set to CCT_DREAMPAD.
	int ControllerType;

	// What the buttons were set to on entering the screen.
	int OldButtons[GS_RUMBLE+1];

	// The player whose controller we're defining.
	int CurrentPlayer;

	FrontEndObject *ControllerModel;

	FrontEndObject *LowModel[13], *HighModel[13];

	TextWord *OKWhite, *OKYellow;
	TextWord *LowlightedWord[14], *HighlightedWord[14];
	TextWord *LowButtonWord[14], *HighButtonWord[14];
	TextWord *OnButtonWhite, *OnButtonYellow, *OffButtonWhite, *OffButtonYellow;

	vector3 RotationPosition[3][14];
	vector3 *CurrentRotation;

	// The currently highlighted word.
	int SelectedWord;

	// The start, current position
	vector3 StartRotation;
	int TransitionPosition;

	bool WaitingForButton;
	bool VibrationPossible;
};
