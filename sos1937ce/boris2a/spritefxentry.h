//================================================================================================================================
// spriteFXentry.h
// ---------------
//			list item for linking sprite effects (spriteFX) in a chain from the engine)
//================================================================================================================================

#ifndef _BS_spriteFXentry
#define _BS_spriteFXentry

class spriteFXentry{
	
	public:
		
		spriteFXentry(){};
		~spriteFXentry(){};

	//private:
			triangle triangleA, triangleB;

			timer   frameTimer;

		
			vector3 worldPosition;
			spriteFXframe *currentFrame ;
			spriteFX * spriteFXptr;
			slong currentCount;
			bool reverse;

				// structure data

			spriteFXentry * next;
			spriteFXentry * prev;

			ulong * externalcounter;			// Address of a counter which is decremented when the FXentry finishes
												// (Very useful in multiple FX sequences).

			bool flipU, flipV;
			bool firstFrame;
			bool useTimer;

							
			void setWorldPosition( vector3 * p );	
			void getWorldPosition( vector3 * p );

			// jcf
			vector3 mydrift;
};



#endif	// _BS_spriteFXentry

//================================================================================================================================
//END OF FILE
//================================================================================================================================
