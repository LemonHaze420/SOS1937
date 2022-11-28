// $Header

// $Log

/* The options screen */

class ChampionshipOverScreen : public Screen
{
public:
	ChampionshipOverScreen(Alphabet *);
	virtual ~ChampionshipOverScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void UpdateTable();
	void DrawResult();


	struct Scenario *CurrentScenario;
	TextWord *ScenarioResult[5];

	int Counter;

};