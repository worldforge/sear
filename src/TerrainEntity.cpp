// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Simon Goodall, University of Southampton

// $Id: TerrainEntity.cpp,v 1.2 2005-03-04 17:58:25 simon Exp $

#include "TerrainEntity.h"

#include <Eris/View.h>
#include "System.h"
#include "renderers/Graphics.h"
#include "environment/Environment.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

namespace Sear {


TerrainEntity::TerrainEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view) : WorldEntity(id, ty, view) {
  Changed.connect(SigC::slot(*this, &TerrainEntity::changed));
}
  
void TerrainEntity::changed(const Eris::StringSet &s) {
  Eris::StringSet::const_iterator I = s.begin();
  while (I != s.end()) {
    if (*I == "terrain") updateTerrain();
    ++I;
  }
} 

void TerrainEntity::updateTerrain() {
  if (!hasAttr("terrain")) {
    std::cerr << "Entity has no terrain" << std::endl << std::flush;
    std::cerr << "Entity id " << getId() << std::endl << std::flush;
    return;
  }

  const Atlas::Message::Element &terrain = valueOfAttr("terrain");
  if (!terrain.isMap()) {
    std::cerr << "Terrain is not a map" << std::endl << std::flush;
  }
  const Atlas::Message::MapType & tmap = terrain.asMap();
  Atlas::Message::MapType::const_iterator I = tmap.find("points");
  int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
  if (I == tmap.end()) {
    std::cerr << "No terrain points" << std::endl << std::flush;
  }
  if (I->second.isList()) {
    // Legacy support for old list format.
    const Atlas::Message::ListType & plist = I->second.asList();
    Atlas::Message::ListType::const_iterator J = plist.begin();
    for(; J != plist.end(); ++J) {
      if (!J->isList()) {
        std::cout << "Non list in points" << std::endl << std::flush;
        continue;
      }
      const Atlas::Message::ListType & point = J->asList();
      if (point.size() != 3) {
        std::cout << "point without 3 nums" << std::endl << std::flush;
        continue;
      }
      int x = (int)point[0].asNum();
      int y = (int)point[1].asNum();
      xmin = std::min(xmin, x);
      xmax = std::max(xmax, x);
      ymin = std::min(ymin, y);
      ymax = std::max(ymax, y);
      Environment::getInstance().setBasePoint(x,y,point[2].asNum());
    }
  } else if (I->second.isMap()) {
    const Atlas::Message::MapType & plist = I->second.asMap();
    Atlas::Message::MapType::const_iterator J = plist.begin();
    for(; J != plist.end(); ++J) {
      if (!J->second.isList()) {
        std::cout << "Non list in points" << std::endl << std::flush;
        continue;
      }
      const Atlas::Message::ListType & point = J->second.asList();
      if (point.size() != 3) {
        std::cout << "point without 3 nums" << std::endl << std::flush;
        continue;
      }
      int x = (int)point[0].asNum();
      int y = (int)point[1].asNum();
      xmin = std::min(xmin, x);
      xmax = std::max(xmax, x);
      ymin = std::min(ymin, y);
      ymax = std::max(ymax, y);

      Environment::getInstance().setBasePoint(x,y,point[2].asNum());
    }
  } else {
    std::cerr << "Terrain is the wrong type" << std::endl << std::flush;
  }
}

} /* namespace Sear */
