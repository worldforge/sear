// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include <string>

#include "src/ModelHandler.h"
#include "src/ObjectLoader.h"

#include "Cal3d_Loader.h"
#include "Cal3d.h"

namespace Sear {

Cal3d_Loader::Cal3d_Loader(ModelHandler *mh) {
  mh->registerModelLoader("cal3d", this);
}

Cal3d_Loader::~Cal3d_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *Cal3d_Loader::loadModel(WorldEntity*, ObjectProperties* op, const std::string &file_name) {
  Cal3d *model = new Cal3d();
  if (!model->init(file_name)) {
    model->shutdown();
    delete (model);
    return NULL;
  }
  model->setInUse(true);
  model->setFlag("outline", op->outline);
  return model;
}

} /* namespace Sear */

