//================================================================================================================================
// RboundBox.cpp
// -------------
//
//================================================================================================================================


#include "BS2all.h"

//--------------------------------------------------------------------------------------------------------------------------------
//transformBoundBoxM3x3
//---------------------
//				Turn a bounding box (boundBox) into a Transformed bounding box (RboundBox) by using the 
//				3 x 3 submatrix of the transformation matrix 'm' specified.
//--------------------------------------------------------------------------------------------------------------------------------

void RboundBox::transformBoundBoxM3x3( boundBox * b, matrix3x4 * m )
{
	
	float x0m00 = b->lo.x * m->E00;
	float x1m00 = b->hi.x * m->E00;

	float x0m10 = b->lo.x * m->E10;
	float x1m10 = b->hi.x * m->E10;

	float x0m20 = b->lo.x * m->E20;
	float x1m20 = b->hi.x * m->E20;


	float y0m01 = b->lo.y * m->E01;
	float y1m01 = b->hi.y * m->E01;

	float y0m11 = b->lo.y * m->E11;
	float y1m11 = b->hi.y * m->E11;

	float y0m21 = b->lo.y * m->E21;
	float y1m21 = b->hi.y * m->E21;


	float z0m02 = b->lo.z * m->E02;
	float z1m02 = b->hi.z * m->E02;

	float z0m12 = b->lo.z * m->E12;
	float z1m12 = b->hi.z * m->E12;

	float z0m22 = b->lo.z * m->E22;
	float z1m22 = b->hi.z * m->E22;

	float A = x0m00 + y0m01 ;
	float B = x0m10 + y0m11 ;
	float C = x0m20 + y0m21 ;

	v000.x = A + z0m02;
	v000.y = B + z0m12;
	v000.z = C + z0m22;

	v001.x = A + z1m02;
	v001.y = B + z1m12;
	v001.z = C + z1m22;

	A = x0m00 + y1m01 ;
	B = x0m10 + y1m11 ;
	C = x0m20 + y1m21 ;

	v010.x = A + z0m02;
	v010.y = B + z0m12;
	v010.z = C + z0m22;

	v011.x = A + z1m02;
	v011.y = B + z1m12;
	v011.z = C + z1m22;

	A = x1m00 + y0m01 ;
	B = x1m10 + y0m11 ;
	C = x1m20 + y0m21 ;

	v100.x =  A + z0m02;
	v100.y =  B + z0m12;
	v100.z =  C + z0m22;

	v101.x =  A + z1m02;
	v101.y =  B + z1m12;
	v101.z =  C + z1m22;

	A = x1m00 + y1m01 ;
	B = x1m10 + y1m11 ;
	C = x1m20 + y1m21 ;

	v110.x =  A + z0m02 ;
	v110.y =  B + z0m12 ;
	v110.z =  C + z0m22 ;

	v111.x =  A + z1m02 ;
	v111.y =  B + z1m12 ;
	v111.z =  C + z1m22 ;
}



//--------------------------------------------------------------------------------------------------------------------------------
//transformBoundBoxM3x4
//---------------------
//				Turn a bounding box (boundBox) into a Transformed bounding box (RboundBox) by using the 
//				3 x 4 submatrix of the transformation matrix 'm' specified.
//--------------------------------------------------------------------------------------------------------------------------------

void RboundBox::transformBoundBoxM3x4( boundBox * b, matrix3x4 * m )
{
		
	float x0m00 = b->lo.x * m->E00;
	float x1m00 = b->hi.x * m->E00;

	float x0m10 = b->lo.x * m->E10;
	float x1m10 = b->hi.x * m->E10;

	float x0m20 = b->lo.x * m->E20;
	float x1m20 = b->hi.x * m->E20;


	float y0m01 = b->lo.y * m->E01;
	float y1m01 = b->hi.y * m->E01;

	float y0m11 = b->lo.y * m->E11;
	float y1m11 = b->hi.y * m->E11;

	float y0m21 = b->lo.y * m->E21;
	float y1m21 = b->hi.y * m->E21;


	float z0m02 = b->lo.z * m->E02;
	float z1m02 = b->hi.z * m->E02;

	float z0m12 = b->lo.z * m->E12;
	float z1m12 = b->hi.z * m->E12;

	float z0m22 = b->lo.z * m->E22;
	float z1m22 = b->hi.z * m->E22;

	float A = x0m00 + y0m01 + m->translation.x;//+ Me(0,3);
	float B = x0m10 + y0m11 + m->translation.y;//+ Me(1,3);
	float C = x0m20 + y0m21 + m->translation.z;//+ Me(2,3);

	v000.x = A + z0m02;
	v000.y = B + z0m12;
	v000.z = C + z0m22;

	v001.x = A + z1m02;
	v001.y = B + z1m12;
	v001.z = C + z1m22;

	A = x0m00 + y1m01 + m->translation.x;
	B = x0m10 + y1m11 + m->translation.y;
	C = x0m20 + y1m21 + m->translation.z;

	v010.x = A + z0m02;
	v010.y = B + z0m12;
	v010.z = C + z0m22;

	v011.x = A + z1m02;
	v011.y = B + z1m12;
	v011.z = C + z1m22;

	A = x1m00 + y0m01 + m->translation.x;
	B = x1m10 + y0m11 + m->translation.y;
	C = x1m20 + y0m21 + m->translation.z;

	v100.x =  A + z0m02;
	v100.y =  B + z0m12;
	v100.z =  C + z0m22;

	v101.x =  A + z1m02;
	v101.y =  B + z1m12;
	v101.z =  C + z1m22;

	A = x1m00 + y1m01 + m->translation.x;
	B = x1m10 + y1m11 + m->translation.y;
	C = x1m20 + y1m21 + m->translation.z;

	v110.x =  A + z0m02 ;
	v110.y =  B + z0m12 ;
	v110.z =  C + z0m22 ;

	v111.x =  A + z1m02 ;
	v111.y =  B + z1m12 ;
	v111.z =  C + z1m22 ;
	
}

//================================================================================================================================
//END OF FILE
//================================================================================================================================
