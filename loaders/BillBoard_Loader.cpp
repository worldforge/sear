// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "src/System.h"
#include <string>

#include "src/ModelHandler.h"
#include "src/WorldEntity.h"
#include "src/Graphics.h"
#include "src/Render.h"
#include "src/ObjectLoader.h"

#include "BillBoard_Loader.h"
#include "BillBoard.h"

namespace Sear {

BillBoard_Loader::BillBoard_Loader(ModelHandler *mh) {
  mh->registerModelLoader("billboard", this);
}

BillBoard_Loader::~BillBoard_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *BillBoard_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  BillBoard *model = new BillBoard();

  std::string type = we->type();
  int id = System::instance()->getGraphics()->getRender()->requestTexture("billboard", type);
  if (id == -1) {
    type = we->parent();
    id = System::instance()->getGraphics()->getRender()->requestTexture("billboard", type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(type, op->width, op->height);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

