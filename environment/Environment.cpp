
#include "Environment.h"

#include "TerrainRenderer.h"

namespace Sear {

Environment  Environment::instance;

void Environment::init() {
  m_terrain = new TerrainRenderer();

}

void Environment::shutdown() {

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

void Environment::render(const WFMath::Point<3> &pos) {
  m_terrain->render(pos);
}

}
