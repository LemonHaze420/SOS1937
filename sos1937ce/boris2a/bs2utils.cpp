//================================================================================================================================
// BS2utils.cpp
// ------------
//
//================================================================================================================================

#include "BS2all.h"

extern engine * boris2;

//================================================================================================================================
// stripfilesuffix
// ---------------
//			replace first '.' with 0 (ASCIIZ terminator)
//================================================================================================================================

void stripfilesuffix( char * filename )
{
	char * cptr = filename;
	char c;
	
	while( (c = *cptr) != 0)
	{
		if(c == '.')
		{
			*cptr = 0;
			break;
		}
		cptr++;
	}
}

//================================================================================================================================
//================================================================================================================================

void startEngine( bool sware,  slong outListCount, slong ttotal, slong vtotal, 
				 slong ntotal, float physicsTicks
				)
{
	boris2 = new engine(   sware, outListCount, ttotal, vtotal, ntotal, physicsTicks  );

	initialiseModelLoading();
	startTextureStore();
	startModelStore();
}

//================================================================================================================================
// normalOctantCode
// ----------------
//		return the octant 0..7 in which a direction vector points. This is used for face normals to allow easy-culllng of faces.
//================================================================================================================================

uchar normalOctantCode( vector3 * v )
{
	uchar code = 7;
	if(v->x < 0) code =  3;				// -4
	if(v->y < 0) code -= 2;				// -2
	if(v->z < 0) code --;				// -1
	return code;
}

//================================================================================================================================
// minmax3 .....  get the minimum and maximum of three floats
//				- requires either 2 or 3 compares
//================================================================================================================================

void minmax3( float a, float b, float c,  float * min, float * max)
{
	if ( a < b )
	{
		if ( b < c )
		{
			*min = a;
			*max = c;
		}
		else
		{
			*max = b;
			if ( a < c )
				*min = a;
			else
				*min = c;
		}

	}
	else
	{	
		if ( b < c )
		{
			*min = b;
			if ( a < c )
				*max = c;
			else
				*max = a;
		}
		else
		{
			*max = a;
			*min = c;
		}
	}
}

//================================================================================================================================
// minmax4 .....  get the minimum and maximum of four floats
//				  - requires 4 compares
//================================================================================================================================

void minmax4( float a, float b, float c, float d, float * min, float * max)
{
	float mincd = d;
	float maxcd = c;

	* min = b;
	* max = a;

	if ( a < b)
	{
		* min = a; 
		* max = b; 
	}
	if ( c < d) 
	{ 
		mincd = c; 
		maxcd = d; 
	}

	if ( * min > mincd)  
		* min = mincd;
	if ( * max < maxcd)  
		* max = maxcd;
}

long sqrttab[0x100]; // declare table of square roots
long isqrttab[0x100]; // declare table of square roots

static void build_table(void) 
{
    unsigned short i;
    float f;
    unsigned int *fi = (unsigned*)&f;   // To access the bits of a float in
                                        // C quickly we must misuse pointers
    for (i = 0; i <= 0x7f; i++) 
	{
        *fi = 0;

            // Build a float with the bit pattern i as mantissa
            // and an exponent of 0, stored as 127
        *fi = (i << 16) | (127 << 23);
        f = (float)sqrt(f);

            // Take the square root then strip the first 7 bits of
            // the mantissa into the table
        sqrttab[i] = (*fi & 0x7fffff);

            // Repeat the process, this time with an exponent of 1,
            // stored as 128
        *fi = 0;
        *fi = (i << 16) | (128 << 23);
        f = (float)sqrt(f);
        sqrttab[i+0x80] = (*fi & 0x7fffff);
    }
}

static void build_itable(void) 
{
    unsigned short i;
    float f;
    unsigned int *fi = (unsigned*)&f;   // To access the bits of a float in
                                        // C quickly we must misuse pointers
    for (i = 0; i <= 0x7f; i++) 
	{
        *fi = 0;

            // Build a float with the bit pattern i as mantissa
            // and an exponent of 0, stored as 127
        *fi = (i << 16) | (127 << 23);
        f = 1.0f/(float)sqrt(f);

            // Take the square root then strip the first 7 bits of
            // the mantissa into the table
        isqrttab[i] = (*fi & 0x7fffff);

            // Repeat the process, this time with an exponent of 1,
            // stored as 128
        *fi = 0;
        *fi = (i << 16) | (128 << 23);
        f = 1.0f/(float)sqrt(f);
        isqrttab[i+0x80] = (*fi & 0x7fffff);
    }
}

void BuildSqrtTable()
{
	build_table();
	build_itable();
}

    // fsqrt - fast square root by table lookup, original C version
float fsqrt(float n) 
{
    unsigned int *num = (unsigned *)&n; // to access the bits of a float in C
                                        // we must misuse pointers

    short e;        // the exponent
    if (n == 0) 
		return (0); /* check for square root of 0 */
    e = (*num >> 23) - 127; /* get the exponent - on a SPARC the */
                            /* exponent is stored with 127 added */
    *num &= 0x7fffff;   /* leave only the mantissa */
    if (e & 0x01) 
		*num |= 0x800000;
                /* the exponent is odd so we have to */
                /* look it up in the second half of  */
                /* the lookup table, so we set the high bit */
    e >>= 1;    /* divide the exponent by two */
                /* note that in C the shift */
                /* operators are sign preserving */
                /* for signed operands */
        // Do the table lookup, based on the quaternary mantissa,
        // then reconstruct the result back into a float
    *num = ((sqrttab[*num >> 16])) + ((e + 127) << 23);
    return(n);
}

float fisqrt(float n) 
{
    unsigned int *num = (unsigned *)&n; // to access the bits of a float in C
                                        // we must misuse pointers

    short e;        // the exponent
    if (n == 0) 
		return (0); /* check for square root of 0 */
    e = (*num >> 23) - 127; /* get the exponent - on a SPARC the */
                            /* exponent is stored with 127 added */
    *num &= 0x7fffff;   /* leave only the mantissa */
    if (e & 0x01) 
		*num |= 0x800000;
                /* the exponent is odd so we have to */
                /* look it up in the second half of  */
                /* the lookup table, so we set the high bit */
    e >>= 1;    /* divide the exponent by two */
                /* note that in C the shift */
                /* operators are sign preserving */
                /* for signed operands */
        // Do the table lookup, based on the quaternary mantissa,
        // then reconstruct the result back into a float
    *num = ((isqrttab[*num >> 16])) + ((e + 127) << 23);
    return(n);
}


//================================================================================================================================
// END OF FILE
//================================================================================================================================
