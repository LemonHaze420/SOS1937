// $Header$

// $Log$
// Revision 1.3  2000-01-18 14:48:07+00  jjs
// Changed to use intrinsics for speed.
//
// Revision 1.2  2000-01-17 12:37:53+00  jjs
// Latest version that has corrected Alpha Test
//
// Revision 1.1  2000-01-13 17:27:44+00  jjs
// First version that supports the loading of objects.
//
// Revision 1.0  1999-12-07 12:14:30+00  jjs
// Initial revision
//

#ifndef _BS_BS2all
#define _BS_BS2all

#define D3D_OVERLOADS

#include <tchar.h>
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#if defined(UNDER_CE)
#include <shsgintr.h>
#include <shintr.h>
#endif

#include "BS2consts.h"
#include "BS2simTypes.h"

#include "tarcman.h"
#include "archiveIface.h"
#include "rgraphics.h"

#include "vector3.h"
#include "matrix3x4.h"
#include "sphere.h"
#include "textureStore.h"
#include "material.h"
#include "nongeodata.h"
#include "vertex.h"
#include "pointdata3d.h"
#include "triangle.h"
#include "mesh.h"
#include "boundbox.h"
#include "model.h"
#include "modelStore.h"
#include "object.h"
#include "xmulder.h"
#include "camera.h"
#include "display.h"
#include "timing.h"
#include "engine.h"

extern engine * boris2;

#endif