// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ModelRecord.h,v 1.7 2003-05-02 20:24:55 simon Exp $

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
    rotation_style(Graphics::ROS_NONE)
  {}
    
  ~ModelRecord() {}
 
  float scale;
  float offset[3];
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

  static const std::string SCALE;
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
