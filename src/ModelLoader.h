// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelLoader.h,v 1.10 2002-09-08 00:24:53 simon Exp $

#ifndef SEAR_MODELLOADER_H
#define SEAR_MODELLOADER_H 1

/*
 * The Model Loader Interface is designed for use with
 * a plugin system. Each ModelLoader will register itself
 * with the ModelHandler object and specify a type of model 
 * to load.
 */ 

#include "ModelStruct.h"
#include "Render.h"

namespace Sear {

// Forward Declarationa
class Model;
class WorldEntity;

class ModelLoader {
public:
  ModelLoader() {}
  virtual ~ModelLoader() {}

  virtual void init() {}
  virtual void shutdown() {}

  virtual Model *loadModel(Render *, ModelStruct &) = 0;
  
  
};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
