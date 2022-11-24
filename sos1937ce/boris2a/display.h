// $Header$

// $Log$

#ifndef _BS_display
#define _BS_display

class display
{
	friend class engine;

	public:
			display( long pixw, long pixh, long posx, long posy);
			~display();


//	private:
			long pixelwidth;
			long pixelheight;
			long deltaX;
			long deltaY;
			bool mirrorx;
			int mirrorVal;

};

#endif	// _BS_display
