// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelHandler.h,v 1.1 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_MODELHANDLER_H
#define SEAR_MODELHANDLER_H 1

#include <map>
#include <string>

namespace Sear {

// Forward Declarationa
class ModelLoader;
class ModelRecord;
class ObjectRecord;
class Render;
class WorldEntity;
	
class ModelHandler {
public:
  ModelHandler();
  ~ModelHandler();

  void init();
  void shutdown();
  
  ModelRecord *getModel(Render *render, ObjectRecord *record, const std::string &model_id, WorldEntity *we = NULL);

  void registerModelLoader(const std::string &model_type, ModelLoader *model_loader);
  void unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader);

  void checkModelTimeouts();
  
protected:
  typedef std::map<std::string, ModelLoader*> ModelLoaderMap;
  typedef std::map<std::string, ModelRecord*> ModelRecordMap; 
  typedef std::map<std::string, ModelRecord*> ObjectRecordMap;
  ModelLoaderMap m_model_loaders; // Stores all the model loaders
  ModelRecordMap m_model_records; // Stores all the model_by_type models
  ObjectRecordMap m_object_map; // Stores model for entity id and model_name
  bool m_initialised;
};

} /* namespace Sear */

#endif /* SEAR_MODELHANDLER_H */
