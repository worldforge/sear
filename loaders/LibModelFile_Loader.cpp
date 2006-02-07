// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: LibModelFile_Loader.cpp,v 1.6 2006-02-07 11:31:03 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelHandler.h"
#include "ModelRecord.h"
#include "src/WorldEntity.h"

#include "LibModelFile_Loader.h"
#include "LibModelFile.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string LibModelFile_Loader::LIBMODELFILE = "md3";
	
LibModelFile_Loader::LibModelFile_Loader(ModelHandler *mh) {
  mh->registerModelLoader(LIBMODELFILE, this);
}

LibModelFile_Loader::~LibModelFile_Loader() {
  // TODO: Add ability to unregister loader.
}

SPtr<ModelRecord> LibModelFile_Loader::loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(render, we, model_id, model_config);

  assert(model_record);

  std::string file_name = model_record->data_file_path;

  if (file_name.empty()) {
    // Use old style path finder
    if (!ModelSystem::getInstance().getModels().findItem(LIBMODELFILE, model_record->data_file_id)) {
      std::cerr << "Error: No MD3 filename" << std::endl;
      return SPtr<ModelRecord>();
    }
    file_name = (std::string)ModelSystem::getInstance().getModels().getItem(LIBMODELFILE, model_record->data_file_id);
  }

  System::instance()->getFileHandler()->expandString(file_name);

  if (debug) printf("LibModelFile_Loader: Loading %s\n", file_name.c_str());

  LibModelFile *model = new LibModelFile(render);
  if (model->init(file_name)) {
//    model->shutdown();
    delete model;
    return SPtr<ModelRecord>();
  }
 
  model_record->model = SPtrShutdown<Model>(model);
  return model_record;
}

} /* namespace Sear */

