// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: NPlane_Loader.h,v 1.6 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_NPLANE_LOADER_H
#define SEAR_NPLANE_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {
class ModelHandler;

class NPlane_Loader : public ModelLoader {
public:	
  NPlane_Loader(ModelHandler *mh);
  ~NPlane_Loader();
  
  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);
protected:
  static const std::string NPLANE;  
};

} /* namespace Sear */

#endif /* SEAR_NPLANE_LOADER_H */
