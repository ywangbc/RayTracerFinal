#include <cmath>
#include <assert.h>
#include <cfloat>



#include "Box.h"


const double length = 0.5;
enum
{
	XPLANE = 0,
	YPLANE,
	ZPLANE
};


//To test if two doubles equal
bool eq(double a, double b)
{
	if (abs(a - b) < RAY_EPSILON)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	double Tnear = -DBL_MAX;
	double Tfar = DBL_MAX;

	double Bl[3] = { -length, -length, -length };
	double Bh[3] = { length, length, length };

	vec3f Rp = r.getPosition();
	vec3f Rd = r.getDirection();

	

	for (int face = 0; face < 3; face++)
	{

		if (Rd[face] == 0)
		{
			if (Rp[face]<-length || Rp[face] >length)
			{
				return false;
			}
		}
		else
		{
			double T1 =(Bl[face] - Rp[face]) / Rd[face];
			double T2 = (Bh[face] - Rp[face]) / Rd[face];
			/* since T1 intersection with near plane */
			if (T1 > T2)
			{
				swap(T1, T2);
			}
			/* want largest Tnear */
			if (T1 > Tnear)
			{
				Tnear = T1;
			}
			/* want smallest Tfar */
			if (T2 < Tfar)
			{
				Tfar = T2;
			}
		}	
	}

	if (Tnear > Tfar)
	{
		return false;
	}

	if (Tfar < RAY_EPSILON)
	{
		return false;
	}

	i.obj = this;
	i.t = Tnear;

	vec3f isectP = r.at(Tnear);

	if (eq(isectP[0], -length))
	{
		i.N = vec3f(-1.0, 0, 0);
		return true;
	}
	else if (eq(isectP[0], length))
	{
		i.N = vec3f(1.0, 0, 0);
		return true;
	}
	else if (eq(isectP[1], -length))
	{
		i.N = vec3f(0, -1.0, 0);
	}
	else if (eq(isectP[1], length))
	{
		i.N = vec3f(0, 1.0, 0);
		return true;
	}
	else if (eq(isectP[2], -length))
	{
		i.N = vec3f(0, 0, -1.0);
		return true;
	}
	else if (eq(isectP[2], length))
	{
		i.N = vec3f(0, 0, 1.0);
		return true;
	}
	else
	{
		cout << "Box Intersection Wrong!\n";
		return false;
	}
}
