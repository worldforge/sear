// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: 3ds_Loader.cpp,v 1.7 2002-09-07 23:27:05 simon Exp $

#include "src/System.h"

#include "src/ModelHandler.h"

#include "3ds_Loader.h"
#include "3ds.h"

namespace Sear {

ThreeDS_Loader::ThreeDS_Loader(ModelHandler *mh) {
  mh->registerModelLoader("3ds", this);
}

ThreeDS_Loader::~ThreeDS_Loader() {
  // TODO: Add ability to unregister loader.
}

Model *ThreeDS_Loader::loadModel(Render *render, ModelStruct &ms) {
  ThreeDS *model = new ThreeDS(render);
  model->init(ms.file_name);
  model->setInUse(true);
  return model;
}

} /* namespace Sear */

