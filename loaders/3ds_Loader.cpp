// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: 3ds_Loader.cpp,v 1.24 2006-05-06 13:50:22 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelRecord.h"
#include "src/WorldEntity.h"

#include "3ds_Loader.h"
#include "3ds.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

const std::string ThreeDS_Loader::THREEDS = "3ds";

SPtr<ModelRecord> ThreeDS_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  // Get basic model record
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

  assert(model_record);

  std::string file_name = model_record->data_file_path;

  System::instance()->getFileHandler()->expandString(file_name);

  // Create new ThreeDS model
  ThreeDS *model = new ThreeDS();

  // Load 3ds model
  if (model->init(file_name)) {
//    model->shutdown();
    std::cerr << "Error: Failed to load \"" << file_name << "\"" << std::endl;
    delete model;
    return SPtr<ModelRecord>();
  }

  // Set height
//  if (model_record->scaleByHeight) {
//    float height = fabs(we->getBBox().highCorner().z() 
//                       - we->getBBox().lowCorner().z());
//    model->setHeight(height);
//  }

  model_record->model = SPtrShutdown<Model>(model);
  // Return newly created model
  return model_record;
}

} /* namespace Sear */
