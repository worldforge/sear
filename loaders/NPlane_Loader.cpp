// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: NPlane_Loader.cpp,v 1.22 2006-02-07 11:31:03 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "renderers/Render.h"
#include "ModelHandler.h"
#include "ModelRecord.h"
#include "src/WorldEntity.h"

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
const std::string KEY_texture = "texture";
	
NPlane_Loader::NPlane_Loader(ModelHandler *mh) {
  mh->registerModelLoader(NPLANE, this);
}

NPlane_Loader::~NPlane_Loader() {
  // TODO: Add ability to unregister loader.
}

SPtr<ModelRecord> NPlane_Loader::loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(render, we, model_id, model_config);

  // Check that required fields exist
  if (!model_config.findItem(model_id, KEY_width)
    || !model_config.findItem(model_id, KEY_height)
    || !model_config.findItem(model_id, KEY_num_planes)
//    || !model_config.findItem(model_id, KEY_texture)
  ) {
    std::cerr << "Error: Required fields missing for NPlane" << std::endl;
    return SPtr<ModelRecord>();
  }
  // Create model instance
  NPlane *model = new NPlane(render);
  // Read config variables
  float width =  (double)model_config.getItem(model_id, KEY_width);
  float height =  (double)model_config.getItem(model_id, KEY_height);
  int num_planes = (int)model_config.getItem(model_id, KEY_num_planes);
  std::string tex = we->type();
  if (model_config.find(model_id, KEY_texture)) {
    tex = (std::string)model_config.getItem(model_id, KEY_texture);
  }
  // Initialise model
  if (model->init(tex, num_planes, width, height)) {
    delete model;
    return SPtr<ModelRecord>();
  }

  model_record->model = SPtrShutdown<Model>(model);
  return model_record;
}

} /* namespace Sear */

