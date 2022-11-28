// $Header

// $Log

#ifndef __MEMORYCARDCHOSENSCREEN_H__
#define __MEMORYCARDCHOSENSCREEN_H__

#define MCCSM_CHOOSING		0
#define MCCSM_SAVING		1
#define MCCSM_LOADING		2
#define MCCSM_CONFIRMING	3

class MemoryCardChosenScreen : public Screen
{
public:
	MemoryCardChosenScreen(Alphabet *, int _ChosenCard);
	virtual ~MemoryCardChosenScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void DeriveLocation();
	void LoadFilenames();

	void SetDefaultHelp();

	void Lowlight();
	void Highlight();

	int Mode;

	int Counter;
	int ChosenCard;
	int CurrentSelection;

	int NextCommand;
	bool SlotUsed[4];
	bool SlotEmpty[4];
	bool WarningFlagged;

	FrontEndObject *MemoryCard;

	TextWord *CardPosition, *SlotContents[4], *YellowContents[4];
};

#endif