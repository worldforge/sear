// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 - 2006 Simon Goodall

// $Id: ModelLoader.h,v 1.7 2006-02-07 11:31:03 simon Exp $

#ifndef SEAR_MODELOADER_H
#define SEAR_MODELOADER_H 1

#include <string>

#include "common/SPtr.h"

namespace varconf {
  class Config;
}

namespace Sear {
  class ModelRecord;
  class ModelHandler;
  class Render;
  class WorldEntity;
  	
class ModelLoader {
public:
  ModelLoader() {}
  virtual ~ModelLoader() {}

  virtual SPtr<ModelRecord> loadModel(Render *, WorldEntity *we, const std::string &model_id, varconf::Config &model_config);

};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
