// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: NPlane_Loader.cpp,v 1.10 2002-09-08 16:15:01 simon Exp $

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"

#include "NPlane_Loader.h"
#include "NPlane.h"

namespace Sear {

NPlane_Loader::NPlane_Loader(ModelHandler *mh) {
  mh->registerModelLoader("nplane", this);
}

NPlane_Loader::~NPlane_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *NPlane_Loader::loadModel(Render *render, ModelStruct &ms) {
  NPlane *model = new NPlane(render);

  std::string type = ms.type;
  int id = render->requestMipMap("nplane", type, true);
  if (id == -1 && ms.parent) {
    type = ms.parent;
    id = render->requestMipMap("nplane", type, true);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  if (!model->init(type, ms.num_planes, ms.width, ms.height)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  return model;
}

} /* namespace Sear */

