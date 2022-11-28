/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\carwheel.cpp,v 1.7 1999/08/31 11:26:45 jcf Exp jcf $
	$Log: carwheel.cpp,v $
	Revision 1.7  1999/08/31 11:26:45  jcf
	First check in for ages :)

	Revision 1.6  1999/02/12 16:22:25  jcf
	First attempt at spin code, too complex. Will work on it more later.

	Revision 1.2  1999/01/12 11:45:30  jcf
	This version seems to work

*/

static const char *rcs_id="$Id: carwheel.cpp,v 1.7 1999/08/31 11:26:45 jcf Exp jcf $";

//================================================================================================================================
// carwheel.cpp
// ------------
//
//================================================================================================================================

#include "ALLcar.h"

extern engine * boris2;

carwheel::carwheel( object * whichObject )
{
	wheelObject = whichObject;
}



//----------------------------------------------------------------------------------------------------------------------
//  retrieve the 'y' height at the car wheel centroid xz. If this xz is ON WORLD the method returns 'true' else 'false'
//----------------------------------------------------------------------------------------------------------------------

bool carwheel::getWorldHeight( float * yvalue, triangle ** resulttriPtr )
{
	vector3 tpos;

	
	// modified to use the wheel attachment point

	c->mainObject->transform.multiplyV3By3x4mat(&tpos,&pos);
	return( boris2->currentworld->getY( &tpos, yvalue, resulttriPtr, false ) );		
	

//	return( boris2->currentworld->getY( &wheelObject->transformedCentroid, yvalue, resulttriPtr, false ) );		
};		

//----------------------------------------------------------------------------------------------------------------------
// transform local space DIRECTION vector to world  ( use 3x3 submatrix because positional translation is not required )
//----------------------------------------------------------------------------------------------------------------------

void carwheel::transformDirectionVectorToWorld( vector3 * r, vector3 * v )
{
	wheelObject->concatenatedTransform.multiplyV3By3x3mat( r, v ); 
};	

//----------------------------------------------------------------------------------------------------------------------
// transform local space DIRECTION vector to body  ( use 3x3 submatrix because positional translation is not required )
//----------------------------------------------------------------------------------------------------------------------

void carwheel::transformDirectionVectorToBody( vector3 * r, vector3 * v )
{
	wheelObject->transform.multiplyV3By3x3mat( r, v );				//
};	

//----------------------------------------------------------------------------------------------------------------------
// transform local space POSITION vector to world  ( use 3x4 matrix because positional translation is required )
//----------------------------------------------------------------------------------------------------------------------

void carwheel::transformPositionVectorToWorld( vector3 * r, vector3 * v )
{
	wheelObject->concatenatedTransform.multiplyV3By3x4mat( r, v ); 
};	

//----------------------------------------------------------------------------------------------------------------------
// transform local space POSITION vector to body  ( use 3x4 submatrix because positional translation is required )
//----------------------------------------------------------------------------------------------------------------------

void carwheel::transformPositionVectorToBody( vector3 * r, vector3 * v )
{
	wheelObject->transform.multiplyV3By3x4mat( r, v );				//
};	

//----------------------------------------------------------------------------------------------------------------------
// get position of  wheel  in world co-ordinates
//----------------------------------------------------------------------------------------------------------------------

vector3 * carwheel::getPositionInWorld()
{
	return(&wheelObject->transformedCentroid);
//	return(&wheelObject->concatenatedTransform.translation);
};

//================================================================================================================================
// END OF FILE
//================================================================================================================================
