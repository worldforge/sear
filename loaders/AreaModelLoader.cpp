// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#include "AreaModelLoader.h"
#include "ModelHandler.h"
#include "AreaModel.h"
#include "ModelRecord.h"

namespace Sear
{

class AreaModel;

const char* AREA_MODEL = "area";

AreaModelLoader::AreaModelLoader(ModelHandler* mh)
{
    mh->registerModelLoader(AREA_MODEL, this);
}

AreaModelLoader::~AreaModelLoader()
{
}

ModelRecord* AreaModelLoader::loadModel(Render *render, 
    ObjectRecord *record,
    const std::string &model_id, 
    varconf::Config &model_config)
{
    ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);

  // Check that required fields exist
/*
  if (!model_config.findItem(model_id, KEY_width)
    || !model_config.findItem(model_id, KEY_height)
    || !model_config.findItem(model_id, KEY_num_planes)) {
    std::cerr << "Error: Required fields missing for NPlane" << std::endl;
    return NULL;
  }
 */
    
    // Create model instance
    AreaModel* amodel = new AreaModel(render, record);
  
  
    model_record->model = amodel;
    return model_record;
}

} // of namespace Sear
