// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall

// $Id: LibModelFile_Loader.cpp,v 1.1 2005-02-21 14:16:46 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <varconf/Config.h>

#include "src/System.h"

#include "ModelHandler.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"

#include "LibModelFile_Loader.h"
#include "LibModelFile.h"


#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#ifdef DEBUG
  static const bool debug = true;
#else
  static const bool debug = false;
#endif
namespace Sear {

const std::string LibModelFile_Loader::LIBMODELFILE = "md3";
	
LibModelFile_Loader::LibModelFile_Loader(ModelHandler *mh) {
  mh->registerModelLoader(LIBMODELFILE, this);
}

LibModelFile_Loader::~LibModelFile_Loader() {
  // TODO: Add ability to unregister loader.
}

ModelRecord *LibModelFile_Loader::loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
  ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
  if (!System::instance()->getModel().findItem(LIBMODELFILE, model_record->data_file_id)) {
    std::cerr << "Error: No MD3 filename" << std::endl;
    return NULL;
  }
  std::string file_name = System::instance()->getModel().getItem(LIBMODELFILE, model_record->data_file_id);
  LibModelFile *model = new LibModelFile(render);
  if (!model->init(file_name)) {
    model->shutdown();
    delete model;
    return NULL;
  }
  if (model_record->scaleByHeight) {
    float height = 1.0f;
    height = fabs(record->bbox.highCorner().z() - record->bbox.lowCorner().z());
    model->setHeight(height);
  }
 
  model->setInUse(true);
  model_record->model = model;
  return model_record;
}

} /* namespace Sear */

