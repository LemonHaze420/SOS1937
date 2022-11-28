/* The car object */

class FrontEndTrack
{
public:
	FrontEndTrack(int _Number);
	~FrontEndTrack();

	void Draw();
	void Hide();
	void Rotate(float x, float y, float z);

	void SetTrackPosition(int x, int y, float z);
	void SetNamePosition(int x, int y, float z);
	void SetGraphPosition(int x, int y, float z);

	int GetNumber();
private:
	int Number;

	FrontEndObject *TrackObject, *TrackName, *TrackDescription, *TrackGraph;
};