// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: NPlane_Loader.h,v 1.3 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_NPLANE_LOADER_H
#define SEAR_NPLANE_LOADER_H 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"

namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class NPlane_Loader : public ModelLoader {
public:	
  NPlane_Loader(ModelHandler *mh);
  ~NPlane_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* SEAR_NPLANE_LOADER_H */
