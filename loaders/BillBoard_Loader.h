// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2002 Simon Goodall

// $Id: BillBoard_Loader.h,v 1.6 2002-09-26 17:17:46 simon Exp $

#ifndef SEAR_BILLBOARD_LOADER_H
#define SEAR_BILLBOARD_LOADER_H 1

#include "src/ModelLoader.h"

namespace Sear {

// Forward Declarations
	
class BillBoard_Loader : public ModelLoader {
public:	
  BillBoard_Loader(ModelHandler *mh);
  ~BillBoard_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config *model_config);

protected:
  static const std::string BILLBOARD;
};

} /* namespace Sear */

#endif /* SEAR_BILLBOARD_LOADER_H */
