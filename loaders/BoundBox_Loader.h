// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: BoundBox_Loader.h,v 1.9 2005-03-15 17:55:03 simon Exp $

#ifndef SEAR_BOUNDBOX_LOADER_H
#define SEAR_BOUNDBOX_LOADER_H 1

#include "ModelLoader.h"


namespace Sear {

// Forward Declarations
class ModelHandler;
	
class BoundBox_Loader : public ModelLoader {
public:	
  BoundBox_Loader(ModelHandler *mh);
  ~BoundBox_Loader();

  ModelRecord *loadModel(Render *render, ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);
protected:
  static const std::string BOUNDBOX;  

};

} /* namespace Sear */

#endif /* SEAR_BOUNDBOX_LOADER_H */
