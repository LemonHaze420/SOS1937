// $Header$

// $Log$
// Revision 1.1  2000-01-13 17:27:04+00  jjs
// First version that supports the loading of objects.
//

#include "BS2all.h"

void matrix3x4::Transpose3x4( matrix3x4 * m )
{
	E00 = m->E00;    
	E10 = m->E01;    
	E20 = m->E02;
	E01 = m->E10;    
	E11 = m->E11;    
	E21 = m->E12;
	E02 = m->E20;    
	E12 = m->E21;    
	E22 = m->E22;
	translation.x = -m->translation.x;
	translation.y = -m->translation.y;
	translation.z = -m->translation.z;
}

//================================================================================================================================
// set the top left 3x3 submatrix = rotation sub-matrix of 4x4 transform  
//================================================================================================================================

void matrix3x4::set3x3submat( float e00, float e10, float e20, 
						 float e01, float e11, float e21, 
						 float e02, float e12, float e22   )
{
	E00 = e00; 
	E01 = e01; 
	E02 = e02; 
	E10 = e10; 
	E11 = e11; 
	E12 = e12; 
	E20 = e20; 
	E21 = e21; 
	E22 = e22;
}

void matrix3x4::set3x3submat( matrix3x4 * m)
{

	E00 = m->E00; 
	E01 = m->E01; 
	E02 = m->E02; 
	E10 = m->E10; 
	E11 = m->E11; 
	E12 = m->E12; 
	E20 = m->E20; 
	E21 = m->E21; 
	E22 = m->E22;
}

//================================================================================================================================
//  this = multiply the top left 3x3 rotation submatrices of 4x4 transform ( m * n ) :: 27 multiplies
//================================================================================================================================

void matrix3x4::multiply3x3submats( matrix3x4 * m,  matrix3x4 * n )
{
	E00 =	m->E00 * n->E00 + m->E01 * n->E10 + m->E02 * n->E20;
	E01 =	m->E00 * n->E01 + m->E01 * n->E11 + m->E02 * n->E21;
	E02 =	m->E00 * n->E02 + m->E01 * n->E12 + m->E02 * n->E22;
	
	E10 =	m->E10 * n->E00 + m->E11 * n->E10 + m->E12 * n->E20;
	E11 =	m->E10 * n->E01 + m->E11 * n->E11 + m->E12 * n->E21;
	E12 =	m->E10 * n->E02 + m->E11 * n->E12 + m->E12 * n->E22;

	E20 =	m->E20 * n->E00 + m->E21 * n->E10 + m->E22 * n->E20;
	E21 =	m->E20 * n->E01 + m->E21 * n->E11 + m->E22 * n->E21;
	E22 =	m->E20 * n->E02 + m->E21 * n->E12 + m->E22 * n->E22;
}

//================================================================================================================================
//  this = multiply the two 4x4 matrices ( m * n )	ASSUMING THAT THE LAST COLUMN IS (0,0,0,1)
//		   (effectively the empirical information is in the 3x4 submatrix)
//================================================================================================================================

void matrix3x4::multiply3x4mats( matrix3x4 * m,  matrix3x4 * n )
{
	E00 =	m->E00 * n->E00 + m->E01 * n->E10 + m->E02 * n->E20 ;
	E01 =	m->E00 * n->E01 + m->E01 * n->E11 + m->E02 * n->E21 ;
	E02 =	m->E00 * n->E02 + m->E01 * n->E12 + m->E02 * n->E22 ;
	translation.x =	m->E00 * n->translation.x + m->E01 * n->translation.y + m->E02 * n->translation.z + m->translation.x ;

	E10 =	m->E10 * n->E00 + m->E11 * n->E10 + m->E12 * n->E20 ;
	E11 =	m->E10 * n->E01 + m->E11 * n->E11 + m->E12 * n->E21 ;
	E12 =	m->E10 * n->E02 + m->E11 * n->E12 + m->E12 * n->E22 ;
	translation.y =	m->E10 * n->translation.x + m->E11 * n->translation.y + m->E12 * n->translation.z + m->translation.y ;

	E20 =	m->E20 * n->E00 + m->E21 * n->E10 + m->E22 * n->E20 ;
	E21 =	m->E20 * n->E01 + m->E21 * n->E11 + m->E22 * n->E21 ;
	E22 =	m->E20 * n->E02 + m->E21 * n->E12 + m->E22 * n->E22 ;
	translation.z =	m->E20 * n->translation.x + m->E21 * n->translation.y + m->E22 * n->translation.z + m->translation.z ;
}

//================================================================================================================================
//  outvector = matrix (3*4 component) multiplied by invector.  This is used for transforming a vector co-ordinate. Assumes that
//  the fourth component of the vector is initially '1' since we don't need it.
//================================================================================================================================


void matrix3x4::multiplyV3By3x4mat( vector3 * outvector, vector3 * invector )
{
	outvector->x = E00 * invector->x + E01 * invector->y + E02 * invector->z + translation.x;   // v->? == 1
	outvector->y = E10 * invector->x + E11 * invector->y + E12 * invector->z + translation.y;  
	outvector->z = E20 * invector->x + E21 * invector->y + E22 * invector->z + translation.z;  

}

void matrix3x4::multiplyV3By3x3mat( vector3 * outvector, vector3 * invector )
{
	outvector->x = E00 * invector->x + E01 * invector->y + E02 * invector->z ;  
	outvector->y = E10 * invector->x + E11 * invector->y + E12 * invector->z ;  
	outvector->z = E20 * invector->x + E21 * invector->y + E22 * invector->z ;  
}
