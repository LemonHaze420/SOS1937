// $Header

// $Log

class ChooseClassScreen : public Screen
{
public:
	ChooseClassScreen(Alphabet *);
	virtual ~ChooseClassScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;
	
	Alphabet *MainAlphabet;
};