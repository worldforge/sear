// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: NPlane_Loader.cpp,v 1.12 2002-09-26 20:23:03 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "NPlane_Loader.h"
#include "NPlane.h"

namespace Sear {

const std::string NPlane_Loader::NPLANE = "nplane";
	
NPlane_Loader::NPlane_Loader(ModelHandler *mh) {
  mh->registerModelLoader(NPLANE, this);
}

NPlane_Loader::~NPlane_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *NPlane_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
//  ModelRecord *model_record = new ModelRecord();
//  model_record->scale = (double)model_config->getItem(model_id, ModelRecord::SCALE);
//  model_record->state = model_config->getItem(model_id, ModelRecord::STATE);
//  model_record->select_state = model_config->getItem(model_id, ModelRecord::SELECT_STATE);
//  model_record->model_by_type = model_config->getItem(model_id, ModelRecord::MODEL_BY_TYPE);
//  model_record->outline =  model_config->getItem(model_id, ModelRecord::OUTLINE);
  NPlane *model = new NPlane(render);

//  std::string type = ms.type;
//  int id = render->requestMipMap("nplane", type, true);
//  if (id == -1 && ms.parent) {
//    type = ms.parent;
//    id = render->requestMipMap("nplane", type, true);
//  }
//  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

//  }
  std::string type = record->type;
  float width =  (double)model_config->getItem(model_id, "width");
  float height =  (double)model_config->getItem(model_id, "height");
  int num_planes = (int)model_config->getItem(model_id, "num_planes");
  if (!model->init(type, num_planes, width, height)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

