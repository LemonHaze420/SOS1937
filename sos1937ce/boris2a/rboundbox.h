//================================================================================================================================
// RboundBox.h
// ----------
//				Holds a Transformed bounding box - 8 vectors describing all 8 corners
//================================================================================================================================

#ifndef _BS_RboundBox
#define _BS_RboundBox

class  RboundBox{

	friend class engine;

	public:
			RboundBox(){};
			~RboundBox(){};

			void transformBoundBoxM3x4( boundBox * b, matrix3x4 * m );
			void transformBoundBoxM3x3( boundBox * b, matrix3x4 * m );	// transform a boundBox to RboundBox using the 3x3 submatrix
																		// of a 3x4 matrix.
		
	private:
			vector3  v000, v001, v010, v011, v100, v101, v110, v111;


};

#endif	// _BS_RboundBox

//================================================================================================================================
//END OF FILE
//================================================================================================================================
