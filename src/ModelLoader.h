// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODELLOADER_H_
#define _MODELLOADER_H_ 1

/*
 * The Model Loader Interface is designed for use with
 * a plugin system. Each ModelLoader will register itself
 * with the ModelHandler object and specify a type of model 
 * to load.
 */ 

#include <string>

//class ObjectProperties;
#include <iostream>
#include "ObjectLoader.h"
namespace Sear {

// Forward Declarationa
class Models;
class WorldEntity;

class ModelLoader {
public:
  ModelLoader() {}
  virtual ~ModelLoader() {}

  virtual void init() {}
  virtual void shutdown() {}

  // TODO: why did =0; suddenly stop working?
  // TODO: reduce  range of args
  virtual Models *loadModel(WorldEntity *we, ObjectProperties *op, const std::string &data_source) { cout << "WATCHA" << endl; return 0; }
  
  
};

} /* namespace Sear */

#endif /* _MODELLOADER_H_ */
