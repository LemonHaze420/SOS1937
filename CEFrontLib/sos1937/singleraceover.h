// $Header

// $Log

/* The options screen */

class SingleRaceOverScreen : public Screen
{
public:
	SingleRaceOverScreen(Alphabet *);
	virtual ~SingleRaceOverScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void LoadStandingsTable();
	void DrawStandings();
		
	int Counter;

	TextWord *RaceOver;

	FrontEndObject *CarInTable[10];
	TextWord *PositionWord[10], *CarNumber[10], *CarType[10], *DriverName[10];
};