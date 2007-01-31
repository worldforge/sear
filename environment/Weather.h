// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 - 2007 Simon Goodall

// $Id: Weather.h,v 1.4 2007-01-31 21:19:37 simon Exp $

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

  float getVisibility() const { return m_visibility; }

private:
  void weatherChanged(const Eris::StringSet &s, Sear::WorldEntity *we);
  bool m_initialised;

  float m_rain;
  float m_snow;
  float m_visibility;
};

} // namespace Sear
#endif // SEAR_ENVIRONMENT_WEATHER_H 
