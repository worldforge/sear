// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: NPlane_Loader.cpp,v 1.19 2005-03-15 17:55:03 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "renderers/Render.h"
#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "NPlane_Loader.h"
#include "NPlane.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string NPlane_Loader::NPLANE = "nplane";
// Config Keys
const std::string KEY_width = "width";
const std::string KEY_height = "height";
const std::string KEY_num_planes = "num_planes";
	
NPlane_Loader::NPlane_Loader(ModelHandler *mh) {
  mh->registerModelLoader(NPLANE, this);
}

NPlane_Loader::~NPlane_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *NPlane_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);

  // Check that required fields exist
  if (!model_config.findItem(model_id, KEY_width)
    || !model_config.findItem(model_id, KEY_height)
    || !model_config.findItem(model_id, KEY_num_planes)) {
    std::cerr << "Error: Required fields missing for NPlane" << std::endl;
    return NULL;
  }
  std::string type = record->type;
  // Create model instance
  NPlane *model = new NPlane(render);
  // Read config variables
  float width =  (double)model_config.getItem(model_id, KEY_width);
  float height =  (double)model_config.getItem(model_id, KEY_height);
  int num_planes = (int)model_config.getItem(model_id, KEY_num_planes);
  // Initialise model
  if (model->init(type, num_planes, width, height)) {
//    model->shutdown();
    delete model;
    return NULL;
  }

  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

