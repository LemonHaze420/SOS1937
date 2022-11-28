// $Header

// $Log

/* The options screen */

class StartQuickRaceScreen : public Screen
{
public:
	StartQuickRaceScreen(Alphabet *);
	virtual ~StartQuickRaceScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;
	
	TextWord *TextValues[7];
};