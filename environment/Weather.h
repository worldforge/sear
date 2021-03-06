// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 - 2008 Simon Goodall

// $Id: Weather.h,v 1.7 2007-04-22 16:37:37 simon Exp $

#ifndef SEAR_ENVIRONMENT_WEATHER_H
#define SEAR_ENVIRONMENT_WEATHER_H 1

#include <sigc++/trackable.h>
#include <Eris/Types.h>

#include "interfaces/ConsoleObject.h"

namespace varconf {
  class Config;
}

namespace Sear {

class Console;
class WorldEntity;

class Weather : public sigc::trackable, public ConsoleObject {
public:

  Weather();
  ~Weather();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void readConfig(const varconf::Config &);
  void writeConfig(varconf::Config &) const;

  void setWeatherEntity(WorldEntity *we);
  void render();

  void registerCommands(Console *con);
  void runCommand(const std::string &cmd, const std::string &args);  

  void update(float time_elapsed);

  float getVisibility() const { return m_current_visibility; }

private:
  void weatherChanged(const Eris::StringSet &s, Sear::WorldEntity *we);
  bool m_initialised;

  float m_rain;
  float m_snow;
  float m_visibility, m_current_visibility;

  int m_rain_drop_id;
  bool m_draw_rain;
};

} // namespace Sear
#endif // SEAR_ENVIRONMENT_WEATHER_H 
