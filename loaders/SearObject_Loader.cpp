// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

// $Id: SearObject_Loader.cpp,v 1.1 2007-01-24 09:52:55 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include "src/FileHandler.h"

#include "ModelSystem.h"

#include "ModelRecord.h"
#include "src/WorldEntity.h"

#include "renderers/RenderSystem.h"

#include "SearObject_Loader.h"
#include "SearObject.h"

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string SearObject_Loader::SEAROBJECT = "searobj";
	
SearObject_Loader::SearObject_Loader() {
}

SearObject_Loader::~SearObject_Loader() {
}

SPtr<ModelRecord> SearObject_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

  assert(model_record);

  std::string file_name = model_record->data_file_path;

  System::instance()->getFileHandler()->getFilePath(file_name);

  if (debug) printf("[SearObject_Loader] Loading %s\n", file_name.c_str());

  SearObject *model = new SearObject();
  if (model->init(file_name)) {
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
 
  model_record->model = SPtrShutdown<Model>(model);
  return model_record;
}

} /* namespace Sear */

