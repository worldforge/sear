// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.7 2003-07-17 16:41:01 simon Exp $

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
  _core_model_handler->shutdown();
  delete _core_model_handler;
}

ModelRecord *Cal3d_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {

  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  
  std::string file_name = System::instance()->getModel().getItem(CAL3D, model_record->data_file_id);
  
//  std::cerr << "Loading Cally model " << file_name << std::endl;

  Cal3dModel *model = _core_model_handler->instantiateModel(file_name);
  
  if (!model) {
    std::cerr << "Unable to load model" << std::endl;	  
    return NULL;
  }
//  std::cerr << "Assigning height" << std::endl << std::flush;
  // Set model height    
  float height = 1.0f;
  height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
  model->setHeight(height);

//  std::cerr << "Assigning texture set" << std::endl << std::flush;
  // Set model default texture set
  if (model_config.findItem(model_id, "default_set")) {
    varconf::Variable v = model_config.getItem(model_id, "default_set");
    if (v.is_int()) {
      model->setMaterialSet((int)v);
    } else { // Assume we have a string
      model->setMaterialSet((std::string)v);
    }
  }
  // Check for meshes
  
//  std::cerr << "Assigning parts" << std::endl << std::flush;
  // Check for individual part assignments
  std::list<std::string> materials = model->getMeshNames();
  for (std::list<std::string>::const_iterator I = materials.begin(); I != materials.end(); ++I) {
    if (model_config.findItem(model_id, *I)) {
      varconf::Variable v = model_config.getItem(model_id, *I);
//      if (v.is_int()) {
//        model->setMaterialPartSet(*I, (int)v);
//      } else {
        model->setMaterialPartSet(*I, (std::string)v);
//      }
    }
  }
  model_record->model = model;

 // std::cerr << "Model Created" << std::endl << std::flush;
  
  return model_record;
}

} /* namespace Sear */

