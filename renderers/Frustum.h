// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

//$Id: Frustum.h,v 1.1 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_FRUSTUM_H
#define SEAR_FRUSTUM_H 1

#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include "common/types.h"

namespace Sear {

// Forward Declarations
class WorldEntity;

class Frustum {
public:
  static void getFrustum(float frustum[6][4], float projp[16], float modl[16]);
  static bool pointInFrustum(float frustum[6][4], float x, float y, float z );
  static int cubeInFrustum(float frustum[6][4], WorldEntity *we );
  static int patchInFrustum(float frustum[6][4], const WFMath::AxisBox<3> & bbox); 
  static float distFromNear(float frustum[6][4], float x, float y, float z);
  static bool sphereInFrustum(float frustum[6][4], const WFMath::AxisBox<3> &bbox, const WFMath::Point<3> &pos);
  static int orientBBoxInFrustum(float frustum[6][4], const OrientBBox &orient, const WFMath::Point<3> &pos);
};
  
} /* namespace Sear */

#endif /* SEAR_FRUSTUM_H */
