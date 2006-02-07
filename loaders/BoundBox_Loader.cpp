// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.28 2006-02-07 11:31:03 simon Exp $

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

#include "src/WorldEntity.h"

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

SPtr<ModelRecord> BoundBox_Loader::loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  assert (we);
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(render, we, model_id, model_config);

  BoundBox *model = new BoundBox(render);

  WFMath::AxisBox<3> bbox = we->getBBox();
 
  std::string texture = we->type();
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
    return SPtr<ModelRecord>();
  }

  model_record->model = SPtrShutdown<Model>(model);

  return model_record;
}

} /* namespace Sear */

