// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall, University of Southampton

// $Id: ModelHandler.h,v 1.7 2002-09-26 17:17:46 simon Exp $

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

  void checkModelTimeout(const std::string &);
  
protected:
  typedef std::map<std::string, ModelLoader*> ModelLoaderMap;
  typedef std::map<std::string, ModelRecord*> ModelRecordMap;
  typedef std::map<std::string, ModelRecord*> ObjectRecordMap;
  ModelLoaderMap _model_loaders;
  ModelRecordMap _model_records;
  ObjectRecordMap _object_map;
  bool _initialised;
};

} /* namespace Sear */

#endif /* SEAR_MODELHANDLER_H */
