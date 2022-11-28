//================================================================================================================================
// display.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"


display::display(slong pixw, slong pixh,  slong posx, slong posy )
{
	if(pixw<0)
	{
		mirrorx = true;
		mirrorVal = posx + posx - pixw;
		pixelwidth  = -pixw;
	}
	else
	{
		mirrorx = false;
		pixelwidth  = pixw;
	}

	pixelheight = pixh;
	deltaX     = posx;
	deltaY     = posy;
}

display::~display()
{
	if( boris2->currentdisplay == this)
		boris2->currentdisplay = NULL;
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
