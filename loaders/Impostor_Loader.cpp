// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#include "src/System.h"
#include <string>

#include "src/Render.h"
#include "src/ModelHandler.h"

#include "Impostor_Loader.h"
#include "Impostor.h"

namespace Sear {

Impostor_Loader::Impostor_Loader(ModelHandler *mh) {
  mh->registerModelLoader("impostor", this);
}

Impostor_Loader::~Impostor_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *Impostor_Loader::loadModel(Render *render, ModelStruct &ms) {
  Impostor *model = new Impostor(render);
  std::string type = ms.type;
  int id = render->requestTexture("impostor", type);
  if (id == -1 && ms.parent) {
    type = ms.parent;
    id = render->requestTexture("impostor", type);
  }
  if (id == -1) {
    // TODO: what happens if we still cannot find a texture?

  }
  model->init(type, ms.width, ms.height);
  return model;
}

} /* namespace Sear */

