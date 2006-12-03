// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.31 2006-12-03 13:38:47 simon Exp $

#include <varconf/Config.h>

#include "src/System.h"

#include <string>

#include "ModelRecord.h"

#include "BoundBox_Loader.h"
#include "BoundBox.h"

#include "src/WorldEntity.h"

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
	
BoundBox_Loader::BoundBox_Loader() {
}

BoundBox_Loader::~BoundBox_Loader() {
}

SPtr<ModelRecord> BoundBox_Loader::loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config) {
  assert (we);
  SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);

  BoundBox *model = new BoundBox();

  WFMath::AxisBox<3> bbox = we->hasBBox() ? (we->getBBox()) : (WFMath::AxisBox<3>(WFMath::Point<3>(0.0f,0.0f,0.0f), WFMath::Point<3>(1.0f,1.0f,1.0f)));
 
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

  StaticObjectList &sol = model->getStaticObjects();
  StaticObjectList::iterator I = sol.begin();
  while (I != sol.end()) {
    (*I)->setState(model_record->state);
    (*I)->setSelectState(model_record->select_state);
    ++I;
  }

  model_record->model = SPtrShutdown<Model>(model);

  return model_record;
}

} /* namespace Sear */

