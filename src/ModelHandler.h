// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

#ifndef _MODELHANDLER_H_
#define _MODELHANDLER_H_ 1

/* This class will provide THE place for models.
 * New models will be created through this class
 * Existing objects will be references to data
 * stored in this class.
 * This class should also take care of recycling unused
 * models or unloading them after a certain timeout value
 * This is the only class that shoud know about the specific 
 * sub classes of MultiModel and Model
 */ 

#include <map>
#include <string>

namespace Sear {

// Forward Declarationa
class Model;
class ModelLoader;
class WorldEntity;
	
class ModelHandler {
public:
  ModelHandler();
  ~ModelHandler();

  void init();
  void shutdown();
  
  /*
   * Obtains a model for the given WorldEntity we
   */ 
  Model *getModel(WorldEntity *we);

  /*
   * Sets up a callback to use to load a model with model_type
   */ 
  void registerModelLoader(const std::string &model_type, ModelLoader *model_loader);

  void checkModelTimeout(const std::string &);

  void lowerDetail();
  void raiseDetail();
  
protected:
  float detail_level;
  unsigned int _number_of_models; //required?
  // Will not be an array as not dynamic -> a map would be better 
  std::map<std::string, ModelLoader*> _model_loaders;
  std::map<std::string, Model*> _models;
};

} /* namespace Sear */

#endif /* _MODELHANDLER_H_ */
