
#include "Environment.h"

#include "TerrainRenderer.h"
#include "SkyDome.h"

namespace Sear {

Environment  Environment::instance;

void Environment::init() {
  if (m_initialised) shutdown();
  m_terrain = new TerrainRenderer();
  m_skyDome = new SkyDome();

  m_initialised = true;
}

void Environment::shutdown() {
  if (!m_initialised) return;

  delete m_terrain;
  m_terrain = NULL;
 
  delete m_skyDome;
  m_skyDome = NULL;

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

void Environment::renderSky() {
  m_skyDome->render(1.0f, 20, 20);
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

}
