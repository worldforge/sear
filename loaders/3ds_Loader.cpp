// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: 3ds_Loader.cpp,v 1.10 2002-10-21 20:12:04 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"

#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "3ds_Loader.h"
#include "3ds.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string ThreeDS_Loader::THREEDS = "3ds";
	
ThreeDS_Loader::ThreeDS_Loader(ModelHandler *mh) {
  mh->registerModelLoader(THREEDS, this);
}

ThreeDS_Loader::~ThreeDS_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *ThreeDS_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
//  ModelRecord *model_record = new ModelRecord();
//  model_record->scale = (double)model_config->getItem(model_id, ModelRecord::SCALE);
//  model_record->state = model_config->getItem(model_id, ModelRecord::STATE);
//  model_record->select_state = model_config->getItem(model_id, ModelRecord::SELECT_STATE);
//  model_record->model_by_type = model_config->getItem(model_id, ModelRecord::MODEL_BY_TYPE);
//  model_record->outline =  model_config->getItem(model_id, ModelRecord::OUTLINE);
  std::string file_name = System::instance()->getModel()->getItem(THREEDS, model_record->data_file_id);
  ThreeDS *model = new ThreeDS(render);
  if (!model->init(file_name)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  
  model->setInUse(true);
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

