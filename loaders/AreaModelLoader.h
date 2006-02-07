// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: AreaModelLoader.h,v 1.4 2006-02-07 11:31:03 simon Exp $

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

  virtual SPtr<ModelRecord> loadModel(Render *render, 
    WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
    
private:
  typedef std::map<int, Mercator::AreaShader*> Shaderstore;
  Shaderstore m_shaders;
};

} // of namespace Sear

#endif // of SEAR_AREAMODEL_LOADER_H
