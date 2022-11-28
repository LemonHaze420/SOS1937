//================================================================================================================================
// spline.cpp
// ----------
//
//================================================================================================================================

#include<math.h>
#include"BS2all.h"


spline::spline(vector3 * points[10], int n)
{
	t=0.0f;
	numpoints=n;
	for(int i=0;i<numpoints;i++) p[i]=*points[i];
	for(i=0;i<4;i++)
	{
		coef[0][i]=0.0f;
		coef[1][i]=0.0f;
		coef[2][i]=0.0f;
	}
}

void spline::initSpline()
{
	refreshCoefs(0);
	curpoint=0;
	lastpoint=0;
	t=0.0f;
}

void spline::resetSpline()
{
	initSpline();
}

void spline::getSplinePos(float q, vector3 * result)
{
	
	//Check that we haven't gone past a spline point
	//and refresh the coefficients if we have

	if(q<0.0f || q>1.0f) return;

	int pts=numpoints-1;
	FASTFTOL((long *)&curpoint , floor(q*pts));
//	dprintf("Current: %d Last: %d", curpoint, lastpoint);
	if (curpoint!=lastpoint)
	{
		refreshCoefs(curpoint);
		lastpoint=curpoint;
	}


	//Calculate t, t^2, and t^3 for the current spline segment

	float unit=1.0f/((float)pts);
	t=(q-curpoint*unit)*pts;
	float t2=t*t;
	float t3=t2*t;

	//dprintf("GetSP: %f  %f  %f", t, t2, t3);

	result->x = ((coef[0][0] * t3) + (coef[0][1] * t2) + (coef[0][2] * t) + coef[0][3]) * 0.5f;
	result->y = ((coef[1][0] * t3) + (coef[1][1] * t2) + (coef[1][2] * t) + coef[1][3]) * 0.5f;
	result->z = ((coef[2][0] * t3) + (coef[2][1] * t2) + (coef[2][2] * t) + coef[2][3]) * 0.5f;

}

void spline::getSplineDir(vector3 * result)
{
	//For speed, no recalculation of t or the coefficients is performed
	//So getSplinePos() MUST be called before this routine


	float t2=t*t;
	float t3=t2*t;

	result->x = ((3.0f*coef[0][0] * t2) + (2.0f*coef[0][1] * t) + coef[0][2] ) * 0.5f;
	result->y = ((3.0f*coef[1][0] * t2) + (2.0f*coef[1][1] * t) + coef[1][2] ) * 0.5f;
	result->z = ((3.0f*coef[2][0] * t2) + (2.0f*coef[2][1] * t) + coef[2][2] ) * 0.5f;
}

void spline::refreshCoefs(int n)
{
//	dprintf("Refreshing %d.", n);

	coef[0][0] = 3.0f * p[n].x;
	coef[0][1] = -5.0f * p[n].x;
	coef[0][2] = 0.0f;
	coef[0][3] = 2.0f * p[n].x;

	coef[1][0] = 3.0f * p[n].y;
	coef[1][1] = -5.0f * p[n].y;
	coef[1][2] = 0.0f;
	coef[1][3] = 2.0f * p[n].y;

	coef[2][0] = 3.0f * p[n].z;
	coef[2][1] = -5.0f * p[n].z;
	coef[2][2] = 0.0f;
	coef[2][3] = 2.0f * p[n].z;

	if(n!=0)
	{
		coef[0][0] -=  p[n-1].x;
		coef[0][1] += 2.0f * p[n-1].x; 
		coef[0][2] -= p[n-1].x;

		coef[1][0] -=  p[n-1].y;
		coef[1][1] += 2.0f * p[n-1].y; 
		coef[1][2] -= p[n-1].y;

		coef[2][0] -=  p[n-1].z;
		coef[2][1] += 2.0f * p[n-1].z; 
		coef[2][2] -= p[n-1].z;
	}

	
	if(n<(numpoints-1))
	{
		coef[0][0] -= 3.0f * p[n+1].x;
		coef[0][1] += 4.0f * p[n+1].x;
		coef[0][2] += p[n+1].x;

		coef[1][0] -= 3.0f * p[n+1].y;
		coef[1][1] += 4.0f * p[n+1].y;
		coef[1][2] += p[n+1].y;

		coef[2][0] -= 3.0f * p[n+1].z;
		coef[2][1] += 4.0f * p[n+1].z;
		coef[2][2] += p[n+1].z;
		
		if(n<(numpoints-2))
		{
			coef[0][0] += p[n+2].x;
			coef[0][1] -= p[n+2].x;

			coef[1][0] += p[n+2].y;
			coef[1][1] -= p[n+2].y;

			coef[2][0] += p[n+2].z;
			coef[2][1] -= p[n+2].z;
		}
	}	

		
	

}



//================================================================================================================================
// END OF FILE
//================================================================================================================================
