// $Header$

// $Log$
// Revision 1.3  2000-04-05 17:46:32+01  img
// Start grid
//
// Revision 1.2  2000-02-29 11:28:29+00  jjs
// Added Header and Log lines.
//

/* The Single Race screen */

class SingleRaceScreen : public Screen
{
public:
	SingleRaceScreen(Alphabet *);
	virtual ~SingleRaceScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void LoadMiniCars();
	void DrawStartGrid();
	void HideStartGrid();

	FrontEndObject *MiniCar[10];
	TextWord *CarName[10];

	int Highlighted;
};