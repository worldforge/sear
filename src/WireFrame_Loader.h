// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _WIREFRAME_LOADER_H_
#define _WIREFRAME_LOADER_H_ 1

#include "ModelLoader.h"

#include <string>

namespace Sear {

// Forward Declarations
class Models;
class ModelHandler;
	
class WireFrame_Loader : public ModelLoader {
public:	
  WireFrame_Loader(ModelHandler *mh);
  ~WireFrame_Loader();

  Models *loadModel(WorldEntity *we, const std::string &file_name);

};

} /* namespace Sear */

#endif /* _WIREFRAME_LOADER_H_ */
