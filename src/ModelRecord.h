// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: ModelRecord.h,v 1.3 2002-12-11 22:19:26 simon Exp $

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
    state("default"),
    select_state("select"),
    model_by_type(false),
    rotation_style(Graphics::ROS_NONE)
  {}
    
  ~ModelRecord() {}
 
  float scale;
  float offset[3];
  std::string id;
  std::string state;
  std::string select_state;
  std::string model_loader;
  std::string data_file_id;
  std::string default_skin;
  bool model_by_type;
  bool outline;
  Model *model;	
  Graphics::RotationStyle rotation_style;

  static const char * const SCALE = "scale";
  static const char * const STATE = "state";
  static const char * const SELECT_STATE = "select_state";
  static const char * const MODEL_BY_TYPE = "model_by_type";
  static const char * const MODEL_LOADER = "model_loader";
  static const char * const OUTLINE = "outline";
  static const char * const ROTATION_STYLE = "rotation_style";
  static const char * const DATA_FILE_ID = "data_file_id";
  static const char * const DEFAULT_SKIN = "default_skin";
};

} /* namespace Sear */

#endif /* SEAR_MODELRECORD_H */
