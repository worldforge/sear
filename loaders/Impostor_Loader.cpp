// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "Impostor_Loader.h"
#include "Impostor.h"
#include "../src/ModelHandler.h"
#include "../src/WorldEntity.h"
#include "../src/System.h"
#include "../src/Render.h"
#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

Impostor_Loader::Impostor_Loader(ModelHandler *mh) {
  mh->registerModelLoader("impostor", this);
}

Impostor_Loader::~Impostor_Loader() {
  // TODO: Add ability to unregister loader.
}

Models *Impostor_Loader::loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name) {
  Impostor *model = new Impostor();

  std::string type = we->type();
  int id = System::instance()->getRenderer()->requestTexture(std::string("impostor_") + type);
  if (id == -1) {
    type = we->parent();
    id = System::instance()->getRenderer()->requestTexture(std::string("impostor_") + type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(std::string("impostor_") + type, op->width, op->height);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

