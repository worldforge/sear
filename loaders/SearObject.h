// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2007 Simon Goodall

// $Id: SearObject.h,v 1.1 2007-01-24 09:52:55 simon Exp $

#ifndef SEAR_SEAROBJECT_H
#define SEAR_SEAROBJECT_H 1

#include <string>
#include <list>
#include <map>

#include <sigc++/trackable.h>
#include <varconf/Config.h>

#include "common/types.h"
#include "Model.h"

namespace Sear {

// Forward declarations	
class StaticObject;

class SearObject : public Model, public sigc::trackable {
public:

  SearObject();
  ~SearObject();
  
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
  virtual StaticObjectList &getStaticObjects() { return m_static_objects; }

protected:
  void varconf_error_callback(const char *message);
  int load(const std::string &filename);

  bool m_initialised;
  StaticObjectList m_static_objects;
  varconf::Config m_config;
};

} /* namespace Sear */

#endif /* SEAR_SEAROBJECT_H */
