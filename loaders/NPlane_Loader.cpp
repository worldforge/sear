// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall

#include <varconf/config.h>

#include "src/System.h"
#include <string>

#include "ObjectRecord.h"
#include "ModelRecord.h"
#include "src/WorldEntity.h"

#include "renderers/RenderSystem.h"

#include "NPlane_Loader.h"
#include "NPlane.h"

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
	
NPlane_Loader::NPlane_Loader() {
}

NPlane_Loader::~NPlane_Loader() {
}

SPtr<ModelRecord> NPlane_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

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
  NPlane *model = new NPlane();
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


  bool use_stencil = RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL) && model_record->outline;

  StaticObjectList &sol = model->getStaticObjects();
  StaticObjectList::iterator I = sol.begin();
  while (I != sol.end()) {
    (*I)->setState(model_record->state);
    (*I)->setSelectState(model_record->select_state);
    (*I)->setUseStencil(use_stencil);
    ++I;
  }

  model_record->model = SPtr<Model>(model);
  return model_record;
}

} /* namespace Sear */

