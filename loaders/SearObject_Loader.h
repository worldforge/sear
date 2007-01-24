// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2007 Simon Goodall

// $Id: SearObject_Loader.h,v 1.1 2007-01-24 09:52:55 simon Exp $

#ifndef SEAR_LOADERS_SEAROBJECT_LOADER_H
#define SEAR_LOADERS_SEAROBJECT_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {

// Forward Declarations
	
class SearObject_Loader : public ModelLoader {
public:	
  SearObject_Loader();
  virtual ~SearObject_Loader();

  virtual std::string getType() const { return SEAROBJECT; }

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
	  
protected:
  static const std::string SEAROBJECT;
};

} /* namespace Sear */

#endif /* SEAR_LOADERS_SEAROBJECT_LOADER_H */
