// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "BoundBox_Loader.h"
#include "BoundBox.h"
#include "../src/ModelHandler.h"
#include "../src/WorldEntity.h"
#include "../src/System.h"
#include "../src/Render.h"
#include "../src/Utility.h"

#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

BoundBox_Loader::BoundBox_Loader(ModelHandler *mh) {
  mh->registerModelLoader("boundbox", this);
}

BoundBox_Loader::~BoundBox_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *BoundBox_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  BoundBox *model = new BoundBox();

  WFMath::AxisBox<3> bbox = we->getBBox();
  if (!we->hasBBox()) {
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  std::string type = we->type();
  int id = System::instance()->getRenderer()->requestTexture(std::string("boundbox_") + type);
  if (id == -1) {
    type = we->parent();
    id = System::instance()->getRenderer()->requestTexture(std::string("boundbox_") + type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(bboxCheck(bbox), std::string("boundbox_") + type, op->wrap_texture);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

