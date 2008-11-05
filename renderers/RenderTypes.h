// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

#ifndef SEAR_RENDERTYPES_H
#define SEAR_RENDERTYPES_H 1

namespace Sear {

typedef enum {
  RES_INVALID = 0,
  RES_POINT,
  RES_LINES,
  RES_TRIANGLES,
  RES_QUADS,
  RES_TRIANGLE_FAN,
  RES_TRIANGLE_STRIP,
  RES_QUAD_STRIP,
  RES_LAST_STYLE
} RenderStyle;

typedef enum {
  ROS_NONE = 0,
  ROS_NORMAL,
  ROS_POSITION,
  ROS_BILLBOARD,
  ROS_HALO
} RotationStyle;

typedef int TextureID;
static const TextureID NO_TEXTURE_ID = 0;

typedef int StateID;

} // namespace Sear

#endif // SEAR_RENDERTYPES_H
