// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: ParticleSystemLoader.h,v 1.3 2006-02-07 11:31:03 simon Exp $

#ifndef SEAR_PARTICLE_SYSTEM_LOADER_H
#define SEAR_PARTICLE_SYSTEM_LOADER_H

#include "ModelLoader.h"

namespace Sear
{

class ParticleSystemLoader : public ModelLoader
{
public:
  ParticleSystemLoader(ModelHandler *mh);
  virtual ~ParticleSystemLoader();

  virtual SPtr<ModelRecord> loadModel(Render *render, 
    WorldEntity *we,
    const std::string &model_id,
    varconf::Config &model_config);

};

} // of namespace Sear

#endif // of SEAR_PARTICLE_SYSTEM_LOADER_H
