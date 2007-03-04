// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 - 2007 Simon Goodall

// $Id: Weather.h,v 1.6 2007-03-04 14:28:40 simon Exp $

#ifndef SEAR_ENVIRONMENT_WEATHER_H
#define SEAR_ENVIRONMENT_WEATHER_H 1

#include <sigc++/trackable.h>
#include <Eris/Types.h>

#include "interfaces/ConsoleObject.h"

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

  void setWeatherEntity(WorldEntity *we);
  void render();

  void registerCommands(Console *con);
  void runCommand(const std::string &cmd, const std::string &args);  

  void update(float time_elapsed) {
    if (m_current_visibility > m_visibility) { 
      m_current_visibility -= time_elapsed;
      if (m_current_visibility < m_visibility)
         m_current_visibility = m_visibility;
    }
    else if (m_current_visibility < m_visibility) { 
      m_current_visibility += time_elapsed;
      if (m_current_visibility > m_visibility)
         m_current_visibility = m_visibility;
    }
  }
  float getVisibility() const { return m_current_visibility; }

private:
  void weatherChanged(const Eris::StringSet &s, Sear::WorldEntity *we);
  bool m_initialised;

  float m_rain;
  float m_snow;
  float m_visibility, m_current_visibility;

  int m_rain_drop_id;
};

} // namespace Sear
#endif // SEAR_ENVIRONMENT_WEATHER_H 
