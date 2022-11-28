//================================================================================================================================
// material.h
// ----------
//
//================================================================================================================================

#ifndef _BS_material
#define _BS_material


struct colourComponents {
	uchar blu;
	uchar grn;
	uchar red;
	uchar alpha;

};


class material
{
	public:
			material();
			~material();
//	private:
			union{
					ulong ambient;
					colourComponents ambientc;
			};

			union{
					ulong diffuse;
					colourComponents diffusec;
			};

			union{
					ulong specular;
					colourComponents specularc;
			};

			union{
					ulong emissive;
					colourComponents emissivec;
			};

			ulong power;
			textureData * textureDataPtr;
			BorisMaterial bm;
			ulong sortingKey;				// needed for sorting material for best ordering for hardware renderer
											// this is used by the masked binary radix sort. and should reflect the
											// actual texture used, alpha, etc.
			// .... all relevant data
};

#endif	// _BS_material

//================================================================================================================================
//END OF FILE
//================================================================================================================================
