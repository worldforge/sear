// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: WireFrame_Loader.cpp,v 1.11 2002-10-21 20:12:04 simon Exp $

#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"

#include "src/Model.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "WireFrame_Loader.h"
#include "WireFrame.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string WireFrame_Loader::WIREFRAME = "wireframe";
	
WireFrame_Loader::WireFrame_Loader(ModelHandler *mh) {
  mh->registerModelLoader(WIREFRAME, this);
}

WireFrame_Loader::~WireFrame_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *WireFrame_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
//  ModelRecord *model_record = new ModelRecord();
//  model_record->scale = (double)model_config->getItem(model_id, ModelRecord::SCALE);
//  model_record->state = model_config->getItem(model_id, ModelRecord::STATE);
//  model_record->select_state = model_config->getItem(model_id, ModelRecord::SELECT_STATE);
//  model_record->model_by_type = model_config->getItem(model_id, ModelRecord::MODEL_BY_TYPE);
//  model_record->outline =  model_config->getItem(model_id, ModelRecord::OUTLINE);
  WireFrame *model = new WireFrame(render);

  WFMath::AxisBox<3> bbox = record->bbox;
  if (!model->init(bboxCheck(bbox))) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

