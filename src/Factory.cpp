// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: Factory.cpp,v 1.5 2005-02-18 16:39:06 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "Factory.h"

#include <Eris/View.h>
#include <Eris/Connection.h>

#include "System.h"
#include "renderers/Graphics.h"
#include "environment/Environment.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

namespace Sear {

   Eris::EntityPtr Factory::instantiate(const Atlas::Objects::Entity::GameEntity & ge, Eris::TypeInfo *type, Eris::View *view) {
    WorldEntity *we = new WorldEntity(ge->getId(), type, view);
std::cout << "Type: " << type->getName() << std::endl;
//    Eris::TypeInfoPtr type = world->getConnection()->getTypeService()->getTypeForAtlas(ge);
    if (type->isA(terrainType)) {
      // Extract base points and send to terrain        
      //TerrainEntity * te = new TerrainEntity(ge,w);


   if (!we->hasAttr("terrain")) {
        std::cerr << "World entity has no terrain" << std::endl << std::flush;
        std::cerr << "World entity id " << we->getId() << std::endl
                  << std::flush;
        return we;
    }
    const Atlas::Message::Element &terrain = we->valueOfAttr("terrain");
    if (!terrain.isMap()) {
        std::cerr << "Terrain is not a map" << std::endl << std::flush;
    }
    const Atlas::Message::Element::MapType & tmap = terrain.asMap();
    Atlas::Message::Element::MapType::const_iterator I = tmap.find("points");
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0;
    if (I == tmap.end()) {
        std::cerr << "No terrain points" << std::endl << std::flush;
    }
   if (I->second.isList()) {
        // Legacy support for old list format.
        const Atlas::Message::Element::ListType & plist = I->second.asList();
        Atlas::Message::Element::ListType::const_iterator J = plist.begin();
        for(; J != plist.end(); ++J) {
            if (!J->isList()) {
                std::cout << "Non list in points" << std::endl << std::flush;
                continue;
            }
            const Atlas::Message::Element::ListType & point = J->asList();
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
    //        m_terrain.setBasePoint(x, y, point[2].asNum());
Environment::getInstance().setBasePoint(x,y,point[2].asNum());
        }
    } else if (I->second.isMap()) {

              const Atlas::Message::Element::MapType & plist = I->second.asMap();
        Atlas::Message::Element::MapType::const_iterator J = plist.begin();
        for(; J != plist.end(); ++J) {
            if (!J->second.isList()) {
                std::cout << "Non list in points" << std::endl << std::flush;
                continue;
            }
            const Atlas::Message::Element::ListType & point = J->second.asList();
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
          //  m_terrain.setBasePoint(x, y, point[2].asNum());
Environment::getInstance().setBasePoint(x,y,point[2].asNum());
//System::instance()->getGraphics()->getTerrainRenderer()->m_terrain.setBasePoint(x,y,point[2].asNum());
//System::Instance()->getGraphics(x,y,point[2].asNum());
        }

    } else {
        std::cerr << "Terrain is the wrong type" << std::endl << std::flush;
    }


    }
    return we;
  }

} /* namespace Sear */

