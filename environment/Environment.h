// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#ifndef SEAR_ENVIRONMENT_H
#define SEAR_EnVIRONMENT_H

#include <stdlib.h>
#include <map>

#include <sigc++/trackable.h>

#include <wfmath/point.h>
#include <wfmath/quaternion.h>

namespace Mercator { 
    class Area;
    class Shader;
}

namespace Sear {

class TerrainRenderer;
class SkyDome;
class Stars;
class WorldEntity;

class Environment : public sigc::trackable {
  Environment() :
    m_initialised(false),
    m_terrain(NULL),
    m_skyDome(NULL)
  {}

public:
  ~Environment() {
    assert(m_initialised == false);
  }

  void init();
  void shutdown();

  static Environment &getInstance() {
    return instance;
  }

  float getHeight(float x, float y);
  void setBasePoint(int x, int y, float z);

  void renderSky();
  void renderTerrain(const WFMath::Point<3> &pos, bool select_mode);
  void renderSea();

  void contextCreated();
  void contextDestroyed(bool check);

  void registerArea(Mercator::Area* we);
  void registerTerrainShader(Mercator::Shader* s, const std::string& texId);
  
  void resetWorld();

private:
  bool m_initialised;

  static Environment instance;
  TerrainRenderer *m_terrain;
  SkyDome *m_skyDome;
  Stars* m_stars;
};

} // namespace Sear

#endif
