// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: WireFrame_Loader.cpp,v 1.19 2006-02-07 11:31:03 simon Exp $

#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"

#include "src/WorldEntity.h"

#include "Model.h"
#include "ModelHandler.h"
#include "ModelRecord.h"

#include "WireFrame_Loader.h"
#include "WireFrame.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
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

SPtr<ModelRecord> WireFrame_Loader::loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(render, we, model_id, model_config);
  WireFrame *model = new WireFrame(render);

  WFMath::AxisBox<3> bbox = we->getBBox();
  if (model->init(bbox)) {
  //if (!model->init(bboxCheck(bbox))) {
//    model->shutdown();
    delete model;
  //  return NULL;
    return SPtr<ModelRecord>();
  }
  model_record->model = SPtrShutdown<Model>(model);
  return model_record;
}

} /* namespace Sear */

