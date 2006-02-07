// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: NPlane_Loader.h,v 1.8 2006-02-07 11:31:03 simon Exp $

#ifndef SEAR_NPLANE_LOADER_H
#define SEAR_NPLANE_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {
class ModelHandler;

class NPlane_Loader : public ModelLoader {
public:	
  NPlane_Loader(ModelHandler *mh);
  ~NPlane_Loader();
  
  SPtr<ModelRecord> loadModel(Render *render, WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
protected:
  static const std::string NPLANE;  
};

} /* namespace Sear */

#endif /* SEAR_NPLANE_LOADER_H */
