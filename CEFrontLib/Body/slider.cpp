// $Header$

// $Log$
// Revision 1.3  2000-03-06 13:03:12+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.2  2000-02-29 08:43:37+00  jjs
// Added Header & Log lines.
//

// Slider class.

#include "ConsoleFrontLib.h"

static bool ModelLoaded;

Slider::Slider()
{
//	arcPush(DISKNAME"\\SOS1937FrontEnd\\Sliders.tc");
	SlideBar = new FrontEndObject(NULL, "SlideBar",1.0f,NULL, false);
	SliderKnob = new FrontEndObject(NULL, "Knob",1.0f,SlideBar, true);
//	Percent = new FrontEndObject(NULL, "Percentage",1.0f,NULL, false);
//	arcPop();

	SliderKnob->MoveInModelSpace(&vector3(0.0f,0.0f,-0.01f));

	ModelLoaded = true;
}

void Slider::Draw(int x, int y, float z)
{
	SlideBar->Draw(x,y,z);
}

void Slider::SetPosition(int Value)
{
	vector3 Low, High;

	SlideBar->GetDimensions(&Low, &High);

	float NewX = (High.x - Low.x) * 0.5f - High.x;
	float Step = (High.x - Low.x) / 100.0f;
	NewX += Step * Value + (High.x - Low.x) * -0.5f;
	NewX *= 0.85f;

	SliderKnob->MoveInModelSpace(&vector3(NewX, 0.0f,-0.01f));
}

Slider::~Slider()
{
	if (ModelLoaded)
	{
//		SliderKnob->DeleteModel();
//		SlideBar->DeleteModel();
//		Percent->DeleteModel();
		ModelLoaded = false;
	}

	delete SliderKnob;
	delete SlideBar;
//	delete Percent;
}