// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: ModelRecord.cpp,v 1.9 2006-02-16 15:59:01 simon Exp $

#include "ModelRecord.h"
#include "Model.h"
#include "renderers/RenderSystem.h"

namespace Sear {
  const std::string ModelRecord::SCALE = "scale";
  const std::string ModelRecord::SCALE_BBOX = "scale_bbox";
  const std::string ModelRecord::SCALE_HEIGHT = "scale_height";
  const std::string ModelRecord::OFFSET_X = "offset_x";
  const std::string ModelRecord::OFFSET_Y = "offset_y";
  const std::string ModelRecord::OFFSET_Z = "offset_z";
  const std::string ModelRecord::ROTATE_X = "rotate_x";
  const std::string ModelRecord::ROTATE_Y = "rotate_y";
  const std::string ModelRecord::ROTATE_Z = "rotate_z";
  const std::string ModelRecord::STATE = "state";
  const std::string ModelRecord::SELECT_STATE = "select_state";
  const std::string ModelRecord::MODEL_BY_TYPE = "model_by_type";
  const std::string ModelRecord::MODEL_LOADER = "model_loader";
  const std::string ModelRecord::OUTLINE = "outline";
  const std::string ModelRecord::ROTATION_STYLE = "rotation_style";
  const std::string ModelRecord::DATA_FILE_ID = "data_file_id";
  const std::string ModelRecord::DATA_FILE_PATH = "data_file_path";
  const std::string ModelRecord::DEFAULT_SKIN = "default_skin";

ModelRecord::ModelRecord() :
    scale(1.0),
    scale_bbox(false),
    state_name("default"),
    select_state_name("select"),
    model_by_type(false),
    rotation_style(ROS_NONE),
    offset_x(0.0f), offset_y(0.0f), offset_z(0.0f),
    scaleByHeight(false)
  {
    // Initialise state numbers
    state = RenderSystem::getInstance().requestState(state_name);
    select_state = RenderSystem::getInstance().requestState(select_state_name);
  }


} /* namespace Sear */

