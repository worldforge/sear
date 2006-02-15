// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: ParticleSystemLoader.h,v 1.4 2006-02-15 09:50:31 simon Exp $

#ifndef SEAR_PARTICLE_SYSTEM_LOADER_H
#define SEAR_PARTICLE_SYSTEM_LOADER_H

#include "ModelLoader.h"

namespace Sear {

class ParticleSystemLoader : public ModelLoader {
public:
  ParticleSystemLoader();
  virtual ~ParticleSystemLoader();

  virtual std::string getType() const { return PARTICLE_MODEL; }

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we,
    const std::string &model_id,
    varconf::Config &model_config);
private:
  static const std::string PARTICLE_MODEL;
};

} // of namespace Sear

#endif // of SEAR_PARTICLE_SYSTEM_LOADER_H
