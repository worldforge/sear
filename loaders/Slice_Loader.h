// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef SEAR_SLICE_LOADER_H
#define SEAR_SLICE_LOADER_H 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class Slice_Loader : public ModelLoader {
public:	
  Slice_Loader(ModelHandler *mh);
  ~Slice_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* SEAR_SLICE_LOADER_H */
