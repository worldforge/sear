// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "Environment.h"

#include "TerrainRenderer.h"
#include "SkyDome.h"
#include "Stars.h"
                                                                                
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
  if (!m_initialised) return;

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
  m_terrain->renderSea();
}

void Environment::invalidate() {
  m_terrain->invalidate();
  m_skyDome->invalidate();
}

} // namespace Sear
