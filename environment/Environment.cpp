
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
  return 0.0f;
}

void Environment::setBasePoint(int x, int y, float z) {
  m_terrain->m_terrain.setBasePoint(x, y, z);
}

}
