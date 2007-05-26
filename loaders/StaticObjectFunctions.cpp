// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

#include <math.h>
#include <algorithm>
#include <limits>

#include "StaticObjectFunctions.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

void transform_object(StaticObjectList &objs, const float m[4][4]) {
  // Find bounds of object
  StaticObjectList::const_iterator I = objs.begin();
  StaticObjectList::const_iterator Iend = objs.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    
    float *v = so->getVertexDataPtr();
    for (unsigned int i = 0; i < so->getNumPoints(); ++i) {

      float x = v[i * 3 + 0];
      float y = v[i * 3 + 1];
      float z = v[i * 3 + 2];
      float w = 1.0f;

      // Transform points by matrix
      float nx = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
      float ny = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
      float nz = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
      float nw = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;

      v[i * 3 + 0] = nx / nw;
      v[i * 3 + 1] = ny / nw;
      v[i * 3 + 2] = nz / nw;

    }
  }
}
 
void scale_object(StaticObjectList &objs, Scaling scale, Alignment align, bool ignore_minus_z) {
  float min[3] = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
  float max[3] = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };
 
  float sum_pos[3] = {0.0f, 0.0f, 0.0f};
  unsigned int num_points = 0;

  // Find bounds of object
  StaticObjectList::const_iterator I = objs.begin();
  StaticObjectList::const_iterator Iend = objs.end();
  for (; I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    
    float m[4][4];
    so->getMatrix().getMatrix(m);
    float *v = so->getVertexDataPtr();
    for (unsigned int i = 0; i < so->getNumPoints(); ++i) {
      ++num_points;

      float x = v[i * 3 + 0];
      float y = v[i * 3 + 1];
      float z = v[i * 3 + 2];
      float w = 1.0f;

      // Transform points by matrix
      float nx = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
      float ny = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
      float nz = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
      float nw = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;

      if (x < min[0]) min[0] = x; 
      if (y < min[1]) min[1] = y; 
      if (z < min[2]) min[2] = z; 

      if (x > max[0]) max[0] = x; 
      if (y > max[1]) max[1] = y; 
      if (z > max[2]) max[2] = z; 

      sum_pos[0] += x;
      sum_pos[1] += y;
      sum_pos[2] += z;
    }
  }

  if (ignore_minus_z) {
    min[2] = 0.0f;
  }

  // Re-scale all points
  float diff_x = fabs(max[0] - min[0]);
  float diff_y = fabs(max[1] - min[1]);
  float diff_z = fabs(max[2] - min[2]);
 
  // Isotropic keeps the "aspect ratio" of the model by performing a constant
  // Scaling in all axis.
  // Otherwise each axis is scaled by a different amount
  switch (scale) {
    case SCALE_ISOTROPIC_X: // Scale so X axis is 1.0
      diff_y = diff_z = diff_x;
      break;
    case SCALE_ISOTROPIC_Y: // Scale so Y axis is 1.0
      diff_x = diff_z = diff_y;
      break;
    case SCALE_ISOTROPIC_Z: // Scale so Z axis is 1.0
      diff_x = diff_y = diff_z;
      break;
    case SCALE_ISOTROPIC: // Scale so largest axis is 1.0
      diff_x = diff_y = diff_z = std::max(std::max(diff_x, diff_y), diff_z);
      break;
    case SCALE_ANISOTROPIC:
      // Nothing to do
      break;
    case SCALE_NONE:
      diff_x = diff_y = diff_z = 1.0;
      break;
  }

  float scale_x = 1.0 / (diff_x);
  float scale_y = 1.0 / (diff_y);
  float scale_z = 1.0 / (diff_z);

  float trans_x = 0.0f;
  float trans_y = 0.0f;
  float trans_z = 0.0f;

  switch (align) {
    case ALIGN_NONE:
      // Do nothing
      break;
    case ALIGN_CENTRE_MASS:
      trans_x = -(sum_pos[0] / (float)num_points);
      trans_z = -(sum_pos[1] / (float)num_points);
      trans_y = -(sum_pos[2] / (float)num_points);
      break;
    case ALIGN_CENTRE_EXTENT: 
      trans_x = -(max[0] - min[0]) / 2.0 -min[0];
      trans_y = -(max[1] - min[1]) / 2.0 -min[1];
      trans_z = -(max[2] - min[2]) / 2.0 -min[2];
      break;
    case ALIGN_Z:
      trans_z = -(min[2]);
      break;
    case ALIGN_BBOX_LC:
      trans_x = -min[0];
      trans_y = -min[1];
      trans_z = -min[2];
      break;
    case ALIGN_BBOX_HC:
      trans_x = -max[0];
      trans_y = -max[1];
      trans_z = -max[2];
      break;
  }

  for (I = objs.begin(); I != Iend; ++I) {
    SPtr<StaticObject> so = *I;
    assert(so);
    
    float *v = so->getVertexDataPtr();
    float m[4][4];
    so->getMatrix().getMatrix(m);
    for (unsigned int i = 0; i < so->getNumPoints(); ++i) {
      float x = v[i * 3 + 0];
      float y = v[i * 3 + 1];
      float z = v[i * 3 + 2];
      float w = 1.0f;

      // Transform the points: perform the scaling and then transform the 
      // points back again
      float nx = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
      float ny = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
      float nz = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
      float nw = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;
      // So we do this here? Or later on?
      x += trans_x;
      y += trans_y;
      z += trans_z;

      // Scale points
      x *= scale_x;
      y *= scale_y;
      z *= scale_z;

      nx = m[0][0] * x + m[1][0] * y + m[2][0] * z + m[3][0] * w;
      ny = m[0][1] * x + m[1][1] * y + m[2][1] * z + m[3][1] * w;
      nz = m[0][2] * x + m[1][2] * y + m[2][2] * z + m[3][2] * w;
      nw = m[0][3] * x + m[1][3] * y + m[2][3] * z + m[3][3] * w;

      x = nx / nw;
      y = ny / nw;
      z = nz / nw;

      v[i * 3 + 0] = x;
      v[i * 3 + 1] = y;
      v[i * 3 + 2] = z;

    }
  }
}
  

} /* namespace Sear */
