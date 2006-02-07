// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: CoreModelHandler.h,v 1.7 2006-02-07 18:45:33 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_COREMODELHANDLER_H
#define SEAR_LOADERS_CAL3D_COREMODELHANDLER_H 1

#include <sigc++/trackable.h>
#include <string>
#include <map>

namespace Sear {

// Forward declarations
class Cal3dModel;
class Cal3dCoreModel;
	
class CoreModelHandler : public sigc::trackable {
public:
  CoreModelHandler();
  ~CoreModelHandler();

  int init();
  int shutdown();

  void loadCoreModel(const std::string &filename);
  Cal3dModel *instantiateModel(const std::string &model);
    
private:
  typedef std::map <std::string, Cal3dCoreModel*> CoreModelMap;
  bool m_initialised;
  CoreModelMap m_core_models;
  
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_COREMODELHANDLER_H */
