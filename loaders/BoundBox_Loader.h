// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _BOUNDBOX_LOADER_H_
#define _BOUNDBOX_LOADER_H_ 1

#include "../src/ModelLoader.h"

#include "../src/ObjectLoader.h"

#include <string>

namespace Sear {

// Forward Declarations
class Models;
class ModelHandler;
//class ObjectProperties;
	
class BoundBox_Loader : public ModelLoader {
public:	
  BoundBox_Loader(ModelHandler *mh);
  ~BoundBox_Loader();

  Models *loadModel(WorldEntity *we, ObjectProperties *op, const std::string &file_name);

};

} /* namespace Sear */

#endif /* _BOUNDBOX_LOADER_H_ */
