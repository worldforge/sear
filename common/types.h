// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: types.h,v 1.2 2004-01-26 22:07:35 simon Exp $

#ifndef SEAR_COMMON_TYPES_H
#define SEAR_COMMON_TYPES_H 1

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
} Mesh;

} /* namespace Sear */

#endif /* SEAR_COMMON_TYPES_H */
