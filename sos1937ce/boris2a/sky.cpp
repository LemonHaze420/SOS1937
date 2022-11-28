//================================================================================================================================
// sky.cpp
// -------
//
//================================================================================================================================

#include "BS2all.h"

void sky::createRotationMatrix()
{
	matrix3x4 RotX, RotY, RotZ, RotXRotZ;
	vector3 cosAngle, sinAngle;


	cosAngle.x = (float)cos(skyangle.x);
	cosAngle.y = (float)cos(skyangle.y);
	cosAngle.z = (float)cos(skyangle.z);
	sinAngle.x = (float)sin(skyangle.x);
	sinAngle.y = (float)sin(skyangle.y);
	sinAngle.z = (float)sin(skyangle.z);
	RotX.set3x3submat(	1,   0,   0,  
						0,  cosAngle.x,  sinAngle.x,  
						0,  -sinAngle.x,  cosAngle.x  
					 );

	RotY.set3x3submat(	cosAngle.y, 0, -sinAngle.y,
						0,  1, 0, 
						sinAngle.y, 0, cosAngle.y
					 );

	RotZ.set3x3submat( 	cosAngle.z,  sinAngle.z, 0, 
						-sinAngle.z, cosAngle.z, 0, 
						0, 0, 1
					  );

	// order of rot...  Z,X,Y

	RotXRotZ.multiply3x3submats( &RotX, &RotZ );
	RotMatrix.multiply3x3submats( &RotY,  &RotXRotZ);	
}

sky::sky( char * filename, float scale, vector3 * initialangle, vector3 * deltaangle, float updaterate )
{
	skyModel = getModelPtr( filename, scale, false, "" );

	updateTimer.setTimer( updaterate );

	skyangle = *initialangle;
	skyangledelta = * deltaangle;

	skyangle.rationaliseAngleVector();
	createRotationMatrix();
}

sky::~sky()
{
	if( boris2->currentSky == this )
	{
		boris2->currentSky = NULL;
		boris2->skyon = false;
	}
}

void sky::updateIfNeeded()
{
	slong ticks = updateTimer.getTicks();

	if( ticks)
	{
		float fticks =(float)ticks;
		skyangle.x += skyangledelta.x * fticks;
		skyangle.y += skyangledelta.y * fticks;
		skyangle.z += skyangledelta.z * fticks;
		skyangle.rationaliseAngleVector();
		createRotationMatrix();
	}

}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
