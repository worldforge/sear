// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Slice_Loader.h,v 1.4 2002-11-12 23:59:22 simon Exp $

#ifndef SEAR_SLICE_LOADER_H
#define SEAR_SLICE_LOADER_H 1

#include "src/ModelLoader.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class Slice_Loader : public ModelLoader {
public:	
  Slice_Loader(ModelHandler *mh);
  ~Slice_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *recrod, const std::string &model_id, varconf::Config &model_config);

protected:
  static const std::string SLICE;
  
};

} /* namespace Sear */

#endif /* SEAR_SLICE_LOADER_H */
