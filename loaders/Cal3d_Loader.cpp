// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.cpp,v 1.15 2002-12-11 22:19:26 simon Exp $

#include <varconf/Config.h>

#include "src/ModelHandler.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

#include "Cal3d_Loader.h"
#include "Cal3d.h"

#ifdef DEBUG
  #include "common/mmgr.h"
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string Cal3d_Loader::CAL3D = "cal3d";
	
Cal3d_Loader::Cal3d_Loader(ModelHandler *mh) {
  mh->registerModelLoader(CAL3D, this);
}

Cal3d_Loader::~Cal3d_Loader() {
  // TODO: Add ability to unregister loader.
}
ModelRecord *Cal3d_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  Cal3d *model = new Cal3d(render);
  float height = 1.0f;
//  if (ms.hasBBox) {
    height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
//  }
  std::string file_name = System::instance()->getModel().getItem(CAL3D, model_record->data_file_id);
  std::string default_skin = System::instance()->getModel().getItem(CAL3D, model_record->default_skin);
  
  if (!model->init(file_name, height, default_skin)) {
    model->shutdown();
    delete (model);
    return NULL;
  }
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

