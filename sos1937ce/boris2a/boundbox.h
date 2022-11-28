//================================================================================================================================
// boundBox.h
// ----------
//				Holds a bounding box - just two vectors describing the lo and hi positions in local space
//================================================================================================================================

#ifndef _BS_boundBox
#define _BS_boundBox

class  boundBox{

	friend class RboundBox;
	friend class model;
	friend class car;
	friend class engine;

	public:
			boundBox(){};
			~boundBox(){};

			bool VISIBILITYOF_transformedBoundBoxM3x4( matrix3x4 * m);

			bool VISIBILITYOF_transformedBoundBoxM3x3( matrix3x4 * m);	
//	private:
			vector3  lo;
			vector3  hi;

};

#endif	// _BS_boundBox

//================================================================================================================================
//END OF FILE
//================================================================================================================================
