// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: NPlane_Loader.cpp,v 1.14 2002-11-12 23:59:22 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "NPlane_Loader.h"
#include "NPlane.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string NPlane_Loader::NPLANE = "nplane";
	
NPlane_Loader::NPlane_Loader(ModelHandler *mh) {
  mh->registerModelLoader(NPLANE, this);
}

NPlane_Loader::~NPlane_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *NPlane_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
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
  float width =  (double)model_config.getItem(model_id, "width");
  float height =  (double)model_config.getItem(model_id, "height");
  int num_planes = (int)model_config.getItem(model_id, "num_planes");
  if (!model->init(type, num_planes, width, height)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

