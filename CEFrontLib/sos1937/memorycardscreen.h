/* The Memory card screen */

class MemoryCardScreen : public Screen
{
public:
	MemoryCardScreen(Alphabet *);
	virtual ~MemoryCardScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void AddNewCards();
	void RotateCards();

	void SetDefaultHelp();
	
	FrontEndObject *MemoryCardOne, *MemoryCardTwo, *MemoryCardBlack;
	int PreviousButton, CurrentCard;
	int RotationOfCard[8];

	TextWord *Labels[8];

	HANDLE MCS_hNewDeviceEvent, MCS_hRemovedDeviceEvent;
};