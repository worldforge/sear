// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "src/System.h"
#include <string>

#include "src/WorldEntity.h"
#include "src/Render.h"
#include "src/ObjectLoader.h"
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

Model *NPlane_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  NPlane *model = new NPlane();

  std::string type = we->type();
  int id = System::instance()->getGraphics()->getRender()->requestTexture("nplane", type);
  if (id == -1) {
    type = we->parent();
    id = System::instance()->getGraphics()->getRender()->requestTexture("nplane", type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(type, op->num_planes, op->width, op->height);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

