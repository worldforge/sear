// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: CoreModelHandler.h,v 1.4 2003-03-06 21:04:14 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_COREMODELHANDLER_H
#define SEAR_LOADERS_CAL3D_COREMODELHANDLER_H 1

#include <sigc++/object_slot.h>
#include <string>
#include <map>

namespace varconf {
  class Config;
}

namespace Sear {

// Forward declarations
class Cal3dModel;
class Cal3dCoreModel;
	
class CoreModelHandler :public SigC::Object {
public:
  CoreModelHandler();
  ~CoreModelHandler();

  void init();
  void shutdown();

  void loadCoreModel(const std::string &filename);
  Cal3dModel *instantiateModel(const std::string &model, varconf::Config &config);
    
private:
  bool _initialised;
  typedef std::map <std::string, Cal3dCoreModel*> CoreModelMap;
  CoreModelMap _core_models;
  
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_COREMODELHANDLER_H */
