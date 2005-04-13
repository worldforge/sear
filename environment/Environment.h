// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

#ifndef SEAR_ENVIRONMENT_H
#define SEAR_EnVIRONMENT_H

#include <stdlib.h>
#include <map>

#include <wfmath/point.h>
#include <wfmath/quaternion.h>

namespace Mercator { class Area; }

namespace Sear {

class TerrainRenderer;
class SkyDome;
class Stars;
class WorldEntity;

class Environment {
  Environment() :
    m_initialised(false),
    m_terrain(NULL),
    m_skyDome(NULL)
  {}

public:
  ~Environment() {
    assert(m_initialised == false);
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

  void invalidate();

    void registerAreaEntity(WorldEntity* we);

private:
  bool m_initialised;

  static Environment instance;
  TerrainRenderer *m_terrain;
  SkyDome *m_skyDome;
  Stars* m_stars;
  
    typedef std::map<std::string, Mercator::Area*> AreaDict;
    AreaDict m_areaEntities;
};

} // namespace Sear

#endif
