// $Header$

// $Log$

#ifndef _BS_carbody
#define _BS_carbody

class carbody {
	
	public:	
		//carbody(){};
		~carbody(){};

		carbody( object * whichObject );


	//private:
		vector3 exhaustCloudPoint;				// body-relative co-ordinate of exahust cloud
		vector3 transformedexhaustCloudPoint;

		object * bodyObject;


		vector3 enginePosition;		// offset of engine centre in body

		bool getWorldHeight( float * yvalue, triangle ** resulttriPtr);

		void transformDirectionVectorToWorld( vector3 * r, vector3 * v );	
		void transformPositionVectorToWorld( vector3 * r, vector3 * v );

		vector3 * getPositionInWorld();		// get position of body centroid in world co-ordinates


};

#endif	// _BS_carbody
