// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _CAL3D_LOADER_H_
#define _CAL3D_LOADER_H_ 1

#include <string>

#include "../src/ModelLoader.h"
#include "../src/ObjectLoader.h"

namespace Sear {

// Forward Declarations
class Models;
class ModelHandler;

class Cal3d_Loader : public ModelLoader {
public:	
  Cal3d_Loader(ModelHandler *mh);
  ~Cal3d_Loader();

  Models *loadModel(WorldEntity *, ObjectProperties*, const std::string &file_name);

};

} /* namespace Sear */

#endif /* _CAL3D_LOADER_H_ */
