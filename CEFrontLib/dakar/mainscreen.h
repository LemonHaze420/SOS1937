// $Header

// $Log

class MainScreen : public Screen
{
public:
	MainScreen(Alphabet *);
	virtual ~MainScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;
	
	Alphabet *MainAlphabet;
};