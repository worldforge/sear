// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: 3ds.h,v 1.18 2005-06-06 19:29:14 simon Exp $

#ifndef SEAR_3DS_H
#define SEAR_3DS_H 1

//#include <stdlib.h>
#include <string>
#include <list>
#include <map>

#include <lib3ds/file.h>
#include <lib3ds/node.h>

#include <varconf/Config.h>

#include "common/types.h"

#include "Model.h"

namespace Sear {

// Forward declarations	
class RenderObject;

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
  int init(const std::string &file_name);

  /*
   * Called when model is to be removed from memory. It cleans up its children.
   */
  int shutdown();
  void render(bool);

  void invalidate();
  void setHeight(float height) { m_height = height; }

protected:
  
  typedef struct {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
  } Material;
  
  void render_node(Lib3dsNode *node, Lib3dsFile *file);
  void render_file(Lib3dsFile *file);
  void render_mesh(Lib3dsMesh *mesh, Lib3dsFile *file, Lib3dsObjectData *d);
  
  void varconf_callback(const std::string &section, const std::string &key, varconf::Config &config);
  void varconf_error_callback(const char *message);


  std::list<RenderObject*> m_render_objects;
  typedef std::map<std::string, Material*> MaterialMap;
  MaterialMap m_material_map;
  bool m_initialised;
  unsigned int m_list;
  unsigned int m_list_select;
  float m_height;
  varconf::Config m_config;  
};

} /* namespace Sear */

#endif /* SEAR_3DS_H */
