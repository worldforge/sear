// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.24 2005-01-06 12:46:54 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include "src/System.h"


#include <string>

#include "common/Utility.h"

#include "renderers/Graphics.h"
#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "BoundBox_Loader.h"
#include "BoundBox.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string BoundBox_Loader::BOUNDBOX = "boundbox";

// Config keys
const std::string KEY_wrap_texture = "wrap_texture";
	
BoundBox_Loader::BoundBox_Loader(ModelHandler *mh) {
  mh->registerModelLoader(BOUNDBOX, this);
}

BoundBox_Loader::~BoundBox_Loader() {
//  mh->unregisterModelLoader("boundbox", this);
  // TODO: Add ability to unregister loader.
}

ModelRecord *BoundBox_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  BoundBox *model = new BoundBox(render);

  WFMath::AxisBox<3> bbox = record->bbox;
//  if (!ms.hasBBox) {
//    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
//    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
//    bbox = WFMath::AxisBox<3>(lc, hc);
//  }
  // BUG: FIXME boundbox has a slash and no slash?
  // 
//  int id = render->requestTexture("boundbox", type);
//  if (id == -1 && ms.parent) {
//    type = ms.parent;
//    id = render->requestTexture("boundbox_", type);
//  }
//  if (id == -1) {
//    // TODO: what happens if we still cannot find a texture?
//
//  }
 
  std::string type = record->type;
  bool wrap = false; //default to false
  // Check whether we specify texture wrapping
  if (model_config.findItem(model_id, KEY_wrap_texture)) {
    wrap = (bool)model_config.getItem(model_id, KEY_wrap_texture);
  }
  // Initialise model
  if (!model->init(bbox, type, wrap)) {
    std::cerr<< "error initing model" << std::endl;
    model->shutdown();
    delete model;
    return NULL;
  }
  model->setInUse(true);
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

