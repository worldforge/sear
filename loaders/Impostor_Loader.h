// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: Impostor_Loader.h,v 1.5 2002-09-07 23:27:06 simon Exp $

#ifndef SEAR_IMPOSTOR_LOADER_H
#define SEAR_IMPOSTOR_LOADER_H 1

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

#endif /* SEAR_IMPOSTOR_LOADER_H */
