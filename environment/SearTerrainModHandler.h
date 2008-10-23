// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

#ifndef SEAR_ENVIRONMENT_SEARTERRAINMODHANDLER_H
#define SEAR_ENVIRONMENT_SEARTERRAINMODHANDLER_H 1

#include <string>
#include "Eris/TerrainModHandler.h"

namespace Sear {

class SearTerrainModHandler : public Eris::TerrainModHandler {
public:

  SearTerrainModHandler() {}
  virtual ~SearTerrainModHandler() {}

  // Override to get client estimiate of entity z pos.
  virtual float getZPos(float x, float y);
  virtual void logFailure(const std::string &msg);

private:

};
} // namespace Sear

#endif // SEAR_ENVIRONMENT_SEARTERRAINMODHANDLER_H
