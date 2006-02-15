// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 - 2006 Simon Goodall

// $Id: ModelLoader.h,v 1.8 2006-02-15 09:50:31 simon Exp $

#ifndef SEAR_MODELOADER_H
#define SEAR_MODELOADER_H 1

#include <string>

#include "common/SPtr.h"

namespace varconf {
  class Config;
}

namespace Sear {
  class ModelRecord;
  class WorldEntity;
  	
class ModelLoader {
public:
  ModelLoader() {}
  virtual ~ModelLoader() {}

  virtual std::string getType() const = 0;

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config);

};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
