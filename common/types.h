// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: types.h,v 1.7 2004-09-29 10:50:25 jmt Exp $

#ifndef SEAR_COMMON_TYPES_H
#define SEAR_COMMON_TYPES_H 1

#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>

namespace Sear {

typedef struct {
  float x;
  float y;
  float z;
} Vertex_3;

typedef struct {
  float x;
  float y;
  float z;
  float w;
} Vertex_4;

typedef struct {
  float x;
  float y;
  float z;
} Normal;

typedef struct {
  float s;
  float t;
} Texel;

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Color_4;

typedef unsigned int TextureObject;

typedef enum {
  POINT,
  LINES,
  TRIANGLES,
  QUADS,
  TRIANGLE_FAN,
  TRIANGLE_STRIP,
  QUAD_STRIP
} DataType;

typedef struct {
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
  float emission[4];
} Material;

typedef struct {
  Vertex_3 *vertex_array;
  unsigned int vertex_vbo;
  Normal *normal_array;
  unsigned int normal_vbo;
  Texel *tex_coord_array;
  unsigned int tex_vbo;
  unsigned int number_of_points;
  DataType data_type;
  Material material;
  TextureObject texture_1;
  unsigned int disp_list;
} Mesh;

class OrientBBox
{
public:
    OrientBBox(const WFMath::AxisBox<3>& axisBox);
    
    OrientBBox();
    
    void rotate(const WFMath::Quaternion& q);
    
    WFMath::Vector<3> points[8];
};

typedef enum {
  UPPER_LEFT_FRONT = 0,
  UPPER_LEFT_BACK,
  UPPER_RIGHT_FRONT,
  UPPER_RIGHT_BACK,
  LOWER_LEFT_FRONT,
  LOWER_LEFT_BACK,
  LOWER_RIGHT_FRONT,
  LOWER_RIGHT_BACK,
  LAST_POSITION
} BBoxPosition;

} /* namespace Sear */

#endif /* SEAR_COMMON_TYPES_H */
