// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall

// $Id: BoundBox_Loader.h,v 1.11 2006-02-15 09:50:31 simon Exp $

#ifndef SEAR_BOUNDBOX_LOADER_H
#define SEAR_BOUNDBOX_LOADER_H 1

#include "ModelLoader.h"


namespace Sear {

class BoundBox_Loader : public ModelLoader {
public:	
  BoundBox_Loader();
  virtual ~BoundBox_Loader();

  virtual std::string getType() const { return BOUNDBOX; }

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
protected:
  static const std::string BOUNDBOX;  

};

} /* namespace Sear */

#endif /* SEAR_BOUNDBOX_LOADER_H */
