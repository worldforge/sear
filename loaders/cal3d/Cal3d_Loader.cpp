// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.3 2003-03-06 21:04:14 simon Exp $

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
  
  std::cerr << "Loading Cally model " << file_name << std::endl;

  Cal3dModel *model = _core_model_handler->instantiateModel(file_name, model_config);
  
  if (!model) {
    std::cerr << "Unable to load model" << std::endl;	  
    return NULL;
  }
    
  float height = 1.0f;
  height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
  model->setHeight(height);
  model_record->model = model;
  return model_record;
}
} /* namespace Sear */

