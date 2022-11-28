//================================================================================================================================
// matrix3x4.h
// -----------
//				general 3x4 matrix
//================================================================================================================================

#ifndef _BS_matrix3x4
#define _BS_matrix3x4

class matrix3x4
{
	friend class camera;

	public:
			//matrix3x4();
			//~matrix3x4();


			void setmat( float e00, float e10, float e20, 
				   float e01, float e11, float e21, 
				   float e02, float e12, float e22, 
				   float e03, float e13, float e23 );

			void set3x3submat(float e00, float e10, float e20, 
						 float e01, float e11, float e21, 
						 float e02, float e12, float e22);

			void set3x3submat( matrix3x4 * m);

			void Transpose3x4( matrix3x4 * m );

			void multiply3x3submats( matrix3x4 * m,  matrix3x4 * n );
			//void multiply4x4mats( matrix4x4 * m,  matrix4x4 * n );
			void multiplyV3By3x4mat( vector3 * outvector, vector3 * invector );
			void multiplyV3By3x3mat( vector3 * outvector, vector3 * invector );
			void multiply3x4mats( matrix3x4 * m,  matrix3x4 * n );
			void multiplyIntoTransform( vector3 * p );
			void scale3x3submatrix( matrix3x4 * m, vector3 * s);
			void getInverse( matrix3x4 * inverseMatrix );
			void orthogonalise();

				// - since we use the 3*3 submatrix most often lets group its elements
				//   first BEFORE e30,e31,e32  and e03, e13, e23

			float E00, E10, E20;
			float E01, E11, E21;
			float E02, E12, E22;
			vector3 translation;

			matrix3x4& operator *= (const float f)
			{
				E00 *= f; E10 *= f; E20 *= f;
				E01 *= f; E11 *= f; E21 *= f;
				E02 *= f; E12 *= f; E22 *= f;
				return *this;
			};

			matrix3x4& operator += (matrix3x4& m)
			{
				E00 += m.E00; E10 += m.E10; E20 += m.E20;
				E01 += m.E01; E11 += m.E11; E21 += m.E21;
				E02 += m.E02; E12 += m.E12; E22 += m.E22;

				return *this;
			};
	private:
		
};

#endif	// _BS_matrix3x4

//================================================================================================================================
//END OF FILE
//================================================================================================================================
