// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2006 Simon Goodall, University of Southampton

#ifndef SEAR_ENVIRONMENT_H
#define SEAR_ENVIRONMENT_H 1

#include <map>

#include <sigc++/trackable.h>

#include <wfmath/point.h>

#include "common/SPtr.h"

namespace Mercator { 
    class Area;
    class Shader;
}

namespace Sear {

class Console;
class TerrainRenderer;
class SkyDome;
class Stars;
class WorldEntity;
class Weather;

class Environment : public sigc::trackable {
public:
  Environment();
  ~Environment();

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
  void renderWeather();

  void contextCreated();
  void contextDestroyed(bool check);

  void registerArea(Mercator::Area* we);
  void registerTerrainShader(Mercator::Shader* s, const std::string& texId);
  
  void resetWorld();
  void setWeatherEntity(WorldEntity *we);

  void registerCommands(Console *con);

private:
  bool m_initialised;

  static Environment instance;

  SPtr<TerrainRenderer> m_terrain;
  SPtr<SkyDome> m_skyDome;
  SPtr<Stars> m_stars;
  SPtrShutdown<Weather> m_weather;
};

} // namespace Sear

#endif
