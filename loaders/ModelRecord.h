// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ModelRecord.h,v 1.9 2006-05-06 13:50:22 simon Exp $

#ifndef SEAR_MODELRECORD_H
#define SEAR_MODELRECORD_H 1

#include <string>

#include "common/SPtr.h"

#include "renderers/RenderTypes.h"


namespace Sear {
  class Model;

class ModelRecord {
public:
  ModelRecord();
  virtual ~ModelRecord() throw() {}
 
  float scale;

  std::string id;
  int state;
  bool scale_bbox;
  std::string state_name;
  int select_state;
  std::string select_state_name;
  std::string model_loader;
  std::string data_file_path;
  std::string default_skin;
  bool model_by_type;
  bool outline;
  SPtrShutdown<Model> model;	
  RotationStyle rotation_style;
  float offset_x, offset_y, offset_z;
  float rotate_x, rotate_y, rotate_z;
  bool scaleByHeight;

  static const std::string SCALE;
  static const std::string SCALE_BBOX;
  static const std::string SCALE_HEIGHT;
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
  static const std::string DATA_FILE_PATH;
  static const std::string DEFAULT_SKIN;
};

} /* namespace Sear */

#endif /* SEAR_MODELRECORD_H */
