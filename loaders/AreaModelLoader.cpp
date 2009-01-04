// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

#include <varconf/config.h>
#include <Mercator/AreaShader.h>

#include "AreaModelLoader.h"
#include "AreaModel.h"
#include "ModelRecord.h"
#include "ObjectRecord.h"
#include "environment/Environment.h"
#include "renderers/StateManager.h"
#include "src/WorldEntity.h"

namespace Sear {

class AreaModel;

const std::string AreaModelLoader::AREA_MODEL = "area";
static const char* KEY_shader_tex = "shader_texture";

AreaModelLoader::AreaModelLoader() {}

AreaModelLoader::~AreaModelLoader() {}

SPtr<ModelRecord> AreaModelLoader::loadModel(WorldEntity *we,
    const std::string &model_id, 
    varconf::Config &model_config)
{
    SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, model_config);
    AreaModel* amodel = new AreaModel(we, this);
    
    if (!amodel->init()) {
      delete amodel;
      return SPtr<ModelRecord>();
    }
    
    model_record->model = SPtr<Model>(amodel);
    model_record->state = 0; // default
    model_record->select_state = 2; // select

    // create a shader if required
    if (model_config.findItem(model_id, KEY_shader_tex) &&
        !m_shaders.count(amodel->getLayer())) 
    {
        Mercator::AreaShader* ashade = new Mercator::AreaShader(amodel->getLayer()); 
        std::string shaderTex(model_config.getItem(model_id, KEY_shader_tex));
        Environment::getInstance().registerTerrainShader(ashade, shaderTex);
        m_shaders[amodel->getLayer()] = ashade;
        m_counters[amodel->getLayer()] = 0;
    }

    m_counters[amodel->getLayer()]++ ;


printf("Layer ID %d just got %d refs\n", amodel->getLayer(), m_counters[amodel->getLayer()]);
    return model_record;
}

void AreaModelLoader::releaseLayer(int id) {

  int count = m_counters[id];
  count--;
  m_counters[id] = count;
  printf("Layer ID %d just got down to %d refs\n", id, count);
  if (count == 0) {
    Mercator::AreaShader* ashade = m_shaders[id];
    Environment::getInstance().deregisterTerrainShader(ashade);
    delete ashade;

    m_shaders.erase(m_shaders.find(id));
    m_counters.erase(m_counters.find(id));
  }

}

} // of namespace Sear
