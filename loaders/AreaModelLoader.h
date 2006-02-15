// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: AreaModelLoader.h,v 1.5 2006-02-15 09:50:31 simon Exp $

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
  AreaModelLoader();
  virtual ~AreaModelLoader();

  virtual std::string getType() const { return AREA_MODEL; }

  virtual SPtr<ModelRecord> loadModel(WorldEntity *we, const std::string &model_id, varconf::Config &model_config);
    
private:
  static const std::string AREA_MODEL;
  typedef std::map<int, Mercator::AreaShader*> Shaderstore;
  Shaderstore m_shaders;
};

} // of namespace Sear

#endif // of SEAR_AREAMODEL_LOADER_H
