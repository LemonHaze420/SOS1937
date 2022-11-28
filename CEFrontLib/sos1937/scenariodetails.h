// $Header

// $Log

/* The options screen */

class ScenarioDetailsScreen : public Screen
{
public:
	ScenarioDetailsScreen(Alphabet *);
	virtual ~ScenarioDetailsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void LoadScenarioDetails();

	void DrawScenarioResult();
	void HideScenarioResult();

	void DrawObjective();
	void HideObjective();
	
	void DrawStartGrid();
	void HideStartGrid();

	void DrawTrackInfo();
	void HideTrackInfo();
	
	void DrawCarInfo();
	void HideCarInfo();
	
	void LoadMiniCars();

	int Counter;
	int CurrentSelection;

	FrontEndCar *CarObject;
	FrontEndTrack *TrackObject;

	char ScenarioLines[10][60];
	TextWord *ScenarioDetails[10], *ScenarioResult[5], *CarName[10];
	struct Scenario *ThisScenario;

	// The track
	FrontEndObject *TrackWord, *TrackDescription, *TrackStats;


	// The mini-cars.
	FrontEndObject *MiniCar[10];
};