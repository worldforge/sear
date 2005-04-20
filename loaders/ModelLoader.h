// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: ModelLoader.h,v 1.5 2005-04-20 08:25:04 jmt Exp $

#ifndef SEAR_MODELOADER_H
#define SEAR_MODELOADER_H 1

#include <string>

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

  virtual ModelRecord *loadModel(Render *, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);

};

} /* namespace Sear */

#endif /* SEAR_MODELLOADER_H */
