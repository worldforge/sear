// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall

#include "AreaModelLoader.h"
#include "ModelHandler.h"
#include "AreaModel.h"
#include "ModelRecord.h"
#include "environment/Environment.h"
#include "renderers/StateManager.h"
#include <Mercator/AreaShader.h>

namespace Sear
{

class AreaModel;

static const char* AREA_MODEL = "area";
static const char* KEY_shader_tex = "shader_texture";

AreaModelLoader::AreaModelLoader(ModelHandler* mh)
{
    mh->registerModelLoader(AREA_MODEL, this);
}

AreaModelLoader::~AreaModelLoader()
{
}

ModelRecord* AreaModelLoader::loadModel(Render *render, 
    ObjectRecord *record,
    const std::string &model_id, 
    varconf::Config &model_config)
{
    ModelRecord *model_record = ModelLoader::loadModel(render, record, model_id, model_config);
    AreaModel* amodel = new AreaModel(render, record);
    model_record->model = amodel;
    model_record->select_state = 0; // default
    model_record->select_state = 2; // select
    
    if (!amodel->init()) {
        delete model_record;
        delete amodel;
        return NULL;
    }
    
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
