// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "WireFrame_Loader.h"
#include "WireFrame.h"
#include "../src/ModelHandler.h"
#include "../src/WorldEntity.h"
#include "../src/GL_Render.h"
#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

WireFrame_Loader::WireFrame_Loader(ModelHandler *mh) {
  mh->registerModelLoader("wireframe", this);
}

WireFrame_Loader::~WireFrame_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *WireFrame_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  WireFrame *model = new WireFrame();

  WFMath::AxisBox<3> bbox = we->getBBox();
  if (!we->hasBBox()) {
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  model->init(GL_Render::bboxCheck(bbox));
  return model;
}

} /* namespace Sear */

