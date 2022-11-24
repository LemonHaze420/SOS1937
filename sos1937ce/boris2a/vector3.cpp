// $Header$

// $Log$

#include "bs2all.h"

void vector3::rationaliseAngleVector()
{
return;
        while (x >= TWOPI)
                 x -= (float)TWOPI;
        while (y >= TWOPI)
                 y -= (float)TWOPI;
        while (z >= TWOPI)
                 z -= (float)TWOPI;

        while (x < 0)
                 x += (float)TWOPI;
        while (y < 0)
                 y += (float)TWOPI;
        while (z < 0)
                 z += (float)TWOPI;
}
