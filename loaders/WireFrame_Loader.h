// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: WireFrame_Loader.h,v 1.8 2005-01-06 12:46:54 simon Exp $

#ifndef SEAR_WIREFRAME_LOADER_H
#define SEAR_WIREFRAME_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {

class ModelHandler;
// Forward Declarations
	
class WireFrame_Loader : public ModelLoader {
public:	
  WireFrame_Loader(ModelHandler *mh);
  ~WireFrame_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);
protected:
  static const std::string WIREFRAME;
};

} /* namespace Sear */

#endif /* SEAR_WIREFRAME_LOADER_H */
