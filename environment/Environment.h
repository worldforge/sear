#ifndef SEAR_ENVIRONMENT_H
#define SEAR_EnVIRONMENT_H

#include <stdlib.h>

#include <wfmath/point.h>
#include <wfmath/quaternion.h>

namespace Sear {

class TerrainRenderer;
class SkyDome;

class Environment {
  Environment() :
    m_initialised(false),
    m_terrain(NULL),
    m_skyDome(NULL)
  {}

public:
  ~Environment() {
    if (m_initialised) shutdown();
  }

  void init();
  void shutdown();

  static Environment &getInstance() {
    return instance;
  }

  float getHeight(float x, float y);
  void setBasePoint(int x, int y, float z);

  void renderSky();
  void renderTerrain(const WFMath::Point<3> &pos);
  void renderSea();

private:
  bool m_initialised;

  static Environment instance;
  TerrainRenderer *m_terrain;
  SkyDome *m_skyDome;

};

}

#endif
