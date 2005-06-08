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

  virtual ModelRecord *loadModel(Render *render, 
    ObjectRecord *record,
    const std::string &model_id,
    varconf::Config &model_config);

};

} // of namespace Sear

#endif // of SEAR_PARTICLE_SYSTEM_LOADER_H
