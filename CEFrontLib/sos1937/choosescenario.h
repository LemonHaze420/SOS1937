// $Header

// $Log

/* The options screen */

struct Scenario
{
	char *Name;
	char *Description;
	int GridPosition;
	char *Track;
	int Laps;
	int Length;
	char *LengthString;
	int Cars;
	int Car[10];
	int SuccessValue;
	char *SuccessString, *FailureString;
};

void LoadAllScenarios();

class ChooseScenarioScreen : public Screen
{
public:
	ChooseScenarioScreen(Alphabet *);
	virtual ~ChooseScenarioScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	// Load all the scenarios for this difficulty level.
	void LoadScenarios();

	void HighlightScenario(int ThisScenario);
	void LowlightScenario(int ThisScenario);
	void WriteDescription(int ThisScenario);

	int Counter, CurrentScenario;

	TextWord *ChooseScenarioText;
	TextWord *ScenarioName, *YellowWord;
	// Up to 10 lines.
	TextWord *ScenarioDetails[10];
	char ScenarioLines[10][60];
};