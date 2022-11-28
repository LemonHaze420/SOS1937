//================================================================================================================================
// object.h
// --------
//
//================================================================================================================================

#ifndef _BS_object
#define _BS_object



#define ATTACH_FOLLOW	    1			//	attached camera follows the object positin.

#define COLLIDE_COLLIDABLE  1			//  detect collision with collidable world triangles
#define COLLIDE_OBJECT	    2			//  detect collision with other objects
#define COLLIDE_SHADOW	    4			//  this object and all its children will be in shadow 

class object {
	
	friend class engine;
	friend class car;
	
public:
	//object(){};
	object( model * modelPtr, uchar cflags, bool defincam );			// fill in all defaults from the model
	~object();
	
	void deleteObjectTreeAndDisconnect();
	
	
	
	void setPosition( vector3 * p )
	{
		transform.translation = *p;
		changed = true;
	}
	void setPosition_Y(float p )
	{
		transform.translation.y = p;
		changed = true;
	}
	
	void setAngle( vector3 * a );
	void getAngle( vector3 * a )
	{
		*a = angle;
	}
	
	void setAngleNOCAMERA( vector3 * a );
	
	void setAngle_X( float a );
	void setAngle_Y( float a );
	void setAngle_Z( float a );
	
	void movePosition( vector3 * p )
	{
		transform.translation += *p;
		changed = true;
	}
	void moveAngle( vector3 * a )
	{
		angle += *a;
		setAllAngles( &transform );
		changed = true;
	}
	void moveAngleNOCAMERA( vector3 * a )
	{
		angle += *a;
		setAllAngles( &transform );
		changed = true;
	}
	
	void insertChild( object * childObject );
	void insertSister( object * sisterObject );
	void detachChild();
	void detachSister();
	
	void tiltToNewUpVector( vector3 * upVector );
	
	void moveAngleFromLocalPosition( vector3 * a , vector3 * p);
	
	//private:
	
	void deleteObjectTree();
	
	vector3  attachmentPoint;
	vector3  transformedattachmentPoint;
	
	bool changed;							// true iff the nodes position or angle have been altered.
	
	
	RboundBox	transformedBoundingBox;		// boundingBox is in modelData->boundingBox;
	// - the transformed contains 8 corner co-ordinate vectors and is held here
	// for use by both physics and rendering.
	
	vector3		transformedCentroid;		// transformed object centroid. Held here for both physics and rendering
	//
	//
	
	//boundSphere		boundingSphere;
	
	model		* modelData;				// 0 = no model data (invisible node )
	
	matrix3x4	transform;					// local transform .. reflects movement at the level of this object only
	matrix3x4   concatenatedTransform;		// we need this to be held so that after updating geometry we keep the
	// transform for physics vector data.
	
				//
				// Pointers to other nodes in hierarchy
				//
	object * child;			
	object * sister;
	
				//
				// Pointers connecting chain of objects from engine  ( an object is UNIQUE and belongs to a single ENGINE )
				// (these are only used when an object is attached to the engine)
				//
	
	object * prev;
	object * next;
	
	void    inheritTransformationsT( matrix3x4 * parentTransform );
	void    inheritTransformations( matrix3x4 * parentTransform );
	void    inheritTransformations();		// parent transform is the identity matrix
	
	
				// local data kept so we can update just one angle at a time, quickly rather than recalculating matrix multiplies
	
	vector3   angle;
	vector3   sinAngle, cosAngle;
	matrix3x4 RotXRotZ, RotYRotX, RotX, RotY, RotZ;
	
	
	void setAllAngles( matrix3x4 * resultmatrix );
	
				//
				// points to an attached camera if any.
				//
	
	camera * attached;
	ulong    attachedValue;
	
				//
				// flags for which collision types are enabled ....
				//
	
	uchar   collisionFlags;
	bool    visible;			// is object to be rendered ?
	
	bool definedInCameraSpace ;
	
	object * prevsister;
	object * parent;
	
	SphereSet * transformedSpheres;		// holds TRANSFORMED collision bounding information IFF it exists.
	
	bool avoidOcclusion; // defaults to false, set to true for objects attached to camera etc. for speed
	
	vector3 turningPoint;	// point of rotation for object - default to (0,0,0)
};

#endif	// _BS_error

//================================================================================================================================
//END OF FILE
//================================================================================================================================

