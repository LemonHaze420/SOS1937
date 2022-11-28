// $Header

// $Log

/* The options screen */

class ChampionshipScreen : public Screen
{
public:
	ChampionshipScreen(Alphabet *);
	virtual ~ChampionshipScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void UpdateTable();

	void LoadScenario();
	void LoadMiniCars();

	void LoadStandingsTable();
	void DrawStandings();
	void HideStandings();

	void DrawCurrentScreen();
	void HideCurrentScreen();
	
	void DrawScenarioDetails();
	void HideScenarioDetails();

	void DrawStartGrid();
	void HideStartGrid();

	void LoadCar();
	void LoadTrack();

	int Counter;
	int CurrentSelection;

	// Set to true when requesting confirmation of leaving championship.
	bool Confirming;

	FrontEndCar *CarObject;
	FrontEndTrack *TrackObject;

	struct Scenario *CurrentScenario;
	TextWord *ScenarioDetails[10];
	char ScenarioLines[10][60];
	
	FrontEndObject *MiniCar[10];
	FrontEndObject *CarInTable[10];

	TextWord *PositionWord[10], *CarNumber[10], *CarPoints[10], *CarType[10], *DriverName[10];
	TextWord *CarName[10];
};