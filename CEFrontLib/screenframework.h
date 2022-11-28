// $Header

// $Log

class DefaultScreen : public Screen
{
public:
	DefaultScreen(Alphabet *);
	virtual ~DefaultScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;
	
	Alphabet *MainAlphabet;
};