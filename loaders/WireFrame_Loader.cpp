// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: WireFrame_Loader.cpp,v 1.9 2002-09-08 16:15:01 simon Exp $

#include <string>

#include "common/Utility.h"

#include "src/Model.h"
#include "src/ModelHandler.h"

#include "WireFrame_Loader.h"
#include "WireFrame.h"

namespace Sear {

WireFrame_Loader::WireFrame_Loader(ModelHandler *mh) {
  mh->registerModelLoader("wireframe", this);
}

WireFrame_Loader::~WireFrame_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *WireFrame_Loader::loadModel(Render *render, ModelStruct &ms) {
  WireFrame *model = new WireFrame(render);

  WFMath::AxisBox<3> bbox = ms.bbox;
  if (!ms.hasBBox) {
    WFMath::Point<3> lc = WFMath::Point<3>(0.0f, 0.0f, 0.0f);
    WFMath::Point<3> hc = WFMath::Point<3>(1.0f, 1.0f, 1.0f);
    bbox = WFMath::AxisBox<3>(lc, hc);
  }
  if (!model->init(bboxCheck(bbox))) {
    model->shutdown();
    delete model;
    return NULL;
  }
  return model;
}

} /* namespace Sear */

