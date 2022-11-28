// $Header

// $Log

class TrackDetailsScreen : public Screen
{
public:
	TrackDetailsScreen(Alphabet *);
	virtual ~TrackDetailsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;
	
	Alphabet *MainAlphabet;
};