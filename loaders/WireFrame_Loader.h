// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef WIREFRAME_LOADER_H
#define WIREFRAME_LOADER_H 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class WireFrame_Loader : public ModelLoader {
public:	
  WireFrame_Loader(ModelHandler *mh);
  ~WireFrame_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* WIREFRAME_LOADER_H */
