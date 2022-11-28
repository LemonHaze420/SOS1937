//================================================================================================================================
// nongeodata.h
// ------------
//			A single vertex of a triangle - we need its geometric co-ordinate, its vertex normal
//================================================================================================================================

#ifndef _BS_nongeodata
#define _BS_nongeodata

class nongeodata
{
	friend class world;
	friend class triangle;
	friend class engine;

	public:
		nongeodata() {};
		~nongeodata() {};


			//
			// union with integer components is for the local software renderer (when we are not using D3D or hardware)
			//

		union {
				float u;
				slong ui;
		};
		union {
				float v;
				slong vi;
		};	

	private:

};


#endif	// _BS_vertex

//================================================================================================================================
//END OF FILE
//================================================================================================================================
