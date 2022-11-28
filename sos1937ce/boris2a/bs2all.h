//================================================================================================================================
// BS2all.h
// --------
//				All includes for BORIS2 engine
//================================================================================================================================

#ifndef _BS_BS2all
#define _BS_BS2all


//#define EDITOR

//#define TESTENV

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

//#include <windows.h>

#include "errorlib.h"	// Error library (IMG)
#include "pool.h"

#include "rGraphics.h"

#include "BS2configuration.h"
#include "BS2simTypes.h"

#include "BS2consts.h"
#include "BS2macros.h"

#include "BS2features.h"


#include <windows.h>
#include <ddraw.h>
#include <d3d.h>


#include "debugger.h"
#include "timeStats.h"

#include "timing.h"
#include "SineTable.h"
#include "intRecip.h"


#include "textureStore.h"

#include "material.h"
#include "nongeoData.h"

#include "vector3.h"
#include "matrix3x4.h"
#include "quaternion.h"
#include "vector2P.h"
#include "vertex.h"


#include "racingLine.h"


#include "sphere.h"
#include "collide.h"

#include "mesh.h"

#include "BS2utils.h"

#include "pointdata3d.h"
#include "pointdata2d.h"

#include "sortlistentry.h"
#include "sort.h"

#include "triangle.h"
#include "hunk.h"


#include "outTriangle.h"
#include "outLists.h"


#include "boundBox.h"
#include "RboundBox.h"


#include "model.h"
#include "modelStore.h"

#include "sky.h"

#include "XMulder.h"					// Mulder file format

#include "object.h"

#include "spriteFX.h"
#include "spriteFXentry.h"

#include "tarcman.h"
#include "archiveIface.h"

#include "camera.h"
#include "display.h"
#include "world.h"
#include "error.h"

#include "engine.h"


#include "softTextures.h"

#include "font.h"
#include "rollOverNumber.h"

#include "grdebug.h"	// JCF debugging code

#include "linkToTriangle.h"
#include "edge.h"

#include "spline.h"

extern engine * boris2;

#endif	// _BS_BS2all

//================================================================================================================================
//END OF FILE
//================================================================================================================================
