// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: ModelRecord.cpp,v 1.1 2003-05-02 20:24:55 simon Exp $

#ifdef HAVE_CONFIG
  #include "config.h"
#endif

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

#include "ModelRecord.h"

namespace Sear {
  const std::string ModelRecord::SCALE = "scale";
  const std::string ModelRecord::STATE = "state";
  const std::string ModelRecord::SELECT_STATE = "select_state";
  const std::string ModelRecord::MODEL_BY_TYPE = "model_by_type";
  const std::string ModelRecord::MODEL_LOADER = "model_loader";
  const std::string ModelRecord::OUTLINE = "outline";
  const std::string ModelRecord::ROTATION_STYLE = "rotation_style";
  const std::string ModelRecord::DATA_FILE_ID = "data_file_id";
  const std::string ModelRecord::DEFAULT_SKIN = "default_skin";
} /* namespace Sear */

