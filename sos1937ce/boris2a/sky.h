//================================================================================================================================
// sky.h
// -----
//			- Deals with a sky.  A sky is a mulder model and will have attributes set accordingly.
//================================================================================================================================

#ifndef _BS_sky
#define _BS_sky

class sky
{

	public:
		sky( char * filename, float scale, vector3 * initialangle, vector3 * deltaangle, float updaterate );
		~sky();

		model  * skyModel;			

		timer  updateTimer;

		vector3 skyangle, skyangledelta;

		matrix3x4 RotMatrix ;

		void updateIfNeeded();

	private:
		void createRotationMatrix();

};

#endif	// _BS_sky

//================================================================================================================================
//END OF FILE
//================================================================================================================================
