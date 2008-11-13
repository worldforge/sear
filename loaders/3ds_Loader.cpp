// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

#include <varconf/config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "src/WorldEntity.h"

#include "3ds_Loader.h"
#include "3ds.h"

#include "renderers/RenderSystem.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif

namespace Sear {

const std::string ThreeDS_Loader::THREEDS = "3ds";

SPtr<ModelRecord> ThreeDS_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  // Get basic model record
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

  assert(model_record);

  std::string file_name = model_record->data_file_path;

  System::instance()->getFileHandler()->getFilePath(file_name);

  // Create new ThreeDS model
  ThreeDS *model = new ThreeDS();

  // Load 3ds model
  if (model->init(file_name)) {
//    model->shutdown();
    std::cerr << "Error: Failed to load \"" << file_name << "\"" << std::endl;
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
