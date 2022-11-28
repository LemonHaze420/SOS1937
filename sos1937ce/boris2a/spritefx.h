//================================================================================================================================
// spriteFX.h
// ----------
//			For dealing with sprite effects  (sparks, explosions, etc.)
//================================================================================================================================

#ifndef _BS_spriteFX
#define _BS_spriteFX

class spriteFXframe{
	
	public:
			spriteFXframe(  vector3 * pos,
							float xsize, float ysize,				
							float ucoord, float vcoord,
							float usize, float vsize
						 );

			~spriteFXframe();

			
			float xlength, ylength;

			nongeodata nv;
			float ulength, vlength;

			
			float xvariation, yvariation, hvariation, wvariation;

			spriteFXframe * next;
			spriteFXframe * prev;

			vector3 offset;

			material mat;
	//private:


};



//	worldPosition = *position;
//	currentFrame = firstFrame;
//	currentCount = repeatCount;
//	reverse = false;


class spriteFX{

	public:

			//spriteFX( char * filename );
			spriteFX(  char * filename); //, float zcastback );
			~spriteFX();

			
		
	//private:
	



		float framesPerSecond;		// Number of frames per second for this effect 

		spriteFXframe * firstFrame, * lastFrame;

		slong repeatCount;			// huge number = forever

		bool  oscillate;				// true iff back/forward direction change required
		bool  ydrift;				// true if sprite drifts heavenwards (like smoke)
		bool  engydrift;

		float zbufferDelta;					// amount for pulling the zbuffering z back by (useful for smoke etc.)

		void appendFrame( float pixelsX, float pixelsY, vector3 * pos,
						  float xsize, float ysize, 
						  float ucoord, float vcoord,
 						  float usize, float vsize,
						  float xvar, float yvar, float wvar, float hvar, textureData * tdptr);

};


void setspriteFXpath( char * filename );


#endif	// _BS_spriteFX

//================================================================================================================================
//END OF FILE
//================================================================================================================================
