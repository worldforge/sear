// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "3ds_Loader.h"
#include "3ds.h"
#include "../src/ModelHandler.h"
#include "../src/WorldEntity.h"
#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

ThreeDS_Loader::ThreeDS_Loader(ModelHandler *mh) {
  mh->registerModelLoader("3ds", this);
}

ThreeDS_Loader::~ThreeDS_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *ThreeDS_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  ThreeDS *model = new ThreeDS();
  model->init(file_name);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

