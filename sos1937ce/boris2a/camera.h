//================================================================================================================================
// camera.h
// --------
//
//================================================================================================================================

#ifndef _BS_camera
#define _BS_camera

class camera
{
	friend class engine;
	
public:
	camera( vector3 * p, float zeyevalue, float zback);
	camera();
	~camera();
	
	
	void formCameraTransformMatrix();
	void setCameraAngle( vector3 * a );
	void moveCameraAngle( vector3 * a );
	
	matrix3x4 RotX, RotY, RotZ;
	vector3   sinAngle, cosAngle;
	//	matrix3x4 inverseRotX, inverseRotY, inverseRotZ;
	vector3 angle;
	
				//
				// position / orientation of camerz in the world ... 
				//
	
	void moveCameraPosition( vector3 * deltap )
	{
		position += *deltap;
		moved = true;
	}
	
	void moveCameraPosition_X( float deltap )
	{
		position.x += deltap;
		moved = true;
	}
	
	void moveCameraPosition_Y( float deltap )
	{
		position.y += deltap;
		moved = true;
	}
	
	void moveCameraPosition_Z( float deltap )
	{
		position.z += deltap;
		moved = true;
	}
	
	void camera::setCameraPosition(  vector3 * p )
	{
		position = *p;
		moved = true;
	}

	void moveCameraPositionInDirection( vector3 * deltap );
	
	void moveCameraPositionLAGGED( vector3 * deltap, vector3 * lagVector);
	
	
	void rotateAboutPoint( vector3 * a, vector3 * p );		// rotate about a point in the world space
	void rotateAboutPoint_ACCURATE( vector3 * a, vector3 * p );		// rotate about a point in the world space
	
	void faceTowardPoint( vector3 * p , bool force = false);
	
	
	
	//
	// Attachment of camera to object
	//
	
	void attach( object * optr, uchar aValue, vector3 * attachPoint);		// attach an object to this camera.
	void detach();							// detach object from this camera.
	
	
	// Linking cameras to form a chain...
	// useful for watching a sequential event (eg. a car race)
	
	void linkAfter( camera * c );
	void linkBefore( camera * c );
	void deLink();
	camera * findNearest( vector3 * p );			// nearest camera 
	//	private:
	
	void transformWorldVector( vector3 * result, vector3 * wvector );
	
	float zeye;					// value needed for projection
	float zbackplane;
	
	vector3 position;
	vector3 direction;
	
	//matrix3x4  RotX, RotY, RotZ, 
	matrix3x4  RotMatrix;
	//matrix3x4  inverseRotX, inverseRotY, inverseRotZ, 
	matrix3x4  inverseRotMatrix;
	
	matrix3x4  faceTurningTransform;		// 3x3 only is ever used
	
	
	object * attached;		// 0 = no attached object

	
	schar Octant;
	
	
	// in case we wish to link cameras to form a chain.
	camera *  next;
	camera *  prev;
	
	
	void setZbackplane( float zback, world * curworld );
	void camera::setZbackplane( float zback );

	float getZbackplane()
	{
		return( zbackplane);
	}
	void checkUnderWater();
	bool underwater;
	bool moved;										// set iff camera has moved (tested during render)
	triangle * underWaterTrianglePtr;
	float minheight;		// minimum height above ground, default 0.1 m
};


#endif	// _BS_camera

//================================================================================================================================
//END OF FILE
//================================================================================================================================
