// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 - 2006 Simon Goodall

// $Id: Mesh.h,v 1.3 2006-05-17 23:15:33 alriddoch Exp $

#ifndef SEAR_COMMON_MESH_H
#define SEAR_COMMON_MESH_H

#include "common/types.h"

#include <sage/GL.h>

namespace Sear
{

class Mesh
{
public:
  Vertex_3 *vertex_array;
  GLuint vertex_vbo;
  Normal *normal_array;
  GLuint normal_vbo;
  Texel *tex_coord_array;
  GLuint tex_vbo;
  unsigned int number_of_points;
  DataType data_type;
  Material material;
  TextureObject texture_1;
  GLuint disp_list;
};

}

#endif
