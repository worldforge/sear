// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

// $Id: Cal3d_Loader.h,v 1.5 2006-02-15 09:50:31 simon Exp $

#ifndef SEAR_CAL3D_LOADER_H
#define SEAR_CAL3D_LOADER_H 1

#include "ModelLoader.h"

namespace Sear {
// Forward Declarations
class CoreModelHandler;

class Cal3d_Loader : public ModelLoader {
public:	
  Cal3d_Loader();
  virtual ~Cal3d_Loader();

  virtual std::string getType() const { return CAL3D; }

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config);

private:
  static const std::string CAL3D;
  CoreModelHandler *m_core_model_handler;
  
};

} /* namespace Sear */

#endif /* SEAR_CAL3D_LOADER_H */
