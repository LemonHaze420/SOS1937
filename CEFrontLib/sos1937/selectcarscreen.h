// $Header$

// $Log$
// Revision 1.9  2000-03-30 14:48:27+01  img
// More preloadings
//
// Revision 1.8  2000-03-20 18:00:59+00  img
// Beta build
//
// Revision 1.7  2000-03-09 11:40:17+00  img
// Now with arrows!
//
// Revision 1.6  2000-02-29 11:28:26+00  jjs
// Added Header and Log lines.
//

/* The select car screen */

class SelectCarScreen : public Screen
{
public:
	SelectCarScreen(Alphabet *, int _CurrentPlayer=0);
	virtual ~SelectCarScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	// Destroy the current model.
	void DestroyModel();

	// Load a car model.
	void LoadNewModel();

	// Draw the current gearbox mode.
	void DrawGearboxMode();

	// The screen title.
	TextWord *Title;
	
	// The current car selected.
	int CurrentCar, MaxCar, CurrentPlayer;

	FrontEndCar *CarModel;
	
	TextWord *GearBoxWord[2];
};