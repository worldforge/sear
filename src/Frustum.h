// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_ 1

#include <wfmath/axisbox.h>

namespace Sear {

// Forward Declarations
class Terrain;
class WorldEntity;
	
class Frustum {
public:
  static void getFrustum(float frustum[6][4], float projp[16], float modl[16]);
  static bool pointInFrustum(float frustum[6][4], float x, float y, float z );
  static int cubeInFrustum(float frustum[6][4], WorldEntity *we );
  static int patchInFrustum(float frustum[6][4], WFMath::AxisBox<3> bbox); 
  static float distFromNear(float frustum[6][4], float x, float y, float z);
  static bool sphereInFrustum(float frustum[6][4], WorldEntity *we, Terrain *);
};
  
} /* namespace Sear */

#endif /* _FRUSTUM_H_ */
