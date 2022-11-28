//================================================================================================================================
// vector2P.h
// ----------
//				2D projected vector....
//
//					consists of both the 2D vector (x,y) and the corresponding 3d 'z' co-ordinate of the
//					original 3d vector.
//
//================================================================================================================================

#ifndef _BS_vector2P
#define _BS_vector2P

class vector2P
{
	friend class engine;
	friend class vector3;

	public:

	//private:
		
					// The reason for these unions is to enable the hardware / D3D renderer to have floating point values
					// BUT if we are using local software rendering then we need fixed point integer representations.

			union{	float x;
					slong xi;
			};
			union{	float y;
					slong yi;
			};
			union{	float z;
					slong zi;
			};
			union{	float w;
					slong wi;
			};

			vector3 v3;
			float l;						// light power at this projected vertex
			float specularl;

			uchar SCmaskX, SCmaskY;

	//char pad[14];
};

#endif	// _BS_vector2P

//================================================================================================================================
//END OF FILE
//================================================================================================================================
