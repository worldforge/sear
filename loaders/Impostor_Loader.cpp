// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Impostor_Loader.cpp,v 1.15 2002-09-26 17:17:46 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "Impostor_Loader.h"
#include "Impostor.h"

namespace Sear {

const std::string Impostor_Loader::IMPOSTOR = "impostor";
	
Impostor_Loader::Impostor_Loader(ModelHandler *mh) {
  mh->registerModelLoader("impostor", this);
}

Impostor_Loader::~Impostor_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *Impostor_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
//  ModelRecord *model_record = new ModelRecord();
//  model_record->scale = (double)model_config->getItem(model_id, ModelRecord::SCALE);
//  model_record->state = model_config->getItem(model_id, ModelRecord::STATE);
//  model_record->select_state = model_config->getItem(model_id, ModelRecord::SELECT_STATE);
//  model_record->model_by_type = model_config->getItem(model_id, ModelRecord::MODEL_BY_TYPE);
//  model_record->outline =  model_config->getItem(model_id, ModelRecord::OUTLINE);
  Impostor *model = new Impostor(render);
//  std::string type = ms.type;
//  bool multi_textures = ms.multi_textures;
//  if (multi_textures) {
//    int id = render->requestTexture("impostor_front", type, true);
//    id = render->requestTexture("impostor_side", type, true);
//    if (id == -1 && ms.parent) {
//      type = ms.parent;
//      id = render->requestTexture("impostor_front", type, true);
//      id = render->requestTexture("impostor_side", type, true);
//    }
//    if (id == -1) {
//      // If nothing, then try a single texture version
//      multi_textures = false;
//    }
//  } 
//  if (!multi_textures) {
//    int id = render->requestTexture("impostor", type, true);
//    if (id == -1 && ms.parent) {
//      type = ms.parent;
 //     id = render->requestTexture("impostor", type, true);
//    }
//    if (id == -1) {
//      // TODO: what happens if we still cannot find a texture?
//    }
 // }
  std::string type = record->name;
  float width =  (double)model_config->getItem(model_id, "width");
  float height =  (double)model_config->getItem(model_id, "height");
  bool multi_textures = (bool)model_config->getItem(model_id, "multi_textures");
  if (!model->init(type, width, height, multi_textures)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

