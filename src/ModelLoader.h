// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: ModelLoader.h,v 1.27 2004-06-25 09:46:16 simon Exp $

#ifndef SEAR_MODELOADER_H
#define SEAR_MODELOADER_H 1

#include <string>

#include <varconf/Config.h>

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

  virtual ModelRecord *loadModel(Render *, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config) {
    ModelRecord * model_record = new ModelRecord();
    model_record->id = model_id;
    // Get model scale
    if (model_config.findItem(model_id, ModelRecord::SCALE)) {
      model_record->scale = (double)model_config.getItem(model_id, ModelRecord::SCALE);
    } else {
      model_record->scale = 1.0;
    }
   // Get model offset (x)
    if (model_config.findItem(model_id, ModelRecord::OFFSET_X)) {
      model_record->offset_x = (double)model_config.getItem(model_id, ModelRecord::OFFSET_X);
    } else {
      model_record->offset_x = 0.0;
    }
   // Get model offset (y)
    if (model_config.findItem(model_id, ModelRecord::OFFSET_Y)) {
      model_record->offset_y = (double)model_config.getItem(model_id, ModelRecord::OFFSET_Y);
    } else {
      model_record->offset_y = 0.0;
    }
    // Get model offset (z)
    if (model_config.findItem(model_id, ModelRecord::OFFSET_Z)) {
      model_record->offset_z = (double)model_config.getItem(model_id, ModelRecord::OFFSET_Z);
    } else {
      model_record->offset_z = 0.0;
    }

    // Get model rotation (x)
    if (model_config.findItem(model_id, ModelRecord::ROTATE_X)) {
      model_record->rotate_x = (double)model_config.getItem(model_id, ModelRecord::ROTATE_X);
    } else {
      model_record->rotate_x = 0.0;
    }
    // Get model rotation (y)
    if (model_config.findItem(model_id, ModelRecord::ROTATE_Y)) {
      model_record->rotate_y = (double)model_config.getItem(model_id, ModelRecord::ROTATE_Y);
    } else {
      model_record->rotate_y = 0.0;
    }
    // Get model rotation (z)
    if (model_config.findItem(model_id, ModelRecord::ROTATE_Z)) {
      model_record->rotate_z = (double)model_config.getItem(model_id, ModelRecord::ROTATE_Z);
    } else {
      model_record->rotate_z = 0.0;
    }
 


    // Get render state number
    if (model_config.findItem(model_id, "state_num")) {
      model_record->state = model_config.getItem(model_id, "state_num");
    } else {
      model_record->state = 0;
    }

    // Get render state name
    if (model_config.findItem(model_id, ModelRecord::STATE)) {
      model_record->state_name = (std::string)model_config.getItem(model_id, ModelRecord::STATE);
    } else {
      model_record->state_name = "default";
    }
    // Get render select state number
    if (model_config.findItem(model_id, "select_state_num")) {
      model_record->select_state = model_config.getItem(model_id, "select_state_num");
    } else {
      model_record->select_state = 0;
    }
    // Get render select state name
    if (model_config.findItem(model_id, ModelRecord::SELECT_STATE)) {
      model_record->select_state_name = (std::string)model_config.getItem(model_id, ModelRecord::SELECT_STATE);
    } else {
      model_record->select_state_name = "default";
    }
    // Get model type 
    if (model_config.findItem(model_id, ModelRecord::MODEL_BY_TYPE)) {
      model_record->model_by_type = model_config.getItem(model_id, ModelRecord::MODEL_BY_TYPE);
    } else {
      model_record->model_by_type = false;
    }
    // Get preferred outline method 
    if (model_config.findItem(model_id, ModelRecord::OUTLINE)) {
      model_record->outline = model_config.getItem(model_id, ModelRecord::OUTLINE);
    } else  {
      model_record->outline = false;
    }
    // Get preferred skin
    if (model_config.findItem(model_id, ModelRecord::DEFAULT_SKIN)) {
      model_record->default_skin = (std::string)model_config.getItem(model_id, ModelRecord::DEFAULT_SKIN);
    } else {
      model_record->default_skin = "";
    }
    // Get rotation style
    std::string rotation_style = "none";
    if (model_config.findItem(model_id, ModelRecord::ROTATION_STYLE)) {
      rotation_style = (std::string)model_config.getItem(model_id, ModelRecord::ROTATION_STYLE);
    }
    // Object doesn't rotate at all
    if (rotation_style == "none") model_record->rotation_style = Graphics::ROS_NONE;
    // Object is rotated to its specified orientation
    else if (rotation_style == "normal") model_record->rotation_style = Graphics::ROS_NORMAL;
    // Object is rotated based on position
    else if (rotation_style == "position") model_record->rotation_style = Graphics::ROS_POSITION;
    // Object is rotates to face the camera in x,y plane
    else if (rotation_style == "billboard") model_record->rotation_style = Graphics::ROS_BILLBOARD;
    // Object is rotates to face the camera in x,y,z plane
    else if (rotation_style == "halo") model_record->rotation_style = Graphics::ROS_HALO;
    if (model_config.findItem(model_id, ModelRecord::DATA_FILE_ID)) {
      model_record->data_file_id = (std::string)model_config.getItem(model_id, ModelRecord::DATA_FILE_ID);
    }
    if (model_config.findItem(model_id, "scale_height")) {
      model_record->scaleByHeight = (bool)model_config.getItem(model_id, "scale_height");
    }
    return model_record;
  }

};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
