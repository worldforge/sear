// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2008 Simon Goodall

#ifndef SEAR_LOADERS_LIBMODELFILE_H
#define SEAR_LOADERS_LIBMODELFILE_H 1

/*
 * This reprents a model based upon its bounding box.
 *
 */ 
#include <sigc++/trackable.h>

#include <varconf/config.h>

#include "Model.h"

namespace Sear {

class StaticObject;

class LibModelFile : public Model, public sigc::trackable {
public:

  /*
   * Constructor stores the bounding box for the basis of this model.
   * wrap indicates how to produce the texture coords. IE 0->1 or 0->size of face
   */ 	
  LibModelFile();

  /*
   * Default destructor
   */ 
  ~LibModelFile();
  
  /*
   * Creates the data arrays
   */ 
  int init(const std::string &filename);

  /*
   * Cleans up object
   */ 
  virtual int shutdown();
  virtual void render(bool); 

  virtual bool isInitialised() const { return m_initialised; }

  virtual void contextCreated();
  virtual void contextDestroyed(bool check);

  virtual bool hasStaticObjects() const { return true; }
  virtual StaticObjectList &getStaticObjects() { return m_static_objects; }

private:
  void varconf_error_callback(const char *message);

  bool m_initialised;

  varconf::Config m_config;

  StaticObjectList m_static_objects;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_LIBMODELFILE_H */
