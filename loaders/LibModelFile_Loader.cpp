// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: LibModelFile_Loader.cpp,v 1.10 2006-12-03 13:38:47 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelRecord.h"
#include "src/WorldEntity.h"

#include "LibModelFile_Loader.h"
#include "LibModelFile.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string LibModelFile_Loader::LIBMODELFILE = "md3";
	
LibModelFile_Loader::LibModelFile_Loader() {
}

LibModelFile_Loader::~LibModelFile_Loader() {
}

SPtr<ModelRecord> LibModelFile_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

  assert(model_record);

  std::string file_name = model_record->data_file_path;

  System::instance()->getFileHandler()->getFilePath(file_name);

  if (debug) printf("LibModelFile_Loader: Loading %s\n", file_name.c_str());

  LibModelFile *model = new LibModelFile();
  if (model->init(file_name)) {
//    model->shutdown();
    delete model;
    return SPtr<ModelRecord>();
  }


  StaticObjectList &sol = model->getStaticObjects();
  StaticObjectList::iterator I = sol.begin();
  while (I != sol.end()) {
    (*I)->setState(model_record->state);
    (*I)->setSelectState(model_record->select_state);
    ++I;
  }
 
  model_record->model = SPtrShutdown<Model>(model);
  return model_record;
}

} /* namespace Sear */

