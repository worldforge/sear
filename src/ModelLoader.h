// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ModelLoader.h,v 1.14 2002-11-12 23:59:22 simon Exp $

#ifndef SEAR_MODELOADER_H
#define SEAR_MODELOADER_H 1

#include <string>

#include <varconf/Config.h>

#include "src/Graphics.h"
#include "src/ModelRecord.h"
#include "src/ObjectRecord.h"

namespace varconf {
  class Config;
}

namespace Sear {
  class ModelRecord;
  class ModelHandler;
  class Render;
  class ObjectRecord;
  	
class ModelLoader {
public:
  ModelLoader() {}
  virtual ~ModelLoader() {}

//  virtual void registerModelLoader(ModelHandler *model_handler) = 0;
//  virtual void unregisterModelLoader(ModelHandler *model_handler) = 0;
  virtual ModelRecord *loadModel(Render *, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
    ModelRecord * model_record = new ModelRecord();
    model_record->id = model_id;
    model_record->scale = (double)model_config.getItem(model_id, ModelRecord::SCALE);
    model_record->state = model_config.getItem(model_id, ModelRecord::STATE);
    model_record->select_state = model_config.getItem(model_id, ModelRecord::SELECT_STATE);
    model_record->model_by_type = model_config.getItem(model_id, ModelRecord::MODEL_BY_TYPE);
    model_record->outline =  model_config.getItem(model_id, ModelRecord::OUTLINE);
    std::string rotation_style = model_config.getItem(model_id, ModelRecord::ROTATION_STYLE);
    if (rotation_style == "none") model_record->rotation_style = Graphics::ROS_NONE;
    else if (rotation_style == "normal") model_record->rotation_style = Graphics::ROS_NORMAL;
    else if (rotation_style == "position") model_record->rotation_style = Graphics::ROS_POSITION;
    else if (rotation_style == "billboard") model_record->rotation_style = Graphics::ROS_BILLBOARD;
    else if (rotation_style == "halo") model_record->rotation_style = Graphics::ROS_HALO;
    model_record->data_file_id = model_config.getItem(model_id, ModelRecord::DATA_FILE_ID);
    return model_record;
  }

};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
