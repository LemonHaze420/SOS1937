/*
	$Header: C:\\RCS\\C\\BORIS2\\car\\carbody.cpp,v 1.2 1999/01/12 11:45:28 jcf Exp jcf $
	$Log: carbody.cpp,v $
	Revision 1.2  1999/01/12 11:45:28  jcf
	This version seems to work

*/

static const char *rcs_id="$Id: carbody.cpp,v 1.2 1999/01/12 11:45:28 jcf Exp jcf $";

//================================================================================================================================
// carbody.cpp
// -----------
//
//================================================================================================================================

#include "ALLcar.h"

extern engine * boris2;

carbody::carbody( object * whichObject )
{
	bodyObject = whichObject;
};

//----------------------------------------------------------------------------------------------------------------------
//  retrieve the 'y' height at the car body centroid xz. If this xz is ON WORLD the method returns 'true' else 'false'
//----------------------------------------------------------------------------------------------------------------------

bool carbody::getWorldHeight( float * yvalue, triangle ** resulttriPtr)
{
	return( boris2->currentworld->getY( &bodyObject->transformedCentroid, yvalue, resulttriPtr, true ));		
};		

//----------------------------------------------------------------------------------------------------------------------
// transform local space DIRECTION vector to world  ( use 3x3 submatrix because positional translation is not required )
//----------------------------------------------------------------------------------------------------------------------

void carbody::transformDirectionVectorToWorld( vector3 * r, vector3 * v )
{
	bodyObject->concatenatedTransform.multiplyV3By3x3mat( r, v ); 
};	

//----------------------------------------------------------------------------------------------------------------------
// transform local space POSITION vector to world  ( use 3x4 matrix because positional translation is required )
//----------------------------------------------------------------------------------------------------------------------

void carbody::transformPositionVectorToWorld( vector3 * r, vector3 * v )
{
	bodyObject->concatenatedTransform.multiplyV3By3x4mat( r, v ); 
};	

//----------------------------------------------------------------------------------------------------------------------
// get position of  centroid in world co-ordinates
//----------------------------------------------------------------------------------------------------------------------

vector3 * carbody::getPositionInWorld()
{
	return(&bodyObject->transformedCentroid);
};

//================================================================================================================================
// END OF FILE
//================================================================================================================================
