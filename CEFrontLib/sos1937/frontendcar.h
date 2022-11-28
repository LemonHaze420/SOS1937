/* The car object */

class FrontEndCar
{
public:
	FrontEndCar(int _Number);
	~FrontEndCar();

	void Draw(bool ModelOnly = false);
	void Hide();
	void Rotate(float x, float y, float z);

	void SetCarPosition(int x, int y, float z);
	void SetNamePosition(int x, int y, float z);
	void SetGraphPosition(int x, int y, float z);

	int GetNumber();
private:
	void GetLevel1Model(char *Name, char *Model);
	
	int Number;

	// The car positions and names.
	vector3 WheelPosition[4];
	char BodyName[32], FrontAxleName[32], RearAxleName[32], FrontWheelName[32], RearWheelName[32];

	FrontEndObject *CarObject, *FrontAxle, *RearAxle, *Wheel[4], *CarName, *CarGraph, *CarNote;


};