// $Header$

// $Log$

// Slider class.

class Slider
{
public:
	Slider();
	~Slider();

	void Draw(int x, int y, float z=600.0f);
	void SetPosition(int Value);
private:
	FrontEndObject *SlideBar, *SliderKnob, *Numeral[10], *Percent;
	FrontEndObject *Value[3];

};