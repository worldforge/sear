// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: 3ds_Loader.h,v 1.4 2002-09-07 23:27:05 simon Exp $

#ifndef SEAR_3DS_LOADER_H
#define SEAR_3DS_LOADER_H 1


#include "src/ModelLoader.h"
#include "src/ModelStruct.h"


namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
	
class ThreeDS_Loader : public ModelLoader {
public:	
  ThreeDS_Loader(ModelHandler *mh);
  ~ThreeDS_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* SEAR_3DS_LOADER_H */
