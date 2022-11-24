// $Header$

// $Log$

#ifndef _BS_carwheel
#define _BS_carwheel

class carwheel {
	
	friend class car;

	public:	
		//carwheel(){};
		~carwheel(){};
		carwheel( object * whichObject );


//	private:

		object * wheelObject;			

		bool  attached;					// true = wheel is attached to car body.
										// false = wheel is detached from car body.

		float radius;					// radius of wheel - important to know how far an angular turn about the axis
										// will cause travel
		float oneoverradius;

		float slipRadiusSquared;		// radius squared of accel./Turning accel. circle ... VERY IMPORTANT FOR CALCULATING 
										// when wheel grip is lost.


		float massAboveWheel;			// 
										//

		bool getWorldHeight( float * yvalue, triangle ** resulttriPtr );		// get height of world centroid XZ ( return TRUE iff on-world )

		void transformDirectionVectorToWorld( vector3 * r, vector3 * v );	
		void transformDirectionVectorToBody( vector3 * r, vector3 * v );		
		void transformPositionVectorToWorld( vector3 * r, vector3 * v );
		void transformPositionVectorToBody( vector3 * r, vector3 * v );

		vector3 * getPositionInWorld();		// get position of wheel centroid in world co-ordinates

		vector3 pos;	// starting position relative to car - i.e. the neutral position without
						// suspension effects

		class car *c;


};

#endif	// _BS_carwheel
