// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2002 - 2003 Alistair Riddoch
// Copyright (C) 2005 - 2006 Simon Goodall

// $Id: TerrainEntity.cpp,v 1.8 2007-09-23 09:37:00 simon Exp $

#include <sigc++/object_slot.h>

#include <Eris/View.h>

#include <Atlas/Message/Element.h>

#include <Mercator/Shader.h>

#include "renderers/Graphics.h"
#include "environment/Environment.h"

#include "System.h"
#include "TerrainEntity.h"

namespace Sear {

TerrainEntity::TerrainEntity(const std::string &id, Eris::TypeInfo *ty, Eris::View *view) : WorldEntity(id, ty, view) {
  Changed.connect(sigc::mem_fun(this, &TerrainEntity::changed));
}
  
void TerrainEntity::changed(const Eris::StringSet &s) {
  Eris::StringSet::const_iterator I = s.begin();
  Eris::StringSet::const_iterator Iend = s.end();
  while (I != Iend) {
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
  } else {
    if (I->second.isList()) {
      // Legacy support for old list format.
      const Atlas::Message::ListType & plist = I->second.asList();
      Atlas::Message::ListType::const_iterator J = plist.begin();
      Atlas::Message::ListType::const_iterator Jend = plist.end();
      for(; J != Jend; ++J) {
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
      Atlas::Message::MapType::const_iterator Jend = plist.end();
      for(; J != Jend; ++J) {
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

  // Read surfaces data
  Atlas::Message::MapType::const_iterator J = tmap.find("surfaces");
  if (J == tmap.end()) {
    std::cerr << "Terrain surfaces does not exist" << std::endl;
    return;
  }
  if (J->second.isList() == false) {
    std::cerr << "Terrain surfaces is not a list" << std::endl;
    return;
  }

  const Atlas::Message::ListType & plist = J->second.asList();
  Atlas::Message::ListType::const_iterator K = plist.begin();
  Atlas::Message::ListType::const_iterator Kend = plist.end();
  for(; K != Kend; ++K) {
    if (!(*K).isMap()) {
      std::cerr << "Surfaces entry is not a map" << std::endl;
      continue;
    }
    const Atlas::Message::MapType &L = K->asMap();
    Atlas::Message::MapType::const_iterator name_itr = L.find("name");
    Atlas::Message::MapType::const_iterator pattern_itr = L.find("pattern");
    Atlas::Message::MapType::const_iterator params_itr = L.find("params");
    if (name_itr == L.end() || pattern_itr == L.end()) {
      std::cerr << "Required params not here" << std::endl;
      continue;
    }
    const std::string &name = name_itr->second.asString(); 
    const std::string &pattern = pattern_itr->second.asString(); 
    printf("Name: %s - Pattern %s\n", name.c_str(), pattern.c_str());

    Mercator::Shader::Parameters params;
    if (params_itr != L.end()) {
      const Atlas::Message::MapType & mlist = params_itr->second.asMap();
      Atlas::Message::MapType::const_iterator M = mlist.begin();
      Atlas::Message::MapType::const_iterator Mend = mlist.end();
      for(; M != Mend; ++M) {
        // TODO: Check type first
        params[M->first] = M->second.asNum();
      }
    }

    Environment::getInstance().setSurface(name, pattern, params);
  }
}

} /* namespace Sear */
