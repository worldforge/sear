// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

#ifndef _BILLBOARD_LOADER_H_
#define _BILLBOARD_LOADER_H_ 1

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

#endif /* _BILLBOARD_LOADER_H_ */
