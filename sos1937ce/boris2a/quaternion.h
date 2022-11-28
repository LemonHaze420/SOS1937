//================================================================================================================================
// quaternion.h
// ------------
//================================================================================================================================

#ifndef _BS_quaternion
#define _BS_quaternion

class quaternion
{

	public:
			//quaternion( float xx, float yy, float zz, float ww);
			//~quaternion();


			float x, y, z, w;


			void quatToMat( matrix3x4 * m );			// return result matrix in 'm' (only 3x3 part affected)
			void quatFromMat( matrix3x4  * m );		// return quaternion from parameter matrix

	private:
		
};

#endif	// _BS_quaternion

//================================================================================================================================
//END OF FILE
//================================================================================================================================
