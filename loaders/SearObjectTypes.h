// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: SearObjectTypes.h,v 1.2 2007-01-27 11:38:48 simon Exp $

#ifndef SEAR_LOADERS_SEAROBJECTTYPES_H
#define SEAR_LOADERS_SEAROBJECTTYPES_H 1

#include <inttypes.h>

namespace Sear {

/*
 The SearObject File Format version 1.
  * Intended for static objects only.
  For Each mesh;
   * Required 1 Vertex Array, 1 Normal Array , 1 Texture Coord Array.
   * Optional Indicies array
   * Supports one texture identifier
   * Supports material properties
   * Supports mesh and texture transform.
 
  Starts with the header struct
  Then for each of the header->num_meshes there is a mesh structure.
  Each structure is followed by the vertex data arrays.

  E.g.

  header

  mesh_struct_1
  vertex_data_array (float)
  normal_data_array (float)
  texture_coords_array (float)
  indicies_array (uint32_t)

  mesh_struct_2
  ....


NOTES:
  * What about colour arrays?
  * What about multiple textures?

*/

typedef struct {
  char magic[8];       // SEARSTAT -- SearObjectStatic
  uint16_t byte_order; // 0xFF00 ???
  uint8_t version;     // File format version
  uint32_t num_meshes; // Number of mesh structs following
} SearObjectHeader; 


typedef struct {
  float mesh_transform[4][4];
  float texture_transform[4][4];
  char texture_map[256];
  uint32_t num_vertices;
  uint32_t num_faces;

  float ambient[4];
  float diffuse[4];
  float specular[4];
  float emissive[4];
  float shininess;

} SearObjectMesh;


} /* namespace Sear */

#endif /* SEAR_LOADERS_SEAROBJECTTYPES_H */
