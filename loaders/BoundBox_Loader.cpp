// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: BoundBox_Loader.cpp,v 1.13 2002-09-08 16:15:01 simon Exp $

#include "src/System.h"

#include <string>

#include "common/Utility.h"

#include "src/ModelHandler.h"

#include "BoundBox_Loader.h"
#include "BoundBox.h"

namespace Sear {

BoundBox_Loader::BoundBox_Loader(ModelHandler *mh) {
  mh->registerModelLoader("boundbox", this);
}

BoundBox_Loader::~BoundBox_Loader() {
//  mh->unregisterModelLoader("boundbox", this);
  // TODO: Add ability to unregister loader.
}

Model *BoundBox_Loader::loadModel(Render *render, ModelStruct &ms) {
  BoundBox *model = new BoundBox(render);

  WFMath::AxisBox<3> bbox = ms.bbox;
  if (!ms.hasBBox) {
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  std::string type = ms.type;
  int id = render->requestTexture("boundbox", type);
  if (id == -1 && ms.parent) {
    type = ms.parent;
    id = render->requestTexture("boundbox_", type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  if (!model->init(bboxCheck(bbox), type, ms.wrap_texture)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

