// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.13 2005-03-15 17:55:04 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "Cal3d_Loader.h"
#include "Cal3dModel.h"
#include "CoreModelHandler.h"


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
  m_core_model_handler = new CoreModelHandler();
  m_core_model_handler->init();
}

Cal3d_Loader::~Cal3d_Loader() {
  m_core_model_handler->shutdown();
  delete m_core_model_handler;
}

ModelRecord *Cal3d_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {

  ModelRecord *model_record =
                ModelLoader::loadModel(render, record, model_id, model_config);

  assert(model_record);

  if (!ModelSystem::getInstance().getModels().findItem(CAL3D, model_record->data_file_id)) {
    std::cerr << "Error: No Cal3D filename" << std::endl;
    return NULL;
  }

  std::string file_name = ModelSystem::getInstance().getModels().getItem(CAL3D, model_record->data_file_id);
  // Expand variable in string
  System::instance()->getFileHandler()->expandString(file_name);
 
  try {
    Cal3dModel *model = m_core_model_handler->instantiateModel(file_name);
  
    if (!model) {
      std::cerr << "Unable to load model" << std::endl;	  
      return NULL;
    }
    // Set model height
    if (model_record->scaleByHeight) {
      float height = 1.0f;
      height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
      model->setHeight(height);
    }

    // Set model default texture set
    if (model_config.findItem(model_id, "default_set")) {
      varconf::Variable v = model_config.getItem(model_id, "default_set");
      if (v.is_int()) {
        model->setMaterialSet((int)v);
      } else { // Assume we have a string
        model->setMaterialSet((std::string)v);
      }
    }
    // Check for individual part assignments
    std::list<std::string> materials = model->getMeshNames();
    for (std::list<std::string>::const_iterator I = materials.begin();
                                                I != materials.end(); ++I) {
      if (model_config.findItem(model_id, *I)) {
        varconf::Variable v = model_config.getItem(model_id, *I);
        model->setMaterialPartSet(*I, (std::string)v);
      }
    }
    model_record->model = model;
  } catch (...) {
    std::cerr << "Cal3d_Loader: Unknown Exception" << std::endl;
    return NULL;
  }

  return model_record;
}

} /* namespace Sear */

