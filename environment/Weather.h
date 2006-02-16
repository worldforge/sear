// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2006 Simon Goodall

// $Id: Weather.h,v 1.2 2006-02-16 21:23:30 simon Exp $

#ifndef SEAR_ENVIRONMENT_WEATHER_H
#define SEAR_ENVIRONMENT_WEATHER_H 1

#include <sigc++/trackable.h>
#include <Eris/Types.h>

namespace Sear {

class WorldEntity;

class Weather : public sigc::trackable {
public:

  Weather();
  ~Weather();

  int init();
  void shutdown();
  bool isInitialised() const { return m_initialised; }

  void setWeatherEntity(WorldEntity *we);
  void render();

private:
  void weatherChanged(const Eris::StringSet &s, Sear::WorldEntity *we);
  bool m_initialised;

  float m_rain;
  float m_snow;
};

} // namespace Sear
#endif // SEAR_ENVIRONMENT_WEATHER_H 
