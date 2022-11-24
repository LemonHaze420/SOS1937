// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:29:18+00  jjs
// First version that supports the loading of objects.
//

#ifndef _BS_car
#define _BS_car

#define MAXWHEELS 4

class car {

public:

	~car();
	car( char * arcname, float carscale, /*spriteFX * exhaustFX,*/ int _id );

	void LoadModel(char * arcname, float carscale);
	void readOff( char * outStr, char ** strPtr, char stopChar );

	model * FBlurMedModel,* FBlurHiModel;
	model * RBlurMedModel,* RBlurHiModel;
	model * usualWModel[4];

	object * mainObject;

	object * bodyObject, *driverObject, *FwheelObjectLEFT, * FwheelObjectRIGHT, 
		   * RwheelObjectLEFT,  * RwheelObjectRIGHT, *FaxleObject, *RaxleObject,
		   * bodyShadowObject, * driverShadowObject, * FwheelShadowObjectLEFT, * FwheelShadowObjectRIGHT,
		   * RwheelShadowObjectLEFT, * RwheelShadowObjectRIGHT, * FaxleShadowObject, * RaxleShadowObject;


	carbody    * body;

	carwheel   * wheel[ MAXWHEELS ];			// up to 4 wheels

	ulong      totalWheels;						// depends on type of car, either 3 or 4

	void readOffVector( vector3 * v, char ** strPtr );

	void setBodyPosition( vector3 * p );
};

#endif