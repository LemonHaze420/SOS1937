// $Header$

// $Log$
// Revision 1.1  2000-02-29 08:44:45+00  jjs
// Added Header and Log lines
//

/* Main header file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		09/11/1999
	Rev:		0.01 */

// The constructor.
class ConsoleFrontLib
{
public:
	// The constructor.
	ConsoleFrontLib();

	// Start the front end.
	bool Start();

	// Pressed a key.
	void KeyPressed(int ControlPacket);


private:
	// The actual game's front end.
	Game *MainGame;

	camera *MainCamera;

	world *DummyWorld;
	model *TestModel, *TestModel2;
	object *TestObject, *TestObject2;
	DWORD BackgroundBitmap;

	//Load up the necessary models.
	void InitialiseModels();

	Alphabet *MainAlphabet;
};

extern int JapanHelp;
inline void SetJapanText(int type)
{
	JapanHelp = type;
}