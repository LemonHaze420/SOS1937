// $Header

// $Log

class CycledBox
{
public:
	CycledBox(int _InitialColour, int _FinalColour, int _StartCycle, int _EndCycle);

	// t is the parameter 0<=t<=1
	int CurrentColour(float t);
private:
	// The main colour of the box.
	int InitialColour;

	// The Final Colour to be cycled to.
	int FinalColour;

	// The start times for cycling each colour (ARGB)
	int StartCycle;
	// And the finish times...
	int EndCycle;
};
