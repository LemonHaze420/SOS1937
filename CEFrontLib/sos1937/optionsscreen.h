// $Header$

// $Log$
// Revision 1.6  2000-04-07 14:38:10+01  img
// Help support, generic background support
//
// Revision 1.5  2000-04-03 14:53:12+01  img
// New models
//
// Revision 1.4  2000-03-06 12:50:18+00  img
// Uses icons now.... New archive needed (SoundIcons.tc)
//
// Revision 1.3  2000-02-29 11:28:20+00  jjs
// Added Header and Log lines.
//

/* The options screen */

class OptionsScreen : public Screen
{
public:
	OptionsScreen(Alphabet *);
	virtual ~OptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void Highlight();
	void Lowlight();

	Slider *MusicSlider, *EffectsSlider, *EngineSlider;

	int CurrentObject;
	FrontEndObject *Objects[3];
	int Value[3],LastChangedVal[3];
	
};

