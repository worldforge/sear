// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: 3ds_Loader.cpp,v 1.17 2005-03-15 17:55:03 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "3ds_Loader.h"
#include "3ds.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

const std::string ThreeDS_Loader::THREEDS = "3ds";
	
ThreeDS_Loader::ThreeDS_Loader(ModelHandler *mh) {
  mh->registerModelLoader(THREEDS, this);
}

ModelRecord *ThreeDS_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  // Get basic model record
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);

  // See if a filename has been specified
  if (!ModelSystem::getInstance().getModels().findItem(THREEDS, model_record->data_file_id)) {
    std::cerr << "Error: No 3DS filename" << std::endl;
    return NULL;
  }

  // Get 3ds filename
  std::string file_name = ModelSystem::getInstance().getModels().getItem(THREEDS, model_record->data_file_id);
  System::instance()->getFileHandler()->expandString(file_name);

  // Create new ThreeDS model
  ThreeDS *model = new ThreeDS(render);

  // Load 3ds model
  if (model->init(file_name)) {
//    model->shutdown();
    delete model;
    return NULL;
  }

  // Set height
  if (model_record->scaleByHeight) {
    float height = fabs(record->bbox.highCorner().z() 
                       - record->bbox.lowCorner().z());
    model->setHeight(height);
  }

  model->setInUse(true);
  model_record->model = model;
  // Return newly created model
  return model_record;
}

} /* namespace Sear */
