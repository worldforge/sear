// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2008 Simon Goodall, University of Southampton

#include <cstdio>

#include "SearTerrainModHandler.h"
#include "Environment.h"

namespace Sear {

float SearTerrainModHandler::getZPos(float x, float y) {
  // TODO: Make sure we are using same coord system
  return Environment::getInstance().getHeight(x, y);
}

void SearTerrainModHandler::logFailure(const std::string &msg) {
  printf("[SearTerrainModHandler] %s\n", msg.c_str());
}

} // namespace Sear
