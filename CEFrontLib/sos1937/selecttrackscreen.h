// $Header$

// $Log$
// Revision 1.4  2000-03-20 18:01:01+00  img
// Beta build
//
// Revision 1.3  2000-02-29 11:28:27+00  jjs
// Added Header and Log lines.
//

/* The track selection screen of the game.

	Ian Gledhill 06/01/1900 :-)
	Broadsword Interactive Ltd. */

class SelectTrackScreen : public Screen
{
public:
	SelectTrackScreen(Alphabet *);
	virtual ~SelectTrackScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void DestroyModel();
	void LoadModel();
	
	FrontEndCar *CarObject;
	FrontEndTrack *TrackObject;

	// The current track selected.
	int CurrentTrack, MaxTrack;
};