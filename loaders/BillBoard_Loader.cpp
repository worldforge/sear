// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "BillBoard_Loader.h"
#include "BillBoard.h"
#include "../src/ModelHandler.h"
#include "../src/WorldEntity.h"
#include "../renderers/GL.h"
#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

BillBoard_Loader::BillBoard_Loader(ModelHandler *mh) {
  mh->registerModelLoader("billboard", this);
}

BillBoard_Loader::~BillBoard_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *BillBoard_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  BillBoard *model = new BillBoard();

  std::string type = we->type();
  int id = GL::instance()->requestTexture(std::string("billboard_") + type);
  if (id == -1) {
    type = we->parent();
    id = GL::instance()->requestTexture(std::string("billboard_") + type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(std::string("billboard_") + type, op->width, op->height);
  return model;
}

} /* namespace Sear */

