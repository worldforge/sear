// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "Cal3d_Loader.h"
#include "Cal3d.h"

#include "../src/ModelHandler.h"
#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

Cal3d_Loader::Cal3d_Loader(ModelHandler *mh) {
  mh->registerModelLoader("cal3d", this);
}

Cal3d_Loader::~Cal3d_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *Cal3d_Loader::loadModel(WorldEntity*, ObjectProperties*, const std::string &file_name) {
  Cal3d *model = new Cal3d();
  model->init(file_name);
  return model;
}

} /* namespace Sear */

