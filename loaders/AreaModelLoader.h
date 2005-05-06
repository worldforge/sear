#ifndef SEAR_AREAMODEL_LOADER_H
#define SEAR_AREAMODEL_LOADER_H

#include "ModelLoader.h"
#include <map>

namespace Mercator
{
    class AreaShader;
}

namespace Sear
{

class AreaModelLoader : public ModelLoader {
public:	
  AreaModelLoader(ModelHandler *mh);
  ~AreaModelLoader();

  virtual ModelRecord *loadModel(Render *render, 
    ObjectRecord *record, const std::string &model_id, varconf::Config &model_config);
    
private:
  typedef std::map<int, Mercator::AreaShader*> Shaderstore;
  Shaderstore m_shaders;
};

} // of namespace Sear

#endif // of SEAR_AREAMODEL_LOADER_H
