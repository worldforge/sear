// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.14 2002-09-26 17:17:46 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"


#include <string>

#include "common/Utility.h"

#include "src/Graphics.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "BoundBox_Loader.h"
#include "BoundBox.h"

namespace Sear {

const std::string BoundBox_Loader::BOUNDBOX = "boundbox";
	
BoundBox_Loader::BoundBox_Loader(ModelHandler *mh) {
  mh->registerModelLoader(BOUNDBOX, this);
}

BoundBox_Loader::~BoundBox_Loader() {
//  mh->unregisterModelLoader("boundbox", this);
  // TODO: Add ability to unregister loader.
}

ModelRecord *BoundBox_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
/*	  new ModelRecord();
  model_record->scale = (double)model_config->getItem(model_id, ModelRecord::SCALE);
  model_record->state = model_config->getItem(model_id, ModelRecord::STATE);
  model_record->select_state = model_config->getItem(model_id, ModelRecord::SELECT_STATE);
  model_record->model_by_type = model_config->getItem(model_id, ModelRecord::MODEL_BY_TYPE);
  model_record->outline =  model_config->getItem(model_id, ModelRecord::OUTLINE);
  std::string rotation_style = model_config->getItem(model_id, ModelRecord::ROTATION_STYLE);
  if (rotation_style == "none") model_record->rotation_style = Graphics::ROS_NONE;
  else if (rotation_style == "normal") model_record->rotation_style = Graphics::ROS_NORMAL;
  else if (rotation_style == "position") model_record->rotation_style = Graphics::ROS_POSITION;
  else if (rotation_style == "billboard") model_record->rotation_style = Graphics::ROS_BILLBOARD;
  else if (rotation_style == "halo") model_record->rotation_style = Graphics::ROS_HALO;
  */
  BoundBox *model = new BoundBox(render);

  WFMath::AxisBox<3> bbox = record->bbox;
//  if (!ms.hasBBox) {
//    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
//    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
//    bbox = WFMath::AxisBox<3>(lc, hc);
//  }
  std::string type = record->name;
  // BUG: FIXME boundbox has a slash and no slash?
  // 
//  int id = render->requestTexture("boundbox", type);
//  if (id == -1 && ms.parent) {
//    type = ms.parent;
//    id = render->requestTexture("boundbox_", type);
//  }
//  if (id == -1) {
//    // TODO: what happens if we still cannot find a texture?
//
//  }
  
  if (!model->init(bboxCheck(bbox), type, (bool)model_config->getItem(model_id, "wrap_texture"))) {
	  cerr<< "error initing model" << endl;
    model->shutdown();
    delete model;
    return NULL;
  }
  model->setInUse(true);
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

