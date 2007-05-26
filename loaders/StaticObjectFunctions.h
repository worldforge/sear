// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

#ifndef SEAR_LOADERS_STATICOBJECTFUNCTIONS_H
#define SEAR_LOADERS_STATICOBJECTFUNCTIONS_H 1

#include "Model.h"

namespace Sear {

typedef enum {
  ALIGN_NONE = 0,
  ALIGN_CENTRE_MASS,
  ALIGN_CENTRE_EXTENT,
  ALIGN_BBOX_LC,
  ALIGN_BBOX_HC,
  ALIGN_Z
} Alignment;

typedef enum {
  SCALE_NONE = 0,
  SCALE_ISOTROPIC,
  SCALE_ISOTROPIC_X,
  SCALE_ISOTROPIC_Y,
  SCALE_ISOTROPIC_Z,
  SCALE_ANISOTROPIC
} Scaling;

extern void transform_object(StaticObjectList &objs, const float m[4][4]);
extern void scale_object(StaticObjectList &objs, Scaling scale, Alignment align, bool ignore_minus_z);

} /* namespace Sear */

#endif // SEAR_LOADERS_STATICOBJECTFUNCTIONS_H
