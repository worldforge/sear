// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelHandler.h,v 1.9 2002-10-21 22:24:29 simon Exp $

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
	
class ModelHandler {
public:
  ModelHandler();
  ~ModelHandler();

  void init();
  void shutdown();
  
  ModelRecord *getModel(Render *render, ObjectRecord *record, const std::string &model_id);

  void registerModelLoader(const std::string &model_type, ModelLoader *model_loader);
  void unregisterModelLoader(const std::string &model_type, ModelLoader *model_loader);

  void checkModelTimeouts();
  void checkModelTimeout(const std::string &);
  
protected:
  typedef std::map<std::string, ModelLoader*> ModelLoaderMap;
  typedef std::map<std::string, ModelRecord*> ModelRecordMap; 
  typedef std::map<std::string, ModelRecord*> ObjectRecordMap;
  ModelLoaderMap _model_loaders; // Stores all the model loaders
  ModelRecordMap _model_records; // Stores all the model_by_type models
  ObjectRecordMap _object_map; // Stores model for entity id and model_name
  bool _initialised;
};

} /* namespace Sear */

#endif /* SEAR_MODELHANDLER_H */
