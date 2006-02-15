// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: ParticleSystemLoader.cpp,v 1.7 2006-02-15 09:50:31 simon Exp $

#include <varconf/Config.h>

#include "ParticleSystemLoader.h"
#include "ParticleSystem.h"
#include "ModelRecord.h"
#include "renderers/StateManager.h"
#include "renderers/RenderSystem.h"
#include "src/WorldEntity.h"

#include <iostream>

namespace Sear {

const std::string ParticleSystemLoader::PARTICLE_MODEL = "particle";

static const char* KEY_particle_tex = "texture";
static const char* KEY_min_lifetime = "min_life";
static const char* KEY_max_lifetime = "max_life";

varconf::Variable getItemWithDefault(varconf::Config& cfg, 
    const std::string& section,
    const std::string& item,
    const varconf::Variable def)
{
    if (cfg.findItem(section, item)) return cfg.getItem(section, item);
    return def;
}

ParticleSystemLoader::ParticleSystemLoader()
{
}

ParticleSystemLoader::~ParticleSystemLoader()
{
}

SPtr<ModelRecord> ParticleSystemLoader::loadModel(WorldEntity *we,
    const std::string &model_id, 
    varconf::Config &cfg)
{
    if (!cfg.findItem(model_id, KEY_particle_tex)) {
        std::cerr << "particle model record is incomplete" << std::endl;
        return SPtr<ModelRecord>();
    }

    SPtr<ModelRecord> model_record = ModelLoader::loadModel(we, model_id, cfg);
    ParticleSystem* ps = new ParticleSystem(we);
    
    ps->m_ttl = DRange(getItemWithDefault(cfg, model_id, KEY_min_lifetime, 0.0),
                    getItemWithDefault(cfg, model_id, KEY_max_lifetime, 10.0));
    
    
    ps->setBBox(we->getBBox());
    ps->init();
    ps->setTextureName( cfg.getItem(model_id, KEY_particle_tex) );
    
    model_record->model = SPtrShutdown<Model>(ps);
    model_record->state = RenderSystem::getInstance().getStateManager()->getState("particles");
    model_record->select_state = 2; // select

    return model_record;
}

}
