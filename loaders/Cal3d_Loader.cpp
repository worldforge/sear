// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.10 2002-09-21 14:20:30 simon Exp $

#include "src/ModelHandler.h"

#include "Cal3d_Loader.h"
#include "Cal3d.h"

namespace Sear {

Cal3d_Loader::Cal3d_Loader(ModelHandler *mh) {
  mh->registerModelLoader("cal3d", this);
}

Cal3d_Loader::~Cal3d_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *Cal3d_Loader::loadModel(Render *render, ModelStruct &ms) {
  Cal3d *model = new Cal3d(render);
  float height = 1.0f;
  if (ms.hasBBox) {
    height = abs(ms.bbox.highCorner().z() - ms.bbox.lowCorner().z());
  }
  if (!model->init(ms.file_name, height)) {
    model->shutdown();
    delete (model);
    return NULL;
  }
  return model;
}

} /* namespace Sear */

