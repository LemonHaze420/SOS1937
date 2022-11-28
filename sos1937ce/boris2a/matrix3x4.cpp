//================================================================================================================================
// matrix3x4.cpp
// -------------
//
//================================================================================================================================

#include "BS2all.h"

#define det2( c00, c01, c10, c11 ) \
	c00 * c11 - c01 * c10
 

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
// orthogonalise
// -------------
//			This is included so that we can ensure that any adaptation of a transform which may introduce an error (as in the 
//			physics) can correct the error by re-orthoganilising.
//================================================================================================================================

void matrix3x4::orthogonalise()
{
	static vector3 x,y,z;

	x = vector3( E00, E01, E02 );
	x.Normalise();

	y= vector3( E10, E11, E12 );
	z = x * y;
	z.Normalise();

	y = z * x;
	y.Normalise();

	E00 = x.x;	
	E10 = y.x;	
	E20 = z.x;
	E01 = x.y;	
	E11 = y.y;	
	E21 = z.y;
	E02 = x.z;	
	E12 = y.z;  
	E22 = z.z;


}

//================================================================================================================================
// Get the inverse of this 3x4 matrix. This is done by taking the inverse of the 3x3 submatrix and
// the NEGATION of the translation component ( the bottom row ).
//
// NOTE THAT THIS ASSUMES THAT THE 3x3 SUB MATRIX IS INVERTIBLE  ( Non-Zero determinant). THIS WILL ALWAYS BE THE CASE
// FOR ROTATION MATRICES.
//
//	we use the method    A-1 = adj(A) * ( 1 / det(A) )
//
//================================================================================================================================

void matrix3x4::getInverse( matrix3x4 * inverseMatrix )
{
 
	#define d11  E00
	#define d12  E10
	#define d13  E20

	#define d21  E01
	#define d22  E11
	#define d23  E21

	#define d31  E02
	#define d32  E12
	#define d33  E22

	float a11, a12, a13;
	float a21, a22, a23;
	float a31, a32, a33;
	

		// compute adj(A)


	a11 =  det2( d22, d23,
		 		 d32, d33  );
	a21 = -det2( d21, d23,
		 		 d31, d33  );
	a31 =  det2( d21, d22,
				 d31, d32  );

	a12 = -det2( d12, d13,
		 		 d32, d33  );
	a22 =  det2( d11, d13,
		 		 d31, d33  );
	a32 = -det2( d11, d12,
				 d31, d32  );

	a13 =  det2( d12, d13,
		 		 d22, d23  );
	a23 = -det2( d11, d13,
		 		 d21, d23  );
	a33 =  det2( d11, d12,
				 d21, d22  );

		// compute 1 / det(A)

	float oneoverdet = RECIPROCAL(
							(d11 * ( d22 * d33 - d32 * d23 ) - 
							 d12 * ( d21 * d33 - d31 * d23 ) +
							 d13 * ( d21 * d32 - d31 * d22 ))
								 );

		// calculate  adj(A) * ( 1/det(A) )

	inverseMatrix->E00 = a11 * oneoverdet;
	inverseMatrix->E01 = a12 * oneoverdet;
	inverseMatrix->E02 = a13 * oneoverdet;
	inverseMatrix->E10 = a21 * oneoverdet;
	inverseMatrix->E11 = a22 * oneoverdet;
	inverseMatrix->E12 = a23 * oneoverdet;
	inverseMatrix->E20 = a31 * oneoverdet;
	inverseMatrix->E21 = a32 * oneoverdet;
	inverseMatrix->E22 = a33 * oneoverdet;


	inverseMatrix->translation.x = -translation.x;
	inverseMatrix->translation.y = -translation.y;
	inverseMatrix->translation.z = -translation.z;

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
// set the 4x4 matrix
//================================================================================================================================

void matrix3x4::setmat( float e00, float e10, float e20,
				   float e01, float e11, float e21,
				   float e02, float e12, float e22,
				   float e03, float e13, float e23
				 )
{
	E00 = e00; 
	E01 = e01; 
	E02 = e02; 
	translation.x = e03;
	E10 = e10; 
	E11 = e11; 
	E12 = e12; 
	translation.y = e13;
	E20 = e20; 
	E21 = e21; 
	E22 = e22; 
	translation.z = e23;
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

//================================================================================================================================
//multiplyIntoTransform
//---------------------
//		multiplies a vector by the 3x4 matrix and adds the result to the translation.  This special operation is particularly
//		useful for moving objects in the direction of their local transform.
//================================================================================================================================

void matrix3x4::multiplyIntoTransform( vector3 * p )
{
	translation.x += E00 * p->x + E01 * p->y + E02 * p->z;   
	translation.y += E10 * p->x + E11 * p->y + E12 * p->z;  
	translation.z += E20 * p->x + E21 * p->y + E22 * p->z;  
}

//================================================================================================================================
// scale3x3submatrix
// -----------------
//		 scale the parameter matrix to produce 'this' - translation part not copied
//================================================================================================================================

void matrix3x4::scale3x3submatrix( matrix3x4 * m, vector3 * s)
{
	E00 =	m->E00 * s->x;
	E01 =	m->E01 * s->y;
	E02 =	m->E02 * s->z;

	E10 =	m->E10 * s->x;
	E11 =	m->E11 * s->y;
	E12 =	m->E12 * s->z;

	E20 =	m->E20 * s->x;
	E21 =	m->E21 * s->y;
	E22 =	m->E22 * s->z;
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
