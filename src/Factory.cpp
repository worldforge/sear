// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2004 Simon Goodall, University of Southampton

// $Id: Factory.cpp,v 1.6 2005-02-18 17:06:16 simon Exp $

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "Factory.h"

#include <Eris/View.h>
#include <Eris/Connection.h>

#include "System.h"
#include "renderers/Graphics.h"
#include "environment/Environment.h"
#include "TerrainEntity.h"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

namespace Sear {

   Eris::EntityPtr Factory::instantiate(const Atlas::Objects::Entity::GameEntity & ge, Eris::TypeInfo *type, Eris::View *view) {

std::cout << "Type: " << type->getName() << std::endl;
    if (type->isA(terrainType)) {
      return new TerrainEntity(ge->getId(), type, view);
    } else {
      return new WorldEntity(ge->getId(), type, view);
    }
  }

} /* namespace Sear */

