// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: WireFrame_Loader.cpp,v 1.15 2004-04-27 15:07:01 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <string>

#include <varconf/Config.h>

#include "common/Utility.h"

#include "src/Model.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

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

ModelRecord *WireFrame_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  WireFrame *model = new WireFrame(render);

  WFMath::AxisBox<3> bbox = record->bbox;
  if (!model->init(bbox)) {
  //if (!model->init(bboxCheck(bbox))) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

