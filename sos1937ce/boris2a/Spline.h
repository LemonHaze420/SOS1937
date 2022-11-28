//================================================================================================================================
// Spline.h
// ---------
//			3d Spline
//================================================================================================================================

#ifndef _BS_spline
#define _BS_spline

class spline
{

public:
	spline(){};
	spline(vector3 * points[10], int n);

	void getSplinePos(float q, vector3 * result); //q is normalised (0->1)
	void getSplineDir(vector3 * result); //returns spline 3D gradient as normalised vector3.
	//NOTE that getSplinePos MUST be called first

	// Call this before using the spline to initialise the variables used by
	// the package.
	void initSpline();

	// Reset the spline back to zero. Use this when the spline's route has been 
	// followed all the way.
	void resetSpline();

private:

	vector3 p[10];

	float t;
	float coef[3][4];
	int curpoint;
	int lastpoint;
	int numpoints;

	void refreshCoefs(int n);
};			

#endif	// _BS_spline

//================================================================================================================================
//END OF FILE
//================================================================================================================================
