// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Cal3d_Loader.h,v 1.2 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_CAL3D_LOADER_H
#define SEAR_CAL3D_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {
// Forward Declarations
class Model;
class ModelHandler;
class CoreModelHandler;

class Cal3d_Loader : public ModelLoader {
public:	
  Cal3d_Loader(ModelHandler *mh);
  ~Cal3d_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);

private:
  static const std::string CAL3D;
  CoreModelHandler *_core_model_handler;
  
};

} /* namespace Sear */

#endif /* SEAR_CAL3D_LOADER_H */
