// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _IMPOSTOR_LOADER_H_
#define _IMPOSTOR_LOADER_H_ 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class Impostor_Loader : public ModelLoader {
public:	
  Impostor_Loader(ModelHandler *mh);
  ~Impostor_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* _IMPOSTOR_LOADER_H_ */
