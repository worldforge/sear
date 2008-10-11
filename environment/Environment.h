// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

#ifndef SEAR_ENVIRONMENT_H
#define SEAR_ENVIRONMENT_H 1

#include <map>
#include <memory>

#include <sigc++/trackable.h>

#include <Mercator/Shader.h>

#include <wfmath/point.h>

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

  int init();
  int reinit();
  void shutdown();

  static Environment &getInstance() {
    return instance;
  }

  float getHeight(float x, float y);
  void setBasePoint(int x, int y, float z);
  void setSurface(const std::string &name, const std::string &pattern, const Mercator::Shader::Parameters &params);

  void update(float time_elapsed);

  void renderSky();
  void renderTerrain(const WFMath::Point<3> &pos, bool select_mode);
  void renderSea();
  void renderWeather();

  void contextCreated();
  void contextDestroyed(bool check);

  void addArea(Mercator::Area* we);
  void removeArea(Mercator::Area* we);
  void registerTerrainShader(Mercator::Shader* s, const std::string& texId);
  
  void resetWorld();
  void setWeatherEntity(WorldEntity *we);

  void registerCommands(Console *con);

  float getVisibility() const;

private:
  bool m_initialised;

  static Environment instance;

  std::auto_ptr<TerrainRenderer> m_terrain;
  std::auto_ptr<SkyDome> m_skyDome;
  std::auto_ptr<Stars> m_stars;
  std::auto_ptr<Weather> m_weather;
};

} // namespace Sear

#endif
