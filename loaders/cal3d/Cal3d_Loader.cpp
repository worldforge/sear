// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.1 2003-03-05 23:39:04 simon Exp $

#include <varconf/Config.h>

#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "Cal3d_Loader.h"
#include "Cal3dModel.h"
#include "CoreModelHandler.h"
//#include "CoreModelHandler.h"

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {
const std::string Cal3d_Loader::CAL3D = "cal3d";
	
Cal3d_Loader::Cal3d_Loader(ModelHandler *mh) {
  mh->registerModelLoader(CAL3D, this);
  _core_model_handler = new CoreModelHandler();
  _core_model_handler->init();
}

Cal3d_Loader::~Cal3d_Loader() {
}

ModelRecord *Cal3d_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  
  std::string file_name = System::instance()->getModel().getItem(CAL3D, model_record->data_file_id);
  Cal3dModel *model = _core_model_handler->instantiateModel(file_name);
//  float height = 1.0f;
//  if (ms.hasBBox) {
//    height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
//  }
//  std::string default_skin = System::instance()->getModel().getItem(CAL3D, model_record->default_skin);
  
//  if (!model->init()) {
//    model->shutdown();
//    delete (model);
//    return NULL;
//  }
  if (!model) return NULL;
  model_record->model = model;
  return model_record;
}
} /* namespace Sear */

