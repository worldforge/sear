// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_ 1

#include <wfmathaxisbox.h>

namespace Sear {

// Forward Declarations
class WorldEntity;
	
class Frustum {
public:

  static float[6][4] Frustum::getFrustum(float projp[16], float modl[16]);
  static bool Frustum::PointInFrustum(float frustum[6][4], float x, float y, float z );
  static int Frustum::CubeInFrustum(float frustum[6][4], WorldEntity *we );
  static int Frustum::patchInFrustum(float frustum[6][4], WFMath::AxisBox<3> bbox); 
  static float Frustum::distFromNear(float frustum[6][4], float x, float y, float z);
  static bool Frustum::SphereInFrustum(float frustum[6][4], WorldEntity *we);
};
  
} /* namespace Sear */

#endif /* _FRUSTUM_H_ */
