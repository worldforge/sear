// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2007 Simon Goodall

#include <varconf/Config.h>
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
    AreaModel* amodel = new AreaModel(we);
    
    if (!amodel->init()) {
//        delete model_record;
        delete amodel;
        return SPtr<ModelRecord>();
  //      return NULL;
    }
    
    model_record->model = SPtr<Model>(amodel);
    model_record->select_state = 0; // default
    model_record->select_state = 2; // select
// create a shader if required
    if (model_config.findItem(model_id, KEY_shader_tex) &&
        !m_shaders.count(amodel->getLayer())) 
    {
        Mercator::AreaShader* ashade = new Mercator::AreaShader(amodel->getLayer()); 
        std::string shaderTex(model_config.getItem(model_id, KEY_shader_tex));
        Environment::getInstance().registerTerrainShader(ashade, shaderTex);
        m_shaders[amodel->getLayer()] = ashade;
    }

    return model_record;
}

} // of namespace Sear
