// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: 3ds.h,v 1.14 2004-10-28 09:30:54 simon Exp $

#ifndef SEAR_3DS_H
#define SEAR_3DS_H 1

//#include <stdlib.h>
#include <string>
#include <list>
#include <map>

#include <lib3ds/file.h>

#include "common/types.h"

#include "src/Model.h"

namespace Sear {

// Forward declarations	

class ThreeDS : public Model {
public:
  /*
   * Defualt constructor
   */ 
  ThreeDS(Render*);

  /*
   * Destructor
   */ 
  ~ThreeDS();
  
  /*
   * Initialise 3ds model. Should probably be combined with loadModel
   */ 
  bool init(const std::string &file_name);

  /*
   * Called when model is to be removed from memory. It cleans up its children.
   */
  void shutdown();
  void render(bool);

  void invalidate();
  void setHeight(float height) { m_height = height; }

protected:
  class RenderObject {
public:
   RenderObject() : 
     vertex_data(NULL),
     normal_data(NULL),
     texture_data(NULL),
     num_points(0),
     texture_id(0),
     texture_mask_id(0),
     vb_vertex_data(0),
     vb_texCoords_data(0),
     vb_normal_data(0)
 {}
    Vertex_3 *vertex_data;
    Normal *normal_data;
    Texel *texture_data;
    unsigned int num_points;
    int texture_id;
    int texture_mask_id;
    std::string material_name;

    unsigned int vb_vertex_data;
    unsigned int vb_texCoords_data;
    unsigned int vb_normal_data;
  };

  typedef struct {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
  } Material;
  
  void render_node(Lib3dsNode * node, Lib3dsFile * file);
  void render_mesh(Lib3dsMesh * mesh, Lib3dsFile * file);
  void render_file(Lib3dsFile * file);
  
  std::list<RenderObject*> render_objects;
  std::map<std::string, Material*> material_map;
  bool _initialised;
  unsigned int _list;
  unsigned int _list_select;
  float m_height;
};

} /* namespace Sear */

#endif /* SEAR_3DS_H */
