// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Slice_Loader.cpp,v 1.9 2002-11-12 23:59:22 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "Slice_Loader.h"
#include "Slice.h"
#ifdef DEBUG
#include "common/mmgr.h"
static const bool debug = true;
#else
static const bool debug = false;
#endif

namespace Sear {

const std::string Slice_Loader::SLICE = "slice";
	
Slice_Loader::Slice_Loader(ModelHandler *mh) {
  mh->registerModelLoader(SLICE, this);
}

Slice_Loader::~Slice_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *Slice_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  Slice *model = new Slice(render);

//  std::string type = ms.type;
//  int id = render->requestMipMap(SLICE, type + "_0_0", true);
//  if (id == -1 && ms.parent) {
//    type = ms.parent;
//    id = render->requestMipMap(SLICE, type + "_0_0", true);
//  }
//  if (id == -1) {
//    // TODO: what happens if we still cannot find a texture?
//
//  }
  std::string type = record->type;
  float width =  (double)model_config.getItem(model_id, "width");
  float height =  (double)model_config.getItem(model_id, "height");
  int num_slices = (int)model_config.getItem(model_id, "num_slices");
  int slices_per_slicing = (int)model_config.getItem(model_id, "sliced_per_slicing");
  if (!model->init(type, width, height, num_slices, slices_per_slicing)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

