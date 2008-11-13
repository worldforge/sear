// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

#include <string>

#include <varconf/config.h>

#include "common/Utility.h"

#include "src/WorldEntity.h"

#include "Model.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "WireFrame_Loader.h"
#include "WireFrame.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string WireFrame_Loader::WIREFRAME = "wireframe";
	
WireFrame_Loader::WireFrame_Loader() {
}

WireFrame_Loader::~WireFrame_Loader() {
}

SPtr<ModelRecord> WireFrame_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);
  WireFrame *model = new WireFrame();

  const WFMath::AxisBox<3> &bbox = we->hasBBox() ? (we->getBBox()) : (WFMath::AxisBox<3>(WFMath::Point<3>(0.0f,0.0f,0.0f), WFMath::Point<3>(1.0f,1.0f,1.0f)));

  if (model->init(bbox)) {
    delete model;
    return SPtr<ModelRecord>();
  }
  model_record->model = SPtr<Model>(model);

  return model_record;
}

} /* namespace Sear */

