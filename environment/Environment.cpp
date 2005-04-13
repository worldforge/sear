// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "Environment.h"

#include "TerrainRenderer.h"
#include "SkyDome.h"
#include "Stars.h"

#include <Mercator/Area.h>
#include "WorldEntity.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

namespace Sear {

Environment  Environment::instance;

void Environment::init() {
  assert(!m_initialised);
  m_terrain = new TerrainRenderer();
  m_skyDome = new SkyDome(1.0f, 20, 20);
  m_stars = new Stars();
  
  m_initialised = true;
}

void Environment::shutdown() {
  assert(m_initialised);
//  if (!m_initialised) return;

  delete m_terrain;
  m_terrain = NULL;
 
  delete m_skyDome;
  m_skyDome = NULL;

  delete m_stars;
  m_stars = NULL;
  
  m_initialised = false;
}

float Environment::getHeight(float x, float y) {
  WFMath::Vector<3> n;
  float z = 0.0f;
  if (m_terrain) {  
    m_terrain->m_terrain.getHeightAndNormal(x,y,z,n);
  }
  return z;
}

void Environment::setBasePoint(int x, int y, float z) {
  if (m_terrain) {  
    m_terrain->m_terrain.setBasePoint(x, y, z);
  } 
}

void Environment::renderSky()
{
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("stars"));
  m_stars->render();
  
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState("sky_0"));
  m_skyDome->render();
}

void Environment::renderTerrain(const WFMath::Point<3> &pos) {
  m_terrain->render(pos);
}

void Environment::renderSea() {
return;
  m_terrain->renderSea();
}

void Environment::invalidate() {
  m_terrain->invalidate();
  m_skyDome->invalidate();
}

void Environment::registerAreaEntity(WorldEntity* we)
{
    assert(we);
    
    if (!we->hasAttr("area")) {
        std::cerr << "registerAreaEntity called on entity with no area attribute" << std::endl;
        return;
    }
    
    const Atlas::Message::MapType& areaData(we->valueOfAttr("area").asMap());
    Atlas::Message::MapType::const_iterator it = areaData.find("points");
    if ((it == areaData.end()) || !it->second.isList()) {
        std::cerr << "malformed area attribute on entity, no points data" << std::endl;
        return;
    }
    
    const Atlas::Message::ListType& pointsData(it->second.asList());
    Mercator::Area* ar = m_areaEntities[we->getId()];
    
    if (ar == NULL) {
        it = areaData.find("layer");
        if ((it == areaData.end()) || !it->second.isInt()) {
            std::cerr << "malformed area attribute on entity, no layer data" << std::endl;
            return;
        }

        int layer = it->second.asInt();
        ar = new Mercator::Area(layer, false);
    } else {
        // check layer + hole haven't changed?
        #warning modifying Area shapes needs to be fixed
        assert(false);
    }
    
    WFMath::Polygon<2> poly;
    for (unsigned int p=0; p<pointsData.size(); ++p) {
        if (!pointsData[p].isList()) {
            std::cerr << "skipped malformed point in area" << std::endl;
            continue;
        }
        
        const Atlas::Message::ListType& point(pointsData[p].asList());
        if ((point.size() < 2) || !point[0].isFloat() || !point[1].isFloat()) {
            std::cerr << "skipped malformed point in area" << std::endl;
            continue;
        }
        
        WFMath::Point<2> wpt(point[0].asFloat(), point[1].asFloat());
        poly.addCorner(poly.numCorners(), wpt);
    }
    
    ar->setShape(poly);
    m_terrain->m_terrain.addArea(ar);
    m_areaEntities[we->getId()] = ar;
}

} // namespace Sear
