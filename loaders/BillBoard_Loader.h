// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: BillBoard_Loader.h,v 1.5 2002-09-07 23:27:05 simon Exp $

#ifndef SEAR_BILLBOARD_LOADER_H
#define SEAR_BILLBOARD_LOADER_H 1

#include "src/ModelLoader.h"
#include "src/ModelStruct.h"


namespace Sear {

// Forward Declarations
class Model;
class ModelHandler;
//class ObjectProperties;
	
class BillBoard_Loader : public ModelLoader {
public:	
  BillBoard_Loader(ModelHandler *mh);
  ~BillBoard_Loader();

  Model *loadModel(Render *, ModelStruct &);

};

} /* namespace Sear */

#endif /* SEAR_BILLBOARD_LOADER_H */
