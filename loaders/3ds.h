// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: 3ds.h,v 1.26 2006-11-30 20:39:45 simon Exp $

#ifndef SEAR_3DS_H
#define SEAR_3DS_H 1

#include <string>
#include <list>
#include <map>

#include <lib3ds/file.h>
#include <lib3ds/node.h>

#include <sigc++/trackable.h>

#include <varconf/Config.h>

#include "common/types.h"

#include "Model.h"

#include "common/SPtr.h"

namespace Sear {

// Forward declarations	
class StaticObject;

class ThreeDS : public Model, public sigc::trackable {
public:
//  typedef std::list<SPtrShutdown<StaticObject> > StaticObjectList;
  /*
   * Defualt constructor
   */ 
  ThreeDS();

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
  virtual int shutdown();
  virtual void render(bool);

  virtual bool isInitialised() const { return m_initialised; }

  virtual void contextCreated();
  virtual void contextDestroyed(bool check);

  virtual bool hasStaticObjects() const { return true; }
  virtual StaticObjectList getStaticObjects() { return m_render_objects; }
  

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
  
  void varconf_error_callback(const char *message);

  StaticObjectList m_render_objects;
  typedef std::map<std::string, Material*> MaterialMap;
  MaterialMap m_material_map;
  bool m_initialised;
  varconf::Config m_config;  
};

} /* namespace Sear */

#endif /* SEAR_3DS_H */
