// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall

// $Id: ModelRecord.h,v 1.10 2004-06-24 15:20:13 simon Exp $

#ifndef SEAR_MODELRECORD_H
#define SEAR_MODELRECORD_H 1

#include <string>
#include "Graphics.h"

namespace Sear {
  class Model;

class ModelRecord {
public:
  ModelRecord() :
    scale(1.0),
    state_name("default"),
    select_state_name("select"),
    model_by_type(false),
    rotation_style(Graphics::ROS_NONE),
    offset_x(0.0f), offset_y(0.0f), offset_z(0.0f),
    scaleByHeight(false)
  {}
    
  ~ModelRecord() {}
 
  float scale;
//  float offset[3];
  std::string id;
  int state;
  std::string state_name;
  int select_state;
  std::string select_state_name;
  std::string model_loader;
  std::string data_file_id;
  std::string default_skin;
  bool model_by_type;
  bool outline;
  Model *model;	
  Graphics::RotationStyle rotation_style;
  float offset_x, offset_y, offset_z;
  float rotate_x, rotate_y, rotate_z;
  bool scaleByHeight;

  static const std::string SCALE;
  static const std::string OFFSET_X;
  static const std::string OFFSET_Y;
  static const std::string OFFSET_Z;
  static const std::string ROTATE_X;
  static const std::string ROTATE_Y;
  static const std::string ROTATE_Z;
  static const std::string STATE;
  static const std::string SELECT_STATE;
  static const std::string MODEL_BY_TYPE;
  static const std::string MODEL_LOADER;
  static const std::string OUTLINE;
  static const std::string ROTATION_STYLE;
  static const std::string DATA_FILE_ID;
  static const std::string DEFAULT_SKIN;
};

} /* namespace Sear */

#endif /* SEAR_MODELRECORD_H */
