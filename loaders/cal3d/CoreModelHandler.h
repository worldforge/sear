// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2003 Simon Goodall

// $Id: CoreModelHandler.h,v 1.1 2003-03-04 23:28:46 simon Exp $

#ifndef SEAR_LOADERS_CAL3D_COREMODELHANDLER_H
#define SEAR_LOADERS_CAL3D_COREMODELHANDLER_H 1

namespace Sear {

class CoreModelHandler :public SigC::Object {
public:
  CoreModelHandler();
  ~CoreModelHandler();

  void init();
  void shutdown();

  void loadCoreModel(const std::string &filename);
  Cal3dModel *instantiateModel(const std::string &model);
    
private:
  bool _initialised;
  typedef std::map <std::string, CalCoreModel> CoreModelMap;
  CoreModelMap _core_models;
  
};
	
} /* namespace Sear */

#endif /* SEAR_LOADERS_CAL3D_COREMODELHANDLER_H */
