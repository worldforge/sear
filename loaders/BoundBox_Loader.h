// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: BoundBox_Loader.h,v 1.5 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_BOUNDBOX_LOADER_H
#define SEAR_BOUNDBOX_LOADER_H 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"


namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class BoundBox_Loader : public ModelLoader {
public:	
  BoundBox_Loader(ModelHandler *mh);
  ~BoundBox_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* SEAR_BOUNDBOX_LOADER_H */
