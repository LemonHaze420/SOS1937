//================================================================================================================================
// display.h
// --------
//
//================================================================================================================================

#ifndef _BS_display
#define _BS_display

class display
{
	friend class engine;

	public:
			display( slong pixw, slong pixh, slong posx, slong posy);
			~display();


//	private:
			slong pixelwidth;
			slong pixelheight;
			slong deltaX;
			slong deltaY;
			bool mirrorx;
			int mirrorVal;

		//	slong positionx, positiony;
};

#endif	// _BS_display

//================================================================================================================================
//END OF FILE
//================================================================================================================================
