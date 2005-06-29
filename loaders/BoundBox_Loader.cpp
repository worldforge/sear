// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.27 2005-06-29 21:19:41 simon Exp $

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
const std::string KEY_texture = "texture";
	
BoundBox_Loader::BoundBox_Loader(ModelHandler *mh) {
  mh->registerModelLoader(BOUNDBOX, this);
}

BoundBox_Loader::~BoundBox_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *BoundBox_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  BoundBox *model = new BoundBox(render);

  WFMath::AxisBox<3> bbox = record->bbox;
 
  std::string texture = record->type;
  bool wrap = false; //default to false

  // Check whether we specify texture wrapping
  if (model_config.findItem(model_id, KEY_texture)) {
    texture = (std::string)model_config.getItem(model_id, KEY_texture);
  }
  // Get texture name
  if (model_config.findItem(model_id, KEY_wrap_texture)) {
    wrap = (bool)model_config.getItem(model_id, KEY_wrap_texture);
  }

  // Initialise model
  if (model->init(bbox, texture, wrap)) {
    std::cerr<< "BoundBoxLoader: Error initialising model" << std::endl;
    delete model;
    return NULL;
  }

  model->setInUse(true);
  model_record->model = model;

  return model_record;
}

} /* namespace Sear */

